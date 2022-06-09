#include "GcodeM48Handle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "CrInc/CrMsgType.h"
#include "CrGcode/GcodeHandle/GcodeType.h"
#include "CrGcode/GcodeHandle/GcodeCommonFunc.h"
#include "CrModel/CrMotion.h"
#include "CrMotion/CrMotionPlanner.h"
#include "Common/Include/CrGlobal.h"
#include "Common/Include/CrTime.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "CrHeater/Pid/CrPid.h"


#define CMD_M48 "M48"

/*******额外定义的变量和函数********/
//FixME
#define DELTA_PRINTABLE_RADIUS          //暂时未找到合适的定义数值
#define WITHIN(N,L,H)       ((N) >= (L) && (N) <= (H))


S16 FeedratePercentage = 100;           //未定义的外部变量
static CrMsg_t LineMsgQueue = NULL;     //用于发送消息
static BOOL BedLeveingActive = true;    //当前调平的状态
extern MotionCoord_t ProbeOffset;


BOOL ProbeCanReach( XYFloat_t Position)       //是否可达函数
{
    if ( !WITHIN(Position.Y, Y_MIN_POS, Y_MAX_POS) )
    {
        return false;
    }
    if ( !WITHIN(Position.X, X_MIN_POS, X_MAX_POS) )
    {
        return false;
    }
    return true;
}

VOID SetBedLeveling(const BOOL Enable)
{

}

FLOAT ProbeAtPoint(XYFloat_t Position,ProbePtRaise RaiseAfter,U8 VerboseLevel,BOOL ProbeRelative,BOOL SanityCheck)      //探针探测给定xy的点，返回z的位置
{
    return 0.0;
}

VOID DevReport(const BOOL Verbose, FLOAT Mean, FLOAT Sigma, FLOAT Min, FLOAT Max, const BOOL Final = false)
{
    if ( Verbose )
    {
        printf("Mean: %.6f",Mean);
        if ( !Final )
        {
            printf(" Sigma: %.6f",Sigma);
        }
        printf(" Min: %.3f",Min);
        printf(" Max: %.3f",Max);
        printf(" Range: %.3f",Max-Min);
        if ( Final )
        {
            printf("\n");
        }
    }
    if ( Final )
    {
        printf("Standard Deviation: %.6f\n",Sigma);
        printf("\n");
    }
}

BOOL ProbeStow()    //探针缩回函数的接口
{
    return false;
}

BOOL IsEnableAutoLevel()    //是否支持调平函数
{
    return EnableAutoLevel;
}
/*******额外定义的变量和函数********/

S32 GcodeM48Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
    {
        return 0;
    }

    S8 Len =  strlen(CmdBuff);
    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( Len <= 1 || CmdBuff[0] != GCODE_CMD_M )
    {
        return 0;
    }

    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0)    //获取命令失败
    {
        return 0;
    }
    if ( strcmp(Cmd,CMD_M48) != 0 )     //传入命令不是M48命令
    {
        return 0;
    }

    S8 Offset = strlen(CMD_M48) + sizeof(S8);
    struct ArgM48_t *CmdArg = (struct ArgM48_t *)Arg;

    S8 Opt[20] = {0};
    S8 OptLen = 0;
    while ( Offset < Len )
    {
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
        {
            break;
        }

        OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            printf("Gcode M48 has not param opt = %s\n", CmdBuff);
        }

        switch ( Opt[0] )
        {
            case OPT_E:
            {
                CmdArg->HasE = true;
                if ( OptLen > 1 )
                {
                    CmdArg->E = !!atoi(Opt + 1);
                }
                else
                {
                    CmdArg->E = true;       //没有E字段时的判断写在了执行流程中，赋false
                }
                break;
            }
            case OPT_L:
            {
                CmdArg->HasL = true;
                if ( OptLen > 1 )
                {
                    CmdArg->L = atoi( Opt + 1);
                    if ( CmdArg->L > 255 )  //规定取值范围
                        CmdArg->L = 255;
                    if ( CmdArg->L < 0 )
                        CmdArg->L = 0;
                }
                else
                {
                    CmdArg->L = 0;
                }
                break;
            }
            case OPT_P:
            {
                CmdArg->HasP = true;
                if ( OptLen > 1 )
                {
                    CmdArg->P = atoi( Opt + 1);
                }
                else
                {
                    CmdArg->P = 0;      //有P没数值给0
                }
                break;
            }
            case OPT_S:
            {
                CmdArg->HasS = true;
                if ( OptLen > 1 )
                {
                    CmdArg->S = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->S = true;
                }
                break;
            }
            case OPT_V:
            {
                CmdArg->HasV = true;
                if ( OptLen > 1 )
                {
                    CmdArg->V = atoi( Opt + 1);
                }
                else
                {
                    CmdArg->V = 0;
                }
                break;
            }
            case OPT_X:
            {
                CmdArg->HasX = true;
                if ( OptLen > 1 )
                {
                    CmdArg->X = atof(Opt + 1);     //转换成double类型，可用atoff转换成float类型
                }
                else
                {
                    CmdArg->X = 0;
                }
                break;
            }
            case OPT_Y:
            {
                CmdArg->HasY = true;
                if ( OptLen > 1 )
                {
                    CmdArg->Y = atof(Opt + 1);     //转换成double类型，可用atoff转换成float类型
                }
                else
                {
                    CmdArg->Y = 0;
                }
                break;
            }
            default:
                break;
        }
        Offset += strlen(Opt) + sizeof(S8);
    }
    return 1;
}
/*
 * 传入的各参数意义
 * P:采样点数量（4-50，默认为10）
 * X:样本X位置
 * Y:样本Y位置
 * V:详细级别（0-4，默认为1）
 * E:每个采样点间探头上升的类型
 * L:要探测的leg的数量
 * S:要探测的图案
 */
S32 GcodeM48Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !MotionModel )
    {
        return 0;
    }

    struct ArgM48_t *Param = (struct ArgM48_t *)ArgPtr;
    struct MsgLine_t LineMsg = {0};
    //定义发送消息的队列名称
    LineMsgQueue = ViewRecvQueueOpen((S8 *)MESSAGE_LINE_INFO_QUEUE, sizeof(struct MsgLine_t));

    //1、判断是否在home点，如果没有归零直接退出
    struct MotionPositionMsg_t MotionPos = MotionModel->GetCurPosition();
    if ( MotionPos.HomeX != true || MotionPos.HomeY != true || MotionPos.HomeZ != true )
    {
        //输出提示没有进行归零
        struct MsgLine_t LineMsg = {0};
        LineMsg.MsgType = SPECIAL_MSG;
        strcpy(LineMsg.Info,"M48 execution failed. It needs to be home");

        if ( LineMsgQueue )
        {
            //参数1是优先级
            CrMsgSend(LineMsgQueue, (S8 *)&LineMsg, sizeof(LineMsg), 1);
        }
        return 0;
    }

    //2、各个变量的赋值判断
    //2.1、V值的范围判断，不在范围内则输出提示并退出，在范围内则输出提示
    if ( !Param->HasV ) //如果没有输入V字符也默认赋1
    {
        Param->V = 1;
    }

    S8 VerboseLevel = Param->V ;
    if ( !(Param->V <= 4 && Param->V >= 0) )
    {
        printf("Verbose level implausible (0-4).");
        return 0;
    }
    if ( Param->V > 0 )
    {
        printf("M48 Z-Probe Repeatability Test");
    }

    //2.2、P值的范围判断，不在范围内则输出提示并退出
    if ( !Param->HasP ) //未输入P字符赋值10
    {
        Param->P = 10;
    }
    const S8 NumSamples = Param->P;
    if ( NumSamples > 50 && NumSamples < 4 )
    {
        printf("Sample size not plausible (4-50).");
        return 0;
    }

    //2.3、判断E值来对枚举变量赋值
    if ( !Param->HasE ) //未输入E值赋值为false
    {
        Param->E = false;
    }
    const ProbePtRaise RaiseAfter = Param->E ? PROBE_PT_STOW : PROBE_PT_RAISE;

    //2.4、根据x和y的值来对测试用的位置点赋值，如果是不可到达的点则输出提示并返回
    XYFloat_t TestPosition = {0};
    if ( !Param->HasX ) //未输入x坐标则使用当前坐标
    {
        TestPosition.X = MotionPos.PosX + ProbeOffset.CoorX;
        TestPosition.Y = MotionPos.PosY + ProbeOffset.CoorY;
    }
    else
    {
        TestPosition.X = Param->X;
        TestPosition.Y = Param->Y;
    }

    if ( !ProbeCanReach(TestPosition) )
    {
        printf("? (X,Y) out of bounds.\n");
        return 0;
    }

    //2.5、L值的范围判断，如果超过15则输出提示并退出
    BOOL HasL = Param->HasL;
    U8 NumLegs = HasL ? Param->L : 0;
    if ( NumLegs > 15 )
    {
        printf("?Legs of movement implausible (0-15).\n");
        return 0;
    }
    if ( NumLegs == 1 )
    {
        NumLegs = 2;
    }

    //2.6、S值的范围判断
    BOOL SchizoidFlag = false;
    if ( false == Param->HasS ) //没有传递S值
    {
        SchizoidFlag = false;
    }
    else
    {
        SchizoidFlag = Param->S;
    }
    BOOL WasEnabled = false;
    if ( SchizoidFlag && !HasL )
    {
        NumLegs = 7;
    }
    if ( VerboseLevel > 2 )
    {
        printf("Positioning the probe...\n");
    }
    //3、如果热床支持调平，则进行调平操作
    if ( IsEnableAutoLevel() )
    {
        WasEnabled = BedLeveingActive;
        SetBedLeveling(true);
    }
    //4、记住进料速度等参数
    FeedrateAttr_t FrAttr = {0};
    FLOAT SavedFeedrate = 0;
    S32   SavedFeedratePercentage = 0;
    FeedrateAttrGet(&FrAttr);
    SavedFeedrate = FrAttr.Feedrate;
    SavedFeedratePercentage = FeedratePercentage;
    FeedratePercentage = 100;

    //5、移动到第一个点并进行探针测试，返回z轴测试的位置
    FLOAT Mean = 0,                     // The average of all points so far, used to calculate deviation
          Sigma = 0,                    // Standard deviation of all points so far
          Min = 99999.9,                // Smallest value sampled so far
          Max = 99999.9,                // Largest value sampled so far
          SampleSet[NumSamples] = {0};  // Storage for sampled values

    const float Distance = ProbeAtPoint(TestPosition,RaiseAfter,VerboseLevel,true,true);
    BOOL ProbingGood = !isnan(Distance);

    //6、如果返回的值是数字则进行7，否则执行8
    if ( ProbingGood )
    {
        //7、设置随机数种子并进入循环,次数为P的值（同时向屏幕输出当前信息）
        CrTimeSpec_t Time = {0};
        CrGetCurrentSystemTime(&Time);
        srand(Time.tv_sec);
        FLOAT SampleSum = 0.0;

        for ( int i = 0; i < NumSamples; i++ )
        {
            //输出M48进度到屏幕，目前只输出到串口
            LineMsgQueue = ViewRecvQueueOpen((S8 *)MESSAGE_LINE_INFO_QUEUE, sizeof(struct MsgLine_t));
            strcpy(LineMsg.Info,(i+1)+int(NumSamples)+"MSG_M48_POINT");
            LineMsg.MsgType = SPECIAL_MSG;
            if ( LineMsgQueue )
            {
                CrMsgSend(LineMsgQueue, (S8 *)&LineMsg, sizeof(LineMsg), 1);
            }
            printf("%d/%dMSG_M48_POINT\n",int(i + 1),int(NumSamples));

            //7.1、如果有L值 ，随机选取方向、角度、半径，当详细级别是4时输出三个数据的信息
            if ( NumLegs )  //When there are "legs" of movement move around the point before probing
            {
                int RandMin = 0,RandMax = 0;
                S32 Dir = ((rand() % 10) > 5.0) ? -1 : 1;                         //随机选择方向
                FLOAT Angle = rand() % 360;                                       //随机角度
                #ifdef DELTA
                RandMin = int(0.1250000000 * (DELTA_PRINTABLE_RADIUS));
                RandMax = int(0.3333333333 * (DELTA_PRINTABLE_RADIUS));
                #else
                RandMin = 5;
                RandMax = int(0.125 * MIN(BED_WIDTH,BED_LENGTH));                  //width是x轴方向的大小，length是y轴方向大小
                #endif
                FLOAT Radius = (FLOAT)(RandMin + rand() % (RandMax - RandMin));    //随机半径


                if ( VerboseLevel > 3 )
                {
                    printf("Start radius:%f angle: %f dir:.",Radius,Angle);
                    if ( Dir > 0 )
                    {
                        printf("C");
                    }
                    printf("CW\n");
                }

                //7.2、进入循环，次数为leg-1，计算移动的x和y的位置并进行移动
                for ( int l = 0; l < NumLegs - 1; l++ )
                {
                    FLOAT DeltaAngle = 0;
                    if ( SchizoidFlag )
                    {
                        //描绘五角星
                        DeltaAngle = Dir * 2.0 * 72.0;
                    }
                    else
                    {
                        DeltaAngle = Dir * (FLOAT)(25 + rand() % (45 - 25));
                    }
                    Angle += DeltaAngle;

                    while ( Angle > 360.0 )
                    {
                        Angle -= 360;
                    }
                    while ( Angle < 0 )
                    {
                        Angle += 360.0;
                    }

                    XYFloat_t NozPos  = {TestPosition.X - ProbeOffset.CoorX, TestPosition.Y - ProbeOffset.CoorY};
                    XYFloat_t NextPos = {NozPos.X + (FLOAT)(cos(Angle * M_PI / 180.0f)) * Radius, NozPos.Y + (FLOAT)(sin(Angle * M_PI / 180.0f)) * Radius};

                    #ifdef DELTA
                    while ( !ProbeCanReach(NextPos) )
                    {
                        NextPos.X *= 0.8f;
                        NextPos.Y *= 0.8f;
                        if ( VerboseLevel > 3 )
                        {
                            printf("Moving inward: X%f Y%f\n",NextPos.X,NextPos.Y);
                        }
                    }
                    #elif HAS_ENDSTOPS
                    if ( MotionModel->HasEndStop() ) //如果有限位开关
                    {
                        LIMIT(NextPos.X, X_MIN_POS, X_MAX_POS);
                        LIMIT(NextPos.Y, Y_MIN_POS, Y_MAX_POS);//此方法是软限位
                    }
                    #endif
                    if ( VerboseLevel > 3)
                    {
                        printf("Going to: X%f Y%f\n",NextPos.X,NextPos.Y);
                    }

                    //将坐标点放入移动队列
                    MotionGcode_t Cood = {0};
                    Cood.Coord.CoorX = NextPos.X;
                    Cood.Coord.CoorY = NextPos.Y;
                    Cood.Coord.CoorZ = MotionPos.PosZ;
                    Cood.Coord.CoorE = MotionPos.PosE;
                    MotionModel->PutMotionCoord(Cood);
                }
            }

            //7.3、出最里层的循环后，再次进行探针测试并得到z轴的位置进行保存，计算z轴位置的平均值、标准偏差
            FLOAT PointZ = ProbeAtPoint(TestPosition, RaiseAfter, 0, true, true);

            ProbingGood = !isnan(PointZ);
            if ( !ProbingGood )
            {
                break;
            }

            SampleSet[i] = PointZ;
            //记录PointZ的最大值和最小值
            if ( PointZ < Min ) Min = PointZ;
            if ( PointZ > Max ) Max = PointZ;

            //记录PointZ的总和跟平均值
            SampleSum += PointZ;
            Mean = SampleSum / (i + 1);

            FLOAT DevSum = 0;
            for( int j = 0; j < i; j++ )
            {
                DevSum += powf((SampleSet[j] - Mean),2);
            }
            Sigma = sqrtf(DevSum / ( i + 1));

            //7.4、如果详细等级大于1向串口输出部分数据信息，一次循环完成
            if ( VerboseLevel > 1 )
            {
                printf("%d of %d: z: %.3f ",i+1,NumSamples,PointZ);
                DevReport(VerboseLevel > 2, Mean, Sigma, Min, Max);
                printf("\n");
            }
        }
    }

    //8、收回探针，如果之前的探针测试有值输出完成信息
    ProbeStow();
    if ( ProbingGood )
    {
        printf("Finished!\n");
        DevReport(VerboseLevel > 0, Mean, Sigma, Min, Max, true);

        //如果有屏幕输出，输出信息到屏幕
        LineMsg.MsgType = SPECIAL_MSG;
        char String[20] ={0};
        sprintf( String,"Devition :%.2f",Sigma);
        strcpy(LineMsg.Info,String);
        LineMsg.Finished = 1;
        if ( LineMsgQueue )
        {
            CrMsgSend(LineMsgQueue, (S8 *)&LineMsg, sizeof(LineMsg), 1);
        }

    }

    //9、恢复进料速度等参数，如果有热床调平重新进行热床调平并报告当前位置
    FrAttr.Feedrate = SavedFeedrate;
    FeedratePercentage = SavedFeedratePercentage;
    FeedrateAttrSet(&FrAttr);

    if ( IsEnableAutoLevel() )
    {
        SetBedLeveling(WasEnabled);
    }

    MotionPositionMsg_t CurPos = MotionModel->GetCurPosition();
    printf("Motion current Position X:%f Y:%f Z:%f.\n",CurPos.PosX,CurPos.PosY,CurPos.PosZ);

    return 1;
}
S32 GcodeM48Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
