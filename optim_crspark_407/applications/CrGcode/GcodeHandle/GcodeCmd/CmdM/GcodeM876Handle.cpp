
#include "GcodeM876Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"

#define CMD_M876  "M876"

PromptReason_t HostPromptReason = PROMPT_NOT_DEFINED;

//#if ENABLED(HOST_ACTION_COMMANDS)
  #ifndef ACTION_ON_PAUSE
    #define ACTION_ON_PAUSE   "pause"
  #endif
  #ifndef ACTION_ON_PAUSED
    #define ACTION_ON_PAUSED  "paused"
  #endif
  #ifndef ACTION_ON_RESUME
    #define ACTION_ON_RESUME  "resume"
  #endif
  #ifndef ACTION_ON_RESUMED
    #define ACTION_ON_RESUMED "resumed"
  #endif
  #ifndef ACTION_ON_CANCEL
    #define ACTION_ON_CANCEL  "cancel"
  #endif
  #ifndef ACTION_ON_KILL
    #define ACTION_ON_KILL    "poweroff"
  #endif
//  #if HAS_FILAMENT_SENSOR
    #ifndef ACTION_ON_FILAMENT_RUNOUT
      #define ACTION_ON_FILAMENT_RUNOUT "filament_runout"
    #endif
    #ifndef ACTION_REASON_ON_FILAMENT_RUNOUT
      #define ACTION_REASON_ON_FILAMENT_RUNOUT "filament_runout"
    #endif
//  #endif
//  #if ENABLED(G29_RETRY_AND_RECOVER)
    #ifndef ACTION_ON_G29_RECOVER
      #define ACTION_ON_G29_RECOVER "probe_rewipe"
    #endif
    #ifndef ACTION_ON_G29_FAILURE
      #define ACTION_ON_G29_FAILURE "probe_failed"
    #endif
//  #endif

const S8 CONTINUE_STR[] = "Continue";

static VOID HostAction(const S8 * const Pstr, const BOOL Eol);

VOID HostActionPrompt(const S8 * const Ptype, const BOOL Eol=true) {
    HostAction("prompt_", false);
    if ( Eol )
    {
        printf("%s\n", Ptype);
    }
    else
    {
        printf(Ptype);
    }
}

VOID HostActionPromptPlus(const S8 * const Ptype, const S8 * const Pstr, const BOOL Eol=true)
{
  HostActionPrompt(Ptype, false);
  if ( Eol )
  {
      printf(" %s\n", Pstr);
  }
  else
  {
      printf(" %s", Pstr);
  }

  return;
}

VOID HostActionPromptBegin(const S8* const Pstr, const BOOL Eol = true) { HostActionPromptPlus("begin", Pstr, Eol); }
VOID HostActionPromptButton(const S8* const Pstr) { HostActionPromptPlus("button", Pstr); }
VOID HostActionPromptEnd() { HostActionPrompt("end"); }
VOID HostActionPromptShow() { HostActionPrompt("show"); }
VOID HostPromptDo(const PromptReason_t reason, const S8 * const Pstr, const S8* const Pbtn/*=nullptr*/)
{
    HostPromptReason = reason;
    HostActionPromptEnd();
    HostActionPromptBegin(Pstr);
    if ( Pbtn )
        HostActionPromptButton(Pbtn);
    HostActionPromptShow();
}

S32 GcodeM876Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    printf("[%s] %s \n", __FUNCTION__, CmdBuff);

    S32 Len = strlen(CmdBuff);

    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = { 0 };
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;

    if ( strcmp(Cmd, CMD_M876) != 0 )
        return 0;

    S32 Offset = strlen(CMD_M876) + sizeof(S8); // 加1是越过分隔符
    struct ArgM876_t *CmdArg = (struct ArgM876_t *)Arg;

    // [A<bool>] [D<seconds>] [I<speed>] [R<bool>] [S<speed>]
    while ( Offset < Len )
    {
        S8 Opt[20] = {0}; // 操作参数
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) ) break;

        S32 OptLen = strlen(Opt);
        if ( OptLen <= 1 ) // 没有操作码
        {
            printf("Warn:without code in parameter:%s \n", Opt);
        }

        // 解析出操作码
        switch ( Opt[0] ) // 操作类型
        {
            case OPT_S:
                CmdArg->HasS = true;
                if ( OptLen > 1 ) CmdArg->S = atoi(Opt + 1);
                break;

            default:
                printf("Unknown parameter:%s \n", Opt);
                break;
        }

        Offset += strlen(Opt) + sizeof(S8); //此处的1是分隔符
    }

    return 1;
 }

S32 GcodeM876Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
        return 0;

    struct ArgM876_t *Param = (struct ArgM876_t *)ArgPtr;
    if ( Param->HasS )
    {
        const char *Msg = "UNKNOWN STATE";
        const PromptReason_t Hpr = HostPromptReason;
        HostPromptReason = PROMPT_NOT_DEFINED;
        switch ( Hpr )
        {
          case PROMPT_FILAMENT_RUNOUT:
            Msg = "FILAMENT_RUNOUT";
            if (Param->S == 0)
            {
              //
//              #if ENABLED(ADVANCED_PAUSE_FEATURE)
//                pause_menu_response = PAUSE_RESPONSE_EXTRUDE_MORE;
//              #endif
              HostActionPromptEnd();   // Close current prompt
              HostActionPromptBegin(ACTION_ON_PAUSED);
              HostActionPromptButton("Purge More");
              if (false
                #if HAS_FILAMENT_SENSOR
                  || runout.filament_ran_out
                #endif
              )
                  HostActionPromptButton("DisableRunout");
              else {
                  HostPromptReason = PROMPT_FILAMENT_RUNOUT;
                  HostActionPromptButton(CONTINUE_STR);
              }
              HostActionPromptShow();
            }
            else if (Param->S == 1) {
              #if HAS_FILAMENT_SENSOR
                if (runout.filament_ran_out) {
                  runout.enabled = false;
                  runout.reset();
                }
              #endif
//              #if ENABLED(ADVANCED_PAUSE_FEATURE)
//                pause_menu_response = PAUSE_RESPONSE_RESUME_PRINT;
//              #endif
            }
            break;
          case PROMPT_USER_CONTINUE:
            #if HAS_RESUME_CONTINUE
              wait_for_user = false;
            #endif
            Msg = "FILAMENT_RUNOUT_CONTINUE";
            break;
          case PROMPT_PAUSE_RESUME:
            Msg = "LCD_PAUSE_RESUME";
//            #if ENABLED(ADVANCED_PAUSE_FEATURE)
//              extern const char M24_STR[];
//              queue.inject_P(M24_STR);
//            #endif
            break;
          case PROMPT_INFO:
            Msg = "GCODE_INFO";
            break;
          default: break;
        }
        printf("M876 Responding PROMPT_%s\n", Msg);
    }

    return 1;
}

S32 GcodeM876Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}

VOID HostAction(const S8 * const Pstr, const BOOL Eol)
{
    if ( Eol )
    {
        printf("//action:%s\n", Pstr);
    }
    else
    {
        printf("//action:%s", Pstr);
    }

    return;
}

