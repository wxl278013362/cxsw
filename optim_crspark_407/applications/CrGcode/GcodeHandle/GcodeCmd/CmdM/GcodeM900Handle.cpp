
#include "GcodeM900Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "CrMotion/StepperApi.h"
#include "CrModel/CrMotion.h"
#include "Common/Include/CrSleep.h"

#define CMD_M900  "M900"

auto EchoValueOor = [](const S8 Ltr, const BOOL Ten = true)
{
    printf("? %c value out of range", Ltr);
    if ( Ten )
        printf(" (0-10)");

    printf(".");
};

S32 GcodeM900Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = { 0 };
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;

    // M911
    if ( strcmp(Cmd, CMD_M900) != 0 )
        return 0;

    S32 Offset = strlen(CMD_M900) + sizeof(S8); //加1是越过分隔符
    struct ArgM900_t *CmdArg = (struct ArgM900_t *)Arg;

    while ( Offset < Len )  //处理结束
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
        {
            //printf(" Cmd get opt failed  cmd = %s\n", CmdBuff);
            break;    //获取参数失败
        }

        S32 OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            printf("Warn:without code in parameter:%s \n", Opt);
        }

        switch ( Opt[0] )  //判断命令
        {
            case OPT_T:
            {
                CmdArg->HasT = true;
                if ( OptLen > 1 ) CmdArg->T = atoi( Opt + 1 );
                break;
            }
            case OPT_L:
            {
                CmdArg->HasL = true;
                if ( OptLen > 1 ) CmdArg->L = atof( Opt + 1 );
                break;
            }
            case OPT_K:
            {
                CmdArg->HasK = true;
                if ( OptLen > 1 ) CmdArg->K = atof( Opt + 1 );
                break;
            }
            case OPT_S:
            {
                CmdArg->HasS = true;
                if ( OptLen > 1 ) CmdArg->S = atof( Opt + 1 );
                break;
            }
            default:
            {
                break;
            }
        }

        Offset += OptLen + sizeof(S8); //此处的1是分隔符
    }

    return 1;
 }

S32 GcodeM900Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !MotionModel )
        return 0;

    struct ArgM900_t *CmdArg = (struct ArgM900_t *)ArgPtr;
#if EXTRUDERS < 2
    constexpr U8 ToolIndex = 0;
#else
    const U8 ToolIndex = 0;    //获取当前活动挤出机的索引
    if ( CmdArg->HasT )
        ToolIndex = CmdArg->T;

    if ( ToolIndex >= EXTRUDERS )
    {
       EchoValueOor('T', false);
       return;
    }
#endif

    FLOAT *Kref = NULL;   //planner.extruder_advance_K[ToolIndex] 获取挤出机的K值变量的指针
    FLOAT NewK = 0.0f;
    if ( Kref )
    {
        NewK = *Kref;
    }
    const FLOAT OldK = NewK;

#ifdef EXTRA_LIN_ADVANCE_K

  FLOAT *Lref = NULL;   //other_extruder_advance_K[ToolIndex] 获取其他挤出机的K值变量的指针

  const BOOL OldSlot = 0;                //TEST(lin_adv_slot, tool_index), // The tool's current slot (0 or 1)
  const BOOL NewSlot = OldSlot;   // The passed slot (default = current)
  if ( CmdArg->HasS )
      NewSlot = CmdArg->S;

  // If a new slot is being selected swap the current and
  // saved K values. Do here so K/L will apply correctly.
  if (NewSlot != OldSlot)
  {                       // Not the same slot?
      //SET_BIT_TO(lin_adv_slot, tool_index, new_slot); // Update the slot for the tool
      if ( Lref )
      {
          NewK = Lref;                                    // Get new K value from backup
          *Lref = OldK;                                    // Save K to backup
      }
  }

  // Set the main K value. Apply if the main slot is active.
  if ( CmdArg->HasK )
  {
      const FLOAT K = CmdArg->K;
      if ( K < 0 || K > 10 )
      {
          EchoValueOor('K', true);
      }
      else if ( NewSlot )
      {
          if ( Lref )
              *Lref = K;  // S1 Knn
      }
      else
      {
          NewK = K;  // S0 Knn
      }
  }

      // Set the extra K value. Apply if the extra slot is active.
      if ( CmdArg->HasL )
      {
          const FLOAT L = CmdArg->L;
          if ( K < 0 || K > 10 )
          {
              EchoValueOor('L', true);
          }
          else if ( !NewSlot )
          {
              if ( Lref )
                  *Lref = L;  // S0 Lnn
          }
          else
          {
              NewK = L;  // S1 Lnn
          }

      }

#else

      if ( CmdArg->HasK )
      {
          if ( (CmdArg->K >= 0.0f) && (CmdArg->K <= 10.0) )
              NewK = CmdArg->K;
          else
          {
              EchoValueOor('K', true);
          }
      }

#endif

    if (NewK != OldK)
    {
        //运动同步
        while ( !MotionModel->IsMotionMoveFinished() )
            CrM_Sleep(5);

        if ( Kref )
          *Kref = NewK;
    }

    if ( !(CmdArg->HasK || CmdArg->HasL || CmdArg->HasS || CmdArg->HasT) )
    {

    #ifdef EXTRA_LIN_ADVANCE_K

         #if EXTRUDERS < 2
              printf("Advance S%d", int(NewSlot));
              if ( Kref )
                  printf(" K%f", *Kref);

              printf("(S%d", int(!NewSlot))
              if ( Lref )
                  printf(" K%f", *Lref)

              printf(")\n");
         #else
              S8 i = 0;
              for ( i= 0; i < EXTRUDERS; i++ )
              {
                  const BOOL Slot = 0; //获取对应的slot： TEST(lin_adv_slot, i);
                  FLOAT K = 0.0f;      //获取挤出机的K值   planner.extruder_advance_K[i]
                  FLOAT OtherK = 0.0f; //获取其他基础机的K值 other_extruder_advance_K[i]
                  printf("Advance T%d S%d K%f(S%d K%f)\n", i, int(Slot) , K, int(!Slot), OtherK);
              }
         #endif

    #else


        #if EXTRUDERS < 2
              FLOAT ExtruderAdvanceK  = 0.0f;    //获取挤出机的K值 planner.extruder_advance_K[0]
              printf("Advance K=%f\n", ExtruderAdvanceK);
        #else
              printf("Advance K");
              S8 i = 0;
              for ( i= 0; i < EXTRUDERS; i++ )
              {
                  FLOAT ExtruderAdvanceK = 0.0; //获取不同挤出头的K值 planner.extruder_advance_K[i]
                  printf(" %d:", i);
                  printf("%f", ExtruderAdvanceK);
              }
              printf("\n");
        #endif

    #endif
    }

    return 1;
}

S32 GcodeM900Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}

