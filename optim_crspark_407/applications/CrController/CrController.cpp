#include "CrController.h"
#include <string.h>
#include <rtthread.h>
#include "CrView/CrShowView.h"
#include "Common/Include/CrThread.h"
#include "Common/Include/CrTime.h"
#include "Common/Include/CrSleep.h"
#include "CrMenu/MenuType.h"
#include "CrMenu/Show/ShowAction.h"
#include "CrMenu/Action/MenuDefaultAction.h"
#include "CrMenu/MenuItem/SdCardMenuItem.h"
#include "CrLcd/KnobEvent/CrKnobEvent.h"
#include "CrBeep/BeepAppInterface/CrBeepAppInterface.h"

CrPthread_t MenuSwitchThreadHanle;
struct MenuItem_t *PCurMenuArray = NULL;
struct MenuItem_t *PCurItem = NULL;
static S32 OptTick = 0;
static S32 RootShowTick = 0;
extern struct ShowPrintInfo_t ShowInfo;
static S32 StopFlag = 0;
static S32 KnobEventFd = -1;

static VOID *CrControllerHandle(VOID *Param);
//static S32 KnobInit(struct KnobAttr_t *KnobAttr ,S32 *KnobFd);
static S32 SwitchPorcess(S32 Action);

static S8 ThreadBuff[2048] = {0};

S32 CrControllerInit(S32 KnobFd)
{
    if ( KnobFd <= 0 )
        return 0;

    KnobEventFd = KnobFd;

    PCurMenuArray = (struct MenuItem_t *)InitMenuDefaultAction();
    PCurItem = PCurMenuArray;

    CrPthreadAttr_t AttrThread;
    memset(&AttrThread, 0, sizeof(AttrThread));
    CrPthreadAttrInit(&AttrThread);
    memset(ThreadBuff, THREAD_BYTE_INIT_VALUE, sizeof(ThreadBuff));
    AttrThread.stackaddr = ThreadBuff;
    AttrThread.stacksize = sizeof(ThreadBuff);
    AttrThread.schedparam.sched_priority = RT_MAIN_THREAD_PRIORITY;

    CrPthreadAttrSetDetachState(&AttrThread, PTHREAD_CREATE_DETACHED);
    //pth00
    S32 Res = CrPthreadCreate(&MenuSwitchThreadHanle, &AttrThread, CrControllerHandle, (VOID *)&KnobEventFd);
    if( Res != 0 )
    {
        rt_kprintf(" Control create thread failed :err = %d\n", Res);
        return -1;
    }
    CrPthreadAttrDestroy(&AttrThread);

    StopFlag = 0;

    return 1;
}

VOID CrControllerUnInit()
{
    StopFlag = 1;
    //rt_thread_delay(1000);
    CrM_Sleep(1000);
    CrKnobClose(KnobEventFd);
    KnobEventFd = -1;

    return ;
}

#if 0
S32 KnobInit(struct KnobAttr_t *KnobAttr ,S32 *KnobFd)
{
    if ( !KnobAttr || !KnobFd )
        return -1;

    *KnobFd = CrKnobOpen((S8 *)"Knob", 0, 0);
    if ( *KnobFd < 0 )
        return -1;

    if ( CrKnobIoctl(*KnobFd, KNOB_CMD_SET_KNOB_ATTR, (VOID *)KnobAttr) != 1 )
        return -1;

    if ( CrKnobIoctl(*KnobFd, KNOB_CMD_SCAN_START, NULL) != 1 )
        return -1;

    return 0;
}
#endif

S32 SwitchPorcess(S32 Action)
{
     struct MenuItem_t *TmpItem = NULL;
    S32 Res = 0;
#if 1
    switch ( Action )
    {
        case 1:    //Ok Action
            {
                OptTick = rt_tick_get();
                if ( (!PCurItem->OkAction) && (!PCurItem->Next) )    //返回父菜单
                {
                    VOID *TmpArray = NULL;
                    TmpItem = (struct MenuItem_t *)MoveToParant(&TmpArray, (VOID *)PCurMenuArray);
                    if ( !TmpItem )
                        return 0;

                    Res = 1;
                    PCurItem = TmpItem;
                    PCurMenuArray = (struct MenuItem_t *)TmpArray;
                    CrBeepAppShortBeeps(); /* 短鸣 */
               }
               else if( PCurItem->OkAction && PCurItem->Next )    //有子菜单
               {
                   //进入下一个菜单
                   TmpItem = (struct MenuItem_t *)PCurItem->OkAction(PCurItem->Next, PCurMenuArray);
                   if ( !TmpItem )
                       return 0;

                   Res = 1;
                   PCurMenuArray = PCurItem->Next;
                   PCurItem = TmpItem;
               }
               else if ( PCurItem->OkAction && !(PCurItem->Next) )   //OK动作有自己的内容要做，但是会返回到一定的菜单界面(父菜单和根菜单)
               {
                   S32 BackParent = 0;
                   PCurItem->OkAction((VOID *)&BackParent, NULL);

                   if ( BackParent == BACK_ROOT )
                   {
                       PCurMenuArray = (struct MenuItem_t *)MoveToRoot(); //跳到root菜单
                       PCurItem = PCurMenuArray;
                    }
                    else if ( BackParent == BACK_PARENT )
                    {
                        CrBeepAppShortBeeps();
                        VOID *TmpArray = NULL;
                        TmpItem = (struct MenuItem_t *)MoveToParant(&TmpArray, (VOID *)PCurMenuArray);
                        if ( !TmpItem )
                        {
                         //printf("*****move to next menu failed,  exit menu action thread*******\n");
                            return 0;
                        }
                        PCurItem = TmpItem;
                        PCurMenuArray = (struct MenuItem_t *)TmpArray;
                    }
                        Res = 1;
                    }

               break;
            }

       case 2:    //upaction
            {
                OptTick = rt_tick_get();
                if ( PCurItem->UpAction )
                {
                    Res = 1;
                    if ( PCurItem->UpAction != MoveToPrevItem )
                    {
                        PCurItem->UpAction(NULL, NULL);
                    }
                    else
                    {
                        TmpItem = (struct MenuItem_t *)PCurItem->UpAction((VOID*)PCurItem, (VOID*)PCurMenuArray);
                        if ( !TmpItem )
                            return 0;
                        while ( (TmpItem->HideMask & (HideMask)) != 0 )
                        {
                            TmpItem--;
                        }

                        PCurItem = TmpItem;
                    }
                }
                break;
            }
       case 3:    //upaction
            {
                OptTick = rt_tick_get();
                if ( PCurItem->DownAction )
                {
                    Res = 1;

                    if ( PCurItem->DownAction != MoveToNextItem )
                    {
                        PCurItem->DownAction(NULL, NULL);
                    }
                    else
                    {
                        TmpItem = (struct MenuItem_t *)PCurItem->DownAction((VOID*)PCurItem, (VOID*)PCurMenuArray);
                        if ( !TmpItem )
                           return 0;

                        S32 Flag = 1;
                        while ( (TmpItem->HideMask & (HideMask)) != 0 )
                        {
                            TmpItem++;
                            if ( TmpItem->ItemNameId == MENU_ITEM_FINISH)
                            {
                                Flag = 0;
                                break;
                            }
                        }

                        if ( Flag )
                        PCurItem = TmpItem;
                    }
                }
                break;
            }
        default:
            {
                //rt_kprintf("@@@@@@ handle move to root menu action @@@@\n");
                if ( (rt_tick_get() - OptTick) < MENUITEM_STAY_TIME )   //一段事件没有操作就进行转换
                    break;

                if ( PCurItem != RootMenu )
                {
                    TmpItem = (struct MenuItem_t *)MoveToRoot();
                    if ( !TmpItem )
                        return 0;

                    Res = 1;
                    PCurItem = TmpItem;
                    PCurMenuArray = PCurItem;
                    RootShowTick = rt_tick_get();

                }
                else
                {
                    if ( (rt_tick_get() - RootShowTick ) > ROOT_SHOW_INTERVAL)
                    {
                        Res = 1;
                        RootShowTick = rt_tick_get();
                    }
                }
            }
            break;
    }
#endif

    if ( PCurMenuArray == RootMenu || PCurItem == RootMenu )
    {
        OptTick -= MENUITEM_STAY_TIME;
        ClearCurrentPath();
    }

    if ( Res ){
        ShowItems(PCurMenuArray, GetItemsCount(PCurMenuArray), GetItemIndex(PCurItem, PCurMenuArray));
        //ShowItems(RootMenu,1,1);
        //rt_kprintf("-8- %d %d\n",GetItemsCount(PCurMenuArray),GetItemIndex(PCurItem, PCurMenuArray));
    }
    return Res;
}


VOID *CrControllerHandle(VOID *Param)
{
    struct KnobEvent_t KnobEvent;
    S32 *KnobFd = (S32 *)Param;
    if ( !KnobFd )
    {
        return NULL;
    }
    S32 Ret = -1;
    CrM_Sleep(3000);    //先休眠10s


    while (1)
    {

        if ( StopFlag )
            break;
        

        Ret = CrKnobIoctl(*KnobFd, KNOB_CMD_GET_KNOB_EVENT, (VOID *)&KnobEvent);

        if ( Ret <= 0  )
        {
            CrM_Sleep(10);
            continue;
        }

        if( (0 == KnobEvent.KnobButton) && ( 0 == KnobEvent.KnobMtion ) )
        {
            Ret = 0;
        }

        if ( KnobEvent.KnobButton )  //Key Down
        {
            Ret = 1;
        }

        if ( KnobEvent.KnobMtion == KNOB_MTION_DOWN )
        {
            Ret = 3;
        }

        if ( KnobEvent.KnobMtion == KNOB_MTION_UP )
        {
            Ret = 2;
        }

        SwitchPorcess(Ret);

        CrM_Sleep(10);
    }

    return NULL;
}



