#include "GcodeM999Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <rtthread.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "Common/Include/CrCommon.h"
#include "CrMotion/CrMotionPlanner.h"
#include "../../../../CrSerial/CrSerial.h"

#define CMD_M999            "M999"

#define WITHIN(N,L,H)       ((N) >= (L) && (N) <= (H))
#define NUMERIC(a)          WITHIN(a, '0', '9')
#define NUMERIC_SIGNED(a)   (NUMERIC(a) || (a) == '-' || (a) == '+')

S8 LastN = 0;
S8 SerialArrayUsed = 0;   //已使用的串口缓存数组个数

//U8 Length = 0;  // Count of commands in the queue
U8 IndexR = 0;  // Ring buffer read position
U8 IndexW = 0;  // Ring buffer write position

S8 CommandBuffer[MAX_SERIAL_ARRAYS][MAX_CMD_SIZE];

// MARK:暂时在这儿定义，最后应该在主循环所在源文件定义
CrFirmwaveState CrState;      // 固件运行状态
// MARK:暂时在这儿定义，最后应该在串口设备管理源文件定义
S16 CrSerialCurrentFd = 0;    // 当前系统和上位机交互使用的串口句柄

/*
 * The port that the command was received on
 */
#if NUM_SERIAL > 1
S16 Port[MAX_SERIAL_ARRAYS];
#endif

// Number of characters read in the current line of serial input
S32 SerialCount[NUM_SERIAL] = { 0 };

BOOL SendOk[MAX_SERIAL_ARRAYS] = {false};

/**
 *
 * 获取当前用于和上位机通信的，用于打印的串口句柄
 *
 * @return S16 串口句柄
 */
S16 CrSerialsGetPort(void)
{
    return CrSerialCurrentFd;
}

/**
 *
 * 刷新串口读缓冲区
 *
 * @param Port 需要刷新的串口句柄
 * @return 无
 */
VOID SerialFlush(S16 Port)
{
    CrSerialIoctl((S32)Port, SERIAL_RX_FLASH, (VOID *)0);
}

/**
 *
 * 将指定串口重定位到标准输出设备，以保证printf的正常使用
 *
 * @param Port 需要定位的串口句柄
 * @return 无
 */
VOID SerialPortRedirRect(S16 Port)
{
    // MARK:目前的重定向并没有将rt_kprintf和printf分开不同的串口使用
    // 如果需要rt_kprintf和printf分开不同的串口使用，需要阅读libc库或者内核源码然后做调整
#if NUM_SERIAL > 1
    char UartName[64];
    CrSerialIoctl((S32)Port, GET_SERIAL_NAME, UartName);
    rt_console_set_device(UartName);
#endif  // NUM_SERIAL > 1
}

/**
 * 刷新串口读缓冲区，并向上位机请求发送上次行校验后开始的所有命令，同时回复上位机一个ok
 *
 * @return 无
 */
VOID FlushAndRequestResend()
{
#if NUM_SERIAL > 1
    const S16 Port = CrSerialsGetPort();          //获取串口端口
    if ( Port < 0 )
    {
        return;
    }
    else
    {
        SerialPortRedirRect(Port);   //串口的端口重定向，自动创建对象并自动保存
    }
#else
    const S16 Port = 0;
#endif

    SerialFlush(Port);

    // MARK:目前源码中解析命令还没有加入行校验，加入后这儿需要先获取一下LastN
    //      如果后面公司自己设计的框架不需要行校验，关于重发命令的指令根据实际定义再实现
    printf("%s %d\n", MSG_RESEND, LastN + 1);  // 请求从接收到的最后一行开始重新发送到主机

    printf("ok\n");

    // MARK:重定向后，执行完动作，需不需要重新重定向回去？
}

/**
 *
 * 解析M999指令
 *
 * @param CmdBuff 上位机下发的命令字符串
 * @param Arg 保存命令参数的缓冲区
 * @return 解析成功返回1，否则返回0
 */
S32 GcodeM999Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
    {
        return 0;
    }
    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )
    {
        return 0;
    }

    S8 Cmd[CMD_BUFF_LEN] = { 0 };
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
    {
        return 0;
    }

    // 判断是不是M999命令
    if ( strcmp(Cmd, CMD_M999) != 0 )
    {
        return 0;
    }

    S32 Offset = strlen(CMD_M999) + sizeof(S8); //加1是越过分隔符
    struct ArgM999_t *CmdArg = (struct ArgM999_t *)Arg;

    while ( Offset < Len )                      //处理结束
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
        {
            //printf(" Cmd get opt failed  cmd = %s\n", CmdBuff);
            break;                              //获取参数失败
        }

        S32 OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            printf("Warn:without code in parameter:%s \n", Opt);
        }

        switch ( Opt[0] )  //判断命令
        {
            case OPT_S:
            {
                CmdArg->HasS = true;
                if ( OptLen > 1 )
                {
                    CmdArg->S = atof( Opt + 1 );
                }
                break;
            }
            default:
            {
                break;
            }
        }

        Offset += OptLen + sizeof(S8); //此处的1是分隔符
    }

    if ( !CmdArg->HasS )
    {
        CmdArg->HasS = true;
        CmdArg->S = false;
    }

    return 1;
 }

/**
 *
 * 执行M999
 *
 * @param ArgPtr M999指令的参数
 * @param ReplyResult 保存执行后需要返回的结果
 * @return 执行成功返回1，否则返回0
 */
S32 GcodeM999Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
    {
        return 0;
    }

    CrState = CF_STATE_RUNNING;

    struct ArgM999_t *CmdArg = (struct ArgM999_t *)ArgPtr;
    if ( !CmdArg->S )
    {
        // 不需要刷新串口直接返回
        return 1;
    }

    // 刷新串口,回应上位机OK并请求重发指令
    FlushAndRequestResend();

    return 1;
}

S32 GcodeM999Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
    {
        return Len;
    }

    return Len;
}

