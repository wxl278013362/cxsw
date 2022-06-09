#ifndef _GCODEM999HANDLE_H_
#define _GCODEM999HANDLE_H_

#include "CrInc/CrType.h"

#define NUM_SERIAL 1

#define MSG_OK             "ok"
#define MSG_RESEND         "Resend: "

#define MAX_SERIAL_ARRAYS  4
#define MAX_CMD_SIZE 96

extern S8 LastN;           //上次行校验的行号
extern S8 SerialArrayUsed; //已使用的串口缓存数组个数

extern U8 IndexR;          // Ring buffer read position
extern U8 IndexW;          // Ring buffer write position

extern S8 CommandBuffer[MAX_SERIAL_ARRAYS][MAX_CMD_SIZE];

/*
 * The port that the command was received on
 */
#if NUM_SERIAL > 1
extern S16 Port[MAX_SERIAL_ARRAYS];
#endif

/**
 * Serial command injection
 */

// Number of characters read in the current line of serial input
extern S32 SerialCount[NUM_SERIAL];

extern BOOL SendOk[MAX_SERIAL_ARRAYS];

typedef enum __CrFirmwaveState
{
  CF_STATE_INITIALIZING =  0,
  CF_STATE_RUNNING,
  CF_STATE_PAUSED,
  CF_STATE_WAITING,
  CF_STATE_STOPPED,
  CF_STATE_SD_COMPLETE,
  CF_STATE_KILLED,
  CF_STATE_MAX
}CrFirmwaveState;

extern CrFirmwaveState CrState;
extern S16 CrSerialCurrentPort;

#ifdef __cplusplus
extern "C" {
#endif

S32 GcodeM999Parse(const S8 *CmdBuff, VOID *Arg);     //M999命令的解析函数
S32 GcodeM999Exec(VOID *ArgPtr, VOID *ReplyResult);   //M999命令的执行函数
S32 GcodeM999Reply(VOID *ReplyResult, S8 *Buff);      //M999命令的回复函数

#ifdef __cplusplus
}
#endif

#endif
