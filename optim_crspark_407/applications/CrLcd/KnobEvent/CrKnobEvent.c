#include "stm32f4xx_hal.h"
#include "CrGpio/CrGpio.h"
#include "CrLcd/KnobEvent/CrKnobEvent.h"
#include <string.h>
#include <stdio.h>
#include "Common/Include/CrTimer.h"

typedef S32 (*KnobIoctlFunc)(VOID *);

static VOID CrKnobEventScan(VOID *Args);

static S32 KnobCmdGetKnobEvent(VOID *Param);
static S32 KnobCmdSetKnobAttr(VOID *Param);
static S32 KnobCmdScanStart(VOID *Param);
static S32 KnobCmdScanStop(VOID *Param);

//Ioconctl 转换表,CrKnobIoctl 调用
static const KnobIoctlFunc KnobCommandFunc[ KNOB_CMD_MAX_NUM ] = {
    [KNOB_CMD_GET_KNOB_EVENT]   = KnobCmdGetKnobEvent,  //获取旋钮事件
    [KNOB_CMD_SET_KNOB_ATTR]    = KnobCmdSetKnobAttr,   //设置属性
    [KNOB_CMD_SCAN_START]       = KnobCmdScanStart,     //开始扫描
    [KNOB_CMD_SCAN_STOP]        = KnobCmdScanStop       //暂停扫描
};

struct Knob_t
{
    S8   Name[12];                      //旋钮器件名称
    S32  PinA_Fd;                       //A引脚 Fd
    S32  PinB_Fd;                       //B引脚 Fd
    S32  PinC_Fd;                       //C引脚 Fd
    S32  PinButtonFd;                   //Button引脚 Fd
    struct KnobAttr_t KnobAttrName;     //  引脚属性（引脚名称）
    CrTimer_t ScanTimer;               //定时器指针
};

static struct Knob_t    Knob;
static struct KnobEvent_t KnobEvent;    //保存旋钮状态

/********************************************
*Discription : 打开旋钮事件
*param
*   Name ：旋钮名称
*   Flag: NULL
*   Mode: NULL
*return：-1 On Error
         其他值为   Fd ， Knob地址
********************************************/
S32 CrKnobOpen(S8 *Name, S32 Flag, S32 Mode)
{
    if ( NULL == Name )   //!Name
        return -1;

    //判断旋钮已经被使用, 判断Name长度

    if ( 0 != strlen(Knob.Name) )
        return -1;

    if ( sizeof(Knob.Name) <= strlen(Name) )
       return -1;

    strcpy( Knob.Name, Name );
    
    CrSignalEvent_t SigEvent =
    {
        .sigev_notify = 0,
        .sigev_notify_attributes = NULL,
        .sigev_signo = 0,
        .sigev_notify_function = (VOID (*) (union SigVal_t))CrKnobEventScan,
        .sigev_value = {0},
    };

    CrClockid_t ClockId = 0;

    struct CrTimerAttr_t CrTimerInfo = {
        .Name = "rotate",
        .HardwareTimer = 0,
        .Mode = RT_TIMER_FLAG_PERIODIC
    };

    Knob.ScanTimer = &CrTimerInfo;
    CrTimerCreate(ClockId, &SigEvent, &Knob.ScanTimer);

    if ( Knob.ScanTimer == NULL )
    {
        //rt_kprintf("--------------------- CrKnobOpen create timer failed\n");
        return -1;
    }
        
    KnobEvent.KnobButton    = KNOB_BUTTON_NONE;
    KnobEvent.KnobMtion     = KNOB_MTION_NONE;

    rt_kprintf("CrKnobOpen\n\r");

    return (S32) &Knob;

}

/********************************************
*Discription : 关闭旋钮事件
*param
*   Fd ：旋钮Fd
*return：-1 On Error
         其他值为    Knob   地址
********************************************/
S32 CrKnobClose(S32 Fd)
{
    struct Knob_t * pKnob = (struct Knob_t *)Fd;

    //指针和全局变量的地址是否相同
    if(&Knob != pKnob)
        return -1;

    //判断全局变量的Name是否为非空字符串
//    if( !(0<strlen(Knob.Name)) )
    if( strlen(Knob.Name) <= 0 )
        return -1;
        
    //关定时器
    if(Knob.ScanTimer)
    {
       if( CrTimerDelete(Knob.ScanTimer) )
           printf("Delete Knob.ScanTimer\n\r");
       Knob.ScanTimer = NULL;
    }

    //关Gpio
    CrGpioClose(pKnob->PinA_Fd);
    CrGpioClose(pKnob->PinB_Fd);
    CrGpioClose(pKnob->PinButtonFd);

    memset(&Knob, 0, sizeof(struct Knob_t));

    return 1;
}

/********************************************
*Discription : Ioctl 对旋钮操作，内部对Cmd的进行转换实现
*param
*   Fd ：Knob Fd
*   Cmd: 命令类型 enum KnobCmdType_t
*return：-1 On Error
         其他值为各个命令的返回值
********************************************/
S32 CrKnobIoctl(S32 Fd, S32 Cmd, VOID *Arg)
{
    struct Knob_t *pKnob = (struct Knob_t *)Fd;

    //指针和全局变量的地址是否相同
    if(&Knob != pKnob)
        return -1;

    //判断全局变量的Name是否为非空字符串
    if( !(0<strlen(Knob.Name)) )
        return -1;

    //第三步 判断Cmd是否是有效命令
    if( (KNOB_CMD_MAX_NUM <= Cmd) || (0 > Cmd) )
        return -1;

    S32 ret = KnobCommandFunc[Cmd](Arg);
    return ret;
}


/********************************************
*Discription : Ioctl 调用 ，获取旋钮事件状态
*param
*   param ：struct KnobEvent_t 地址，获取旋钮事件内容
*   return：0 On Error
            1 On Succeed
********************************************/
S32 KnobCmdGetKnobEvent( VOID * Param ){
    //rt_kprintf("start GetKnobEvent\n\r");
    if ( !Param )
        return 0;

    struct KnobEvent_t * pKnobEvent = (struct KnobEvent_t *)Param;
    pKnobEvent->KnobMtion = KnobEvent.KnobMtion;
    pKnobEvent->KnobButton = KnobEvent.KnobButton;

    KnobEvent.KnobButton    = KNOB_BUTTON_NONE;
    KnobEvent.KnobMtion     = KNOB_MTION_NONE;

    return 1;
}

/********************************************
*Discription : Ioctl 调用 ，设置旋钮属性（引脚名称）
*param
*   param ：struct KnobAttr_t 地址，传入旋钮各个属性
*   return：0 On Error
            1 On Succeed
********************************************/
S32 KnobCmdSetKnobAttr(VOID * Param){

    struct PinInitParam_t pin_init = {
        .Mode = GPIO_MODE_INPUT,
        .Pull = GPIO_PULLUP,
        .Speed = GPIO_SPEED_FREQ_HIGH
    };

    if ( !Param )
        return 0;

    struct KnobAttr_t *KnobAttrp =  (struct KnobAttr_t *)Param;

    //open GPIO 判断GPIO打开失败的情况，有任意一个失败都初始化失败
    Knob.PinA_Fd    = CrGpioOpen((S8 *)KnobAttrp->PinA_Name, (U32)NULL, (U32)NULL);
    if( 0 > Knob.PinA_Fd )
        return 0;
        
    Knob.PinB_Fd    = CrGpioOpen((S8 *)KnobAttrp->PinB_Name, (U32)NULL, (U32)NULL);
    if ( 0 > Knob.PinB_Fd )
    {
        CrGpioClose(Knob.PinA_Fd);
        Knob.PinA_Fd = 0;

        return 0;
    }

    Knob.PinButtonFd    = CrGpioOpen((S8 *)KnobAttrp->PinButtonName, (U32)NULL, (U32)NULL);
    if( 0 > Knob.PinButtonFd )
    {
        CrGpioClose(Knob.PinA_Fd);
        Knob.PinA_Fd = 0;
        CrGpioClose(Knob.PinB_Fd);
        Knob.PinB_Fd = 0;

        return 0;
    }

    memcpy(&Knob.KnobAttrName, KnobAttrp, sizeof(struct KnobAttr_t));

    //init Gpio 
    CrGpioIoctl(Knob.PinA_Fd, GPIO_INIT, (void *)&pin_init);
    CrGpioIoctl(Knob.PinB_Fd, GPIO_INIT, (void *)&pin_init);
    CrGpioIoctl(Knob.PinButtonFd, GPIO_INIT, (void *)&pin_init);

    return 1;
}

/********************************************
*Discription : Ioctl 调用 ，停止旋钮扫描
*param
*   param ：NULL 暂无参数，和Ioctl转换表类型统一
*   return：0 On Error
            1 On Succeed
********************************************/
S32 KnobCmdScanStart(VOID *Param){

    KnobEvent.KnobButton    = KNOB_BUTTON_NONE;
    KnobEvent.KnobMtion     = KNOB_MTION_NONE;

    if ( Knob.ScanTimer )
    {
        CrTimerSpec_t OldTimerSpac = {0};
        CrTimerSpec_t NewTimerSpac = {{0, 1000000}};
        CrTimerSetTime(Knob.ScanTimer, 1, &NewTimerSpac, &OldTimerSpac);
    }
    else
        return 0;

    return 1;
}

/********************************************
*Discription : Ioctl 调用 ，开始旋钮扫描
*param
*   param ：NULL 暂无参数，和Ioctl转换表类型统一
*   return：-1       定时器未创建
            0       失败
            1       成功
********************************************/
S32 KnobCmdScanStop(VOID *Param){

    KnobEvent.KnobButton    = KNOB_BUTTON_NONE;
    KnobEvent.KnobMtion     = KNOB_MTION_NONE;
//    rt_kprintf("ScanTimer Stop!!\n\r");

    if ( Knob.ScanTimer )
    {
//        return (!rt_timer_stop(Knob.ScanTimer)) ? 1 : 0;
    }
    else
        return -1;
}


/********************************************
*Discription : 旋钮扫描函数，定时器回调执行
*param
*   args ：NULL 暂无参数，定时器回调
*   return：void
********************************************/
VOID CrKnobEventScan(VOID *Args)
{
//    static S8 ClockWiseSpinFlag = 0, CounterSpinFlag = 0;

    static S32 ScanEvent = 0;
    static uint32_t StartTick = 0;
    static S32 PinALast = 0;                        //EC11的A引脚上一次电平状态
//    static S32 PinBLast = 0;                        //EC11的B引脚上一次电平状态
    static S32 PinANow = 1;                        //EC11的A引脚上一次电平状态
    static S32 PinBNow = 1;                        //EC11的B引脚上一次电平状态
    S32 KnobButton = 0;
    static S32 ButtonLaststatus = 0;

    struct PinParam_t KnobPinA;
    struct PinParam_t KnobPinB;
    struct PinParam_t KnobPinButton;

    CrGpioRead(Knob.PinA_Fd, (S8 *)&KnobPinA, ( S32 )NULL);
    CrGpioRead(Knob.PinB_Fd, (S8 *)&KnobPinB, ( S32 )NULL);
    PinANow = KnobPinA.PinState;
    PinBNow	= KnobPinB.PinState;

    //Get Motion Event
    if ( PinANow!=PinALast ) //只需要采集A的上升沿或下降沿的任意一个状态，若A下降沿时B为1，正转
    {
        if( GPIO_PIN_RESET == PinANow )
        {
            StartTick = HAL_GetTick();
            if( GPIO_PIN_SET == PinBNow )
            {
                ScanEvent = 1;
            }
            else
            {
                ScanEvent = -1;
            }
        }
        PinALast = PinANow;
//        PinBLast = PinBNow;
    }
    else if ( (GPIO_PIN_SET==PinANow) && (GPIO_PIN_SET==PinBNow) )
    {
        if ( ( 5 < (HAL_GetTick() - StartTick) ) && ( 500>(HAL_GetTick() - StartTick)) )
       {
            if ( 1==ScanEvent )
            {
                KnobEvent.KnobMtion = KNOB_MTION_DOWN ;
            }
            else if ( -1==ScanEvent )
            {
                KnobEvent.KnobMtion = KNOB_MTION_UP ;
            }

            ScanEvent = 0;
        }
    }

    //Get BottunEvent
    CrGpioRead(Knob.PinButtonFd, (S8 *)&KnobPinButton, ( S32 )NULL);
    KnobButton = KnobPinButton.PinState;

    if ( GPIO_PIN_RESET == KnobButton )
    {
        if ( ButtonLaststatus == GPIO_PIN_SET )
        {
            KnobEvent.KnobButton = KNOB_BUTTON_DOWN; //按键按下
            ButtonLaststatus = GPIO_PIN_RESET;
        }
    }
    else
    {
        if ( GPIO_PIN_RESET == ButtonLaststatus )
        {
            ButtonLaststatus = GPIO_PIN_SET; 
        }
    }

}


#ifdef KNOBEVENT_TEST

S32 Knob_FD = NULL;


void TimerContrl(VOID *args)
{
    static S32 time = 0;
    S32 ret;
    rt_kprintf("Scan Timer Contrl!!\n\r");
    if( time++%2 )
    {
        ret = CrKnobIoctl(Knob_FD, KNOB_CMD_SCAN_START, (VOID *)NULL);
        switch( ret )
        {
            case 0:
                rt_kprintf("Scan Timer not Open\n\r");
                break;
            case 1:
                rt_kprintf("Scan Timer Start Succeed\n\r");
                break;
        }
    }
    else
    {
        ret = CrKnobIoctl(Knob_FD, KNOB_CMD_SCAN_STOP, (VOID *)NULL);
        switch( ret )
        {
            case -1:
                rt_kprintf("Scan Timer not Create\n\r");
                break;
            case 0:
                rt_kprintf("Scan Timer STOP Error\n\r");
                break;
            case 1:
                rt_kprintf("Scan Timer STOP Succeed\n\r");
                break;
        }
    }
}

VOID SwitchTimerAsPeriod(VOID)
{
    CrSignalEvent_t SigEvent =
    {
        .sigev_notify = 0,
        .sigev_notify_attributes = NULL,
        .sigev_signo = 0,
        .sigev_notify_function = TimerContrl,
        .sigev_value = {0},
    };

    CrClockid_t ClockId = 0;

    struct CrTimerAttr_t CrTimerInfo = {
        .Name = "TimeC",
        .HardwareTimer = 0,
        .Mode = RT_TIMER_FLAG_PERIODIC
    };

    CrTimer_t Crtimer = &CrTimerInfo;
    CrTimerCreate(ClockId, &SigEvent, &Crtimer);
    if ( Crtimer == NULL )
    {
        return ;
    }

    //开启定时器
    CrTimerSpec_t OldTimerSpac = {0};
    CrTimerSpec_t NewTimerSpac = {{5000}};
    CrTimerSetTime(Crtimer, 1, &NewTimerSpac, &OldTimerSpac);
}


VOID CrKnobEventTestMain(){

    struct KnobEvent_t KnobEvent;
    S32 Position = 0;
    S32 ret;
    struct KnobAttr_t KnobAttr = {
        .PinA_Name = EC11_PINA_NAME,
        .PinB_Name = EC11_PINB_NAME,
        .PinButtonName = EC11_PINBOTTUN_NAME
    };

    //1 Create Timer to Test switch ScanTimer By Cycle
    SwitchTimerAsPeriod();

    //2 Open Knob
    Knob_FD = CrKnobOpen("Knob_test", NULL, NULL);
    //3 Set Pin Name
    ret = CrKnobIoctl(Knob_FD, KNOB_CMD_SET_KNOB_ATTR, (VOID *)&KnobAttr);
    //4 Start Timer to Scan
    ret = CrKnobIoctl(Knob_FD, KNOB_CMD_SCAN_START, NULL);

    while(1)
    {

        //5 Get Input Key Value
        S32 ret = CrKnobIoctl(Knob_FD, KNOB_CMD_GET_KNOB_EVENT, (VOID *)&KnobEvent);

        //6 Button Action
        if(KNOB_BUTTON_DOWN ==KnobEvent.KnobButton)
            printf("Input Enter \n\r");

        //7 Motion Action
        switch(KnobEvent.KnobMtion)
        {

            case KNOB_MTION_DOWN:   printf("Position = %d --->\n\r",Position++);
                                    break;
            case KNOB_MTION_UP:
                                    printf("Position = %d <---\n\r",Position--);
                                    break;
        }
    }

}



#endif


