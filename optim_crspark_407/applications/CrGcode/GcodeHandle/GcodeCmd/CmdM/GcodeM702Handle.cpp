#include "GcodeM702Handle.h"
#include <stdlib.h>
#include <stdio.h>  /*sprintf*/
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"
#include "CrModel/CrMotion.h"
#include "CrModel/CrPrinter.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "CrMotion/CrStepper.h"
#include "CrMotion/StepperApi.h"
#include "CrMenu/MenuType.h"
#include "CrMenu/language/language.h"
#include "Common/Include/CrSleep.h"

#define CMD_M702 ("M702")

#ifndef X_MIN_POS
#define X_MIN_POS X_AXIS_MIN_SIZE
#endif

#ifndef Y_MAX_POS
#define Y_MAX_POS Y_AXIS_MAX_SIZE
#endif

#ifndef Z_MAX_POS
#define Z_MAX_POS Z_AXIS_MAX_SIZE
#endif

#ifndef NOZZLE_PARK_POINT
#define NOZZLE_PARK_POINT { (X_MIN_POS + 10), (Y_MAX_POS - 10), 20 , 0}
#endif

#ifndef EXTRUDE_MINTEMP
#define EXTRUDE_MINTEMP 185
#endif

#define FILAMENT_UNLOAD_RETRACT_LENGTH      13  // (mm) Unload initial retract length.
#define FILAMENT_UNLOAD_DELAY             5000  // (ms) Delay for the filament to cool after retract.
#define FILAMENT_UNLOAD_PURGE_LENGTH         8  // (mm) An unretract is done, then this length is purged.

//VOID GcodeM702Init(VOID *Gcode, VOID *Nozzle)
//{
//    GcodeModel = (CrGcode *)Gcode;
//    NozModel = (CrHeaterManager *)Nozzle;
//
//    return ;
//}

S32 GcodeM702Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S8 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )   //Gcode命令的长度大于1,且是m
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)  //获取命令失败
        return 0;

    if ( strcmp(Cmd, CMD_M702) != 0 )  //传进来的命令不是G0命令
        return 0;

    S8 Offset = strlen(CMD_M702) + sizeof(S8); //加1是越过分隔符
    struct ArgM702_t *CmdArg = (struct ArgM702_t *)Arg;

    S8 Opt[20] = {0};
    S8 OptLen = 0;

    while ( Offset < Len )
    {
        memset(Opt, 0, sizeof(Opt));
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
            break;    //获取参数失败

        OptLen = strlen(Opt);

        switch ( Opt[0] )
        {
            case OPT_U:
                CmdArg->HasU = true;

                if (OptLen > 1)
                    CmdArg->U = abs(atoi(Opt + 1));
            break;

            case OPT_T:
                CmdArg->HasT = true;

                if (OptLen > 1)
                    CmdArg->T = atoi(Opt + 1);
            break;

            case OPT_Z:
                CmdArg->HasZ = true;

                if (OptLen > 1)
                    CmdArg->Z = atof(Opt + 1);
            break;
        }
        Offset += strlen(Opt) + sizeof(S8);
    }

    return 1;
}

static VOID LcdShowMsg(const S8 *Ptr)
{
    CrMsg_t LineMsgQueue = NULL;

    if ( !(LineMsgQueue = ViewRecvQueueOpen((S8 *)MESSAGE_LINE_INFO_QUEUE, sizeof(struct MsgLine_t))))
    {
        rt_kprintf("Open LineMsgQueue Fail.\n");
        return ;
    }

    struct MsgLine_t LineMsg = {0};
    LineMsg.MsgType = SPECIAL_MSG;

    sprintf(LineMsg.Info,  "%s", Ptr);
    if ( LineMsgQueue )
        CrMsgSend(LineMsgQueue, (S8 *)&LineMsg, sizeof(LineMsg), 1);

    LineMsg.MsgType = SPECIAL_MSG;
    strcpy(LineMsg.Info, "");
    LineMsg.Finished = 1;

    if ( LineMsgQueue )
        CrMsgSend(LineMsgQueue, (S8 *)&LineMsg, sizeof(LineMsg), 1);
}

static void WaitNozTemp()
{
    S8 WaitHeating = 1;
    if ( GcodeModel )
        GcodeModel->StartWaiting();

    S8 RecvBuf[40] = {0};

    if ( Languages::FindMenuItemNameById(NOZ_HEARTING_NAME_ID, RecvBuf, sizeof(RecvBuf)) > 0 )
        LcdShowMsg(RecvBuf);

    HotEndArr[0]->SetWaitForHeat(true);

    while ( WaitHeating && HotEndArr[0]->SetWaitForHeat(-1) )
    {
        if( GcodeModel )
            if ( !(GcodeModel->IsWaiting()) )
                break;

        if ( HotEndArr[0]->ReachedTargetTemper() )
        {
            S32 i = 10;
            do
            {
                if( GcodeModel )
                    if ( !(GcodeModel->IsWaiting()) )
                        break;

                rt_thread_delay(300);
            }while ( i-- && HotEndArr[0]->ReachedTargetTemper() );
            if (i <= 0 )
            {
                WaitHeating = 0;
            }
        }
        rt_thread_delay(300);
    }

    if ( GcodeModel )
        GcodeModel->StopWaiting();
}

static BOOL UnLaodFilament(float Length)
{
    if ( !HotEndArr[0] )  return false;

    if ( HotEndArr[0]->CrHeaterMagagerGetAttr().CurTemper < EXTRUDE_MINTEMP ||
            HotEndArr[0]->CrHeaterMagagerGetAttr().TargetTemper < EXTRUDE_MINTEMP )
    {
        S8 RecvBuf[40] = {0};

        if ( Languages::FindMenuItemNameById(MOVE_AXIS_ITEM_TOOCOLD_NAME_ID, RecvBuf, sizeof(RecvBuf)) > 0 )
            LcdShowMsg(RecvBuf);

        return false;
    }
    else if ( !HotEndArr[0]->CrHeaterMagagerGetAttr().ReachedTargetTemper )
        WaitNozTemp();

    struct MotionGcode_t Target ={0};
    Target.Coord = MotionModel->GetLatestCoord();

    Target.Coord.CoorE -= FILAMENT_UNLOAD_RETRACT_LENGTH;
    Target.RetractFlag = true;
    Target.FeedRate = 10.0f;
    MotionModel->PutMotionCoord(Target);

    CrM_Sleep(FILAMENT_UNLOAD_DELAY);

    Target.Coord = MotionModel->GetLatestCoord();

    Target.Coord.CoorE -= (FILAMENT_UNLOAD_PURGE_LENGTH + FILAMENT_UNLOAD_RETRACT_LENGTH) ;
    Target.RetractFlag = true;
    Target.FeedRate = 10.0f;
    MotionModel->PutMotionCoord(Target);

    Target.Coord = MotionModel->GetLatestCoord();
    Target.Coord.CoorE -= Length ;
    Target.RetractFlag = true;
    Target.FeedRate = 10.0f;
    MotionModel->PutMotionCoord(Target);

    /* waitting for motion finish.*/
    while ( !CrBlockIsFinished() )
        CrM_Sleep(10);

    /*Disable E steppers for manual change*/
    MotionModel->DisableStepper(StepperE);

    return true;
}

S32 GcodeM702Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !MotionModel || (HOTENDS < 1)  || !HotEndArr[0] )
        return 0;

    struct ArgM701_t *Param = (struct ArgM701_t *)ArgPtr;
    struct Coordinate3d_t ParkPoint = NOZZLE_PARK_POINT;

    if ( !IsHome() )
        ParkPoint.Z = 0;

    if ( Param->HasZ )
        ParkPoint.Z = Param->Z;

    struct MotionGcode_t Target ={0};

    if ( ParkPoint.Z > 0 )
    {
        Target.Coord = MotionModel->GetLatestCoord();

        Target.Coord.CoorZ =  ((Target.Coord.CoorZ + ParkPoint.Z) < Z_MAX_POS) \
                               ? Target.Coord.CoorZ + ParkPoint.Z : Z_MAX_POS;
        Target.FeedRate = 5.0f;
        MotionModel->PutMotionCoord(Target);
    }

    UnLaodFilament(Param->L);

    if (ParkPoint.Z > 0)
    {
        Target.Coord = MotionModel->GetLatestCoord();

        Target.Coord.CoorZ =  ((Target.Coord.CoorZ - ParkPoint.Z) < 0) \
                               ? Target.Coord.CoorZ - ParkPoint.Z : 0;
        Target.FeedRate = 5.0f;
        MotionModel->PutMotionCoord(Target);
    }

    return 1;
}

S32 GcodeM702Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
