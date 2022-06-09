#include "GcodeM261Handle.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../GcodeType.h"
#include "../../GcodeCommonFunc.h"
#include "GcodeM260Handle.h"

#if EXPERIMENTAL_I2CBUS

#define CMD_M261 "M261"

static S16 Index = 0;
//static S32  I2CAddr = 0;
//static S8 I2CBuff[TWIBUS_BUFFER_SIZE] = {0};
//static S8 ByteCount = 0;
//#define I2C_MAX_ADDR 127
//#define I2C_MIN_ADDR 8

// MARK:该接口应该在IIC驱动中定义，暂时没有实现，预留接口
/**
 * IIC数据传输函数
 *
 * @param Addr IIC从设备地址
 * @param Dir 数据传输方向 1:代表读 0:代表写
 * @param Buff 数据Buff
 * @param Bytes 需要传输的数据的大小
 * @return 实际传输的数据的大小，小于0表示传输失败
 */
extern S8 IicTransfer (S32 Addr, U8 Dir, S8 *Buff, U8 Bytes);
S8 IicTransfer (S32 Addr, U8 Dir, S8 *Buff, U8 Bytes)
{
    // 空函数
}
/**
 *
 * 从指定地址的IIC设备读取长度为Bytes的数据到I2CBuff
 *
 * @param Addr IIC设备地址
 * @param Bytes 需要读取的长度
 * @return 读取到的长度
 */
S8 RequestFrom(S32 Addr, U8 Bytes)
{
    if ( !Addr || Bytes == 0 || Bytes >= TWIBUS_BUFFER_SIZE )
    {
        return 0;
    }

    S16 Count = 0;
    //从指定地址获取数据
    Count = IicTransfer(Addr, 1, I2CBuff, Bytes);
    if ( Count < 0 )
    {
        return 0;
    }

    while ( Count < Bytes )
    {
        // 如果数据不够,填充数据
        I2CBuff[Count++] = '\0';
    }

    ByteCount = Count;

    return Bytes;
}

/**
 *
 * 数据读取请求
 *
 * @param Addr IIC设备地址
 * @param Bytes 需要读取的长度
 * @return 请求执行成功返回true,否则返回false
 */
BOOL Request(S32 Addr, const U8 Bytes)
{
  if ( !Addr || Bytes == 0 || Bytes >= TWIBUS_BUFFER_SIZE )
  {
      return false;
  }

  // 从指定IIC设备中读取数据
  if ( RequestFrom(Addr, Bytes) == 0 )
  {
      return false;
  }

  return true;
}

/**
 *
 * 判断IIC数据是否有效
 *
 * @return 有效返回true,否则返回false
 */
BOOL WireAvailable()
{
    if ( Index >= ByteCount || (Index >= TWIBUS_BUFFER_SIZE) )
    {
        return false;
    }

    return true;
}

/**
 *
 * 从I2CBuff读取一个字节数据
 *
 * @return 读取到的数据
 */
S8 WireRead()
{
    S8 Data = '\0';

    if ( (Index >= ByteCount) || (Index >= TWIBUS_BUFFER_SIZE) )
    {
        return Data;
    }

    Data = I2CBuff[Index++];
    if ( Index == ByteCount )
    {
        Index = 0;
        ResetI2C();
    }

    return Data;
}

/**
 * 回显读取到的IIC设备数据
 *
 * @param Bytes 需要回显的数据大小
 * @param Prefix 回显数据前打印的前缀
 * @param Addr 设备地址
 * @return 无
 */
VOID EchoData(U8 Bytes, const S8 Prefix[], U8 Addr)
{
  printf("%s: from:%d bytes:%d data:", Prefix, Addr, Bytes);
  while ( Bytes-- && WireAvailable() )
  {
      printf("%c", WireRead());
  }
  printf("\n");
}

/**
 *
 * 回复下发M261命令的上位机
 *
 * @param Bytes 需要回复的数据的大小
 * @param Addr 回复数据来源设备的地址
 * @return 无
 */
VOID Relay(const U8 Bytes, S32 Addr)
{
  if ( Request(Addr, Bytes) )
  {
      EchoData(Bytes, "i2c-reply", Addr);
  }
}

/**
 *
 * 解析M261指令
 *
 * @param CmdBuff 上位机下发的命令字符串
 * @param Arg 保存命令参数的缓冲区
 * @return 解析成功返回1，否则返回0
 */
S32 GcodeM261Parse(const S8 *CmdBuff, VOID *Arg)
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

    // M261
    if ( strcmp(Cmd, CMD_M261) != 0 )
    {
        return 0;
    }

    S32 Offset = strlen(CMD_M261) + sizeof(S8);
    struct ArgM261_t *CmdArg = (struct ArgM261_t *)Arg;

    while ( Offset < Len )
    {
        S8 Opt[20] = {0};
        if ( !GetCmdOpt(CmdBuff + Offset, Opt) )
        {
            break;
        }

        S32 OptLen = strlen(Opt);
        if ( OptLen <= 1 )
        {
            printf("Warn:without code in parameter:%s \n", Opt);
        }

        switch ( Opt[0] )
        {
            case OPT_A:
            {
                CmdArg->HasA = true;
                if ( OptLen > 1 )
                {
                    CmdArg->A = atoi(Opt + 1);
                }
                break;
            }
            case OPT_B:
            {
                CmdArg->HasB = true;
                if ( OptLen > 1 )
                {
                    CmdArg->B = atoi(Opt + 1);
                }
                break;
            }
            default:
            {
                printf("Warn:unknown parameter:%s \n", Opt);
                break;
            }
        }

        Offset += strlen(Opt) + sizeof(S8);
    }

    if ( !CmdArg->HasB )
    {
        CmdArg->HasB = true;
        CmdArg->B = 1;
    }

    return 1;
 }

/**
 *
 * 执行M261
 *
 * @param ArgPtr M261指令的参数
 * @param ReplyResult 保存执行后需要返回的结果
 * @return 执行成功返回1，否则返回0
 */
S32 GcodeM261Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult )
    {
        return 0;
    }

    struct ArgM261_t *Param = (struct ArgM261_t *)ArgPtr;
    if ( !Param->HasA || (Param->A <= 0 ) || (Param->A > 255) )
    {
        printf("Bad i2c request\n");
        return 1;
    }

    if ( (Param->B > 0) && (Param->B <= TWIBUS_BUFFER_SIZE))
    {
        Relay(Param->B, Param->A);
    }
    else
    {
        printf("Bad i2c request\n");
    }

    return 1;
}

S32 GcodeM261Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
    {
        return Len;
    }

    return Len;
}

#endif  // EXPERIMENTAL_I2CBUS
