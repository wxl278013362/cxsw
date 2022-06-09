#include "GcodeG6Handle.h"

#include "CrGcode/GcodeHandle/GcodeType.h"
#include "CrGcode/GcodeHandle/GcodeCommonFunc.h"
#include "CrModel/CrMotion.h"
#include "Common/Include/CrTime.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define CMD_G6 "G6"

/***暂未实现的外部变量和方法接口****/
struct Planner_t
{
  BOOL LastPageDirX;        //X轴方向
  BOOL LastPageDirY;        //Y轴方向
  BOOL LastPageDirZ;        //Z轴方向
  BOOL LastPageDirE;        //E轴方向
  float LastPageStepRate;   //步进速度
};
struct Planner_t Planner;       //保存值的结构体
#define DIRECTIONAL false          //方向
U16 TotalSteps;
U32 PreviousMoveMs;             //步进计时器

// Delay for delivery of first block to the stepper ISR, if the queue contains 2 or
// fewer movements. The delay is measured in milliseconds, and must be less than 250ms
#define BLOCK_DELAY_FOR_1ST_MOVE 100

struct Block_t
{
    volatile U8 Flag;           // Block flags
    U8 FanSpeed;
    U8 Extruder;
    U8 BlockBufferHead;         // Index of the next block to be pushed
    U8 BlockBufferTail;         // Index of the busy block, if any
    U8 DelayBeforeDelivering;   // This counter delays delivery of blocks when queue becomes empty to allow the opportunity of merging blocks
    U32 PageIndex;              // Page index used for direct stepping
    U32 StepEventCount;         // The number of step events required to complete this block
    U32 InitialRate;            // The jerk-adjusted step rate at start of block
    U32 FinalRate;              // The minimal rate at exit
    U32 NominalRate;            // The nominal step rate for this block in step_events/sec

    U32 AccelerateUntil;        // The index of the step event on which to stop acceleration
    U32 DecelereateAfter;       // The index of the step event on which to start decelerating
    U8 DirectionBits;           // The direction bit set for this block (refers to *_DIRECTION_BIT in config.h)
};

Block_t * GetNextFreeBlock(U8 NextBufferHead)
{
    //FIXME 定义一个环形队列数组并从中读取下一个块（待补充）
    Block_t Blook = {0};
    return &Blook;
}

void BufferPage(const U32 PageIdx, const U8 Extruder, const U16 NumSteps)
{
    if ( !Planner.LastPageStepRate )
    {
        //FIXME:进入会杀死整个程序
        return;
    }

    U8 NextBufferHead;
    Block_t *Block = GetNextFreeBlock(NextBufferHead);

    //对Block赋值
    Block->Flag = 4;    //是枚举变量BlockFlag中BLOCK_FLAG_IS_PAGE的值
    Block->Extruder         = Extruder;
    Block->PageIndex        = PageIdx;
    Block->StepEventCount   = NumSteps;
    Block->InitialRate      = Planner.LastPageStepRate;
    Block->FinalRate        = Planner.LastPageStepRate;
    Block->NominalRate      = Planner.LastPageStepRate;
    Block->AccelerateUntil  = 0;
    Block->DecelereateAfter = Block->StepEventCount;
    Block->DirectionBits    = 0;

    if ( !DIRECTIONAL )
    {
        //FIXME:更新四个方向的值
    }

    if ( Block->BlockBufferHead == Block->BlockBufferTail )
    {
        Block->DelayBeforeDelivering = BLOCK_DELAY_FOR_1ST_MOVE;
    }

    // Move buffer head
    Block->BlockBufferHead = NextBufferHead;

    //使能所有电机
    MotionModel->EnableStepper(StepperX);
    MotionModel->EnableStepper(StepperY);
    MotionModel->EnableStepper(StepperZ);
    MotionModel->EnableStepper(StepperE);

    //唤醒电机(需要补充)

}

void ResetStepperTimeOut()      //重置步进计时器
{
    CrTimeSpec_t *Time;
    PreviousMoveMs = CrGetCurrentSystemTime(Time);
}

U32 GetPreviousMoveMs()
{
    return PreviousMoveMs;
}

/***暂未实现的外部变量和方法接口****/

S32 GcodeG6Parse(const S8 *CmdBuff, VOID *Arg)
{
    /* G6的参数E I R S X Y Z*/
    if ( !CmdBuff || !Arg )
    {
        return 0;
    }

    S32 Lenth = strlen(CmdBuff);
    if ( Lenth <= 1 || CmdBuff[0] != GCODE_CMD_G )  //命令长度有效且是G开头的命令
    {
        return 0;
    }

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )    //获取命令失败
    {
        return 0;
    }

    if ( strcmp(Cmd,CMD_G6) != 0 )          //确保是G6命令
    {
        return 0;
    }

    S32 Offset = strlen(CMD_G6) + sizeof(S8);
    struct ArgG6_t *CmdArg = (struct ArgG6_t *)Arg;
    S8 Opt[20] = {0};

    while ( Offset < Lenth ) //从命令行中获取所有参数
    {
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )    //获取参数失败
        {
            break;
        }

        S32 OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            printf("Gcode G6 has no param opt = %s\n",CmdBuff);
        }

        switch ( Opt[0] )
        {
            case OPT_E:
            {
                CmdArg->HasE = 1;
                if ( OptLen > 1 )
                {
                    CmdArg->E = !!atoi(Opt + 1);
                }
                else
                {
                    CmdArg->E = 0;
                }
                break;
            }
            case OPT_I:
            {
                CmdArg->HasI = 1;
                if ( OptLen > 1 )
                {
                    CmdArg->I = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->I = 0;
                }
                break;
            }
            case OPT_R:
            {
                CmdArg->HasR = 1;
                if ( OptLen > 1 )
                {
                    CmdArg->R = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->R = 0;
                }
                break;
            }
            case OPT_S:
            {
                CmdArg->HasS = 1;
                if ( OptLen > 1 )
                {
                    CmdArg->S = atoi(Opt + 1);
                }
                else
                {
                    CmdArg->S = 0;
                }
                break;
            }
            case OPT_X:
            {
                CmdArg->HasX = 1;
                if ( OptLen > 1 )
                {
                    CmdArg->X = !!atoi(Opt + 1);
                }
                else
                {
                    CmdArg->X = 0;
                }
                break;
            }
            case OPT_Y:
            {
                CmdArg->HasY = 1;
                if ( OptLen > 1 )
                {
                    CmdArg->Y = !!atoi(Opt + 1);
                }
                else
                {
                    CmdArg->Y = 0;
                }
                break;
            }
            case OPT_Z:
            {
                CmdArg->HasZ = 1;
                if ( OptLen > 1 )
                {
                    CmdArg->Z = !!atoi(Opt + 1);
                }
                else
                {
                    CmdArg->Z = 0;
                }
                break;
            }
            default:
            {
                printf("Gcode G6 has no define opt = %s\n", CmdBuff);
                break;
            }
        }
        Offset += strlen(Opt) + sizeof(S8);     //加上分隔符的长度

    }
    return 1;
}

S32 GcodeG6Exec(VOID *ArgPtr,VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
    {
        return 0;
    }

    struct ArgG6_t *Param = (struct ArgG6_t *)ArgPtr;
    if ( Param->HasR )
    {
        Planner.LastPageStepRate =  Param->R;
    }

    if ( !DIRECTIONAL )
    {
        if ( Param->HasX )
        {
            Planner.LastPageDirX =  Param->X;
        }
        if ( Param->HasY )
        {
            Planner.LastPageDirY =  Param->Y;
        }
        if ( Param->HasZ )
        {
            Planner.LastPageDirZ =  Param->Z;
        }
        if ( Param->HasE )
        {
            Planner.LastPageDirE =  Param->E;
        }
    }

    // No speed is set, can't schedule the move
    if ( !Planner.LastPageStepRate )
    {
        return 0;
    }

    // No index means we just set the state
    if ( !Param->HasI )
    {
        return 0;
    }

    const U32 PageIndex = Param->I;    //index赋值

    U16 NumSteps = TotalSteps;          //step赋值
    if ( Param->HasS )
    {
        NumSteps = Param->S;
    }

    BufferPage(PageIndex, 0, NumSteps);
    ResetStepperTimeOut();

    return 1;
}

S32 GcodeG6Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
