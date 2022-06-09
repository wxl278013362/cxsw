#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "GcodeM75Handle.h"
#include "CrGcode/GcodeHandle/GcodeType.h"
#include "CrModel/CrPrinter.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "CrGcode/GcodeHandle/GcodeCommonFunc.h"
#include "CrModel/CrMotion.h"


#define CMD_M75 "M75"

/********增加的函数和变量****************/
//#define GCODE_REPEAT_MARKERS    // Enable G-code M808 to set repeat markers and do looping
//#define CANCEL_OBJECTS          //Implement M486 to allow Marlin to skip objects
//#define LCD_SHOW_E_TOTAL        // Show the E position (filament used) during printing

BOOL PrintingIsPaused() //当前的打印状态
{
    return false;
}

VOID RepeatReset()
{
    U8 index;
    index = 0;
}

VOID CancelableReset()
{
    U32 Canceled = 0x0000;
    S8 ObjectCount = 0;
}
/********增加的函数和变量****************/

S32 GcodeM75Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
    {
        return 0;
    }

    S32 Len = strlen(CmdBuff);
    if ( Len <= 1 || CmdBuff[0] != GCODE_CMD_M ) //Gcode命令长度需要大于1且是M命令
    {
        return 0;
    }

    S8 Cmd[CMD_BUFF_LEN] = {0};
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
    {
        return 0;
    }

    if ( strcmp(Cmd,CMD_M75) != 0 )
    {
        return 0;
    }

    return 1;
}
S32 GcodeM75Exec(VOID *ArgPtr, VOID *ReplyResult)
{
    if ( !ArgPtr || !ReplyResult || !Printer )
    {
        return 0;
    }

    if ( !(PAUSE_PRINT == Printer->GetPrintStatus()) )
    {
        #ifdef  GCODE_REPEAT_MARKERS  //如果有重复标记的宏定义
            RepeatReset();
        #endif

        #ifdef  CANCEL_OBJECTS    //允许跳过项目
            CancelableReset();
        #endif

        #ifdef  LCD_SHOW_E_TOTAL
            StepperMotionAttr_t Attr = MotionModel->GetSetpperMotion(StepperE);
            Attr.Acceleration = 0;
            MotionModel->SetStepperMotion(Attr,StepperE);
        #endif


    }
    /*
     * Marlin原生流程：开启定时器。
     * 同时根据当前定时器的状态,如果不是暂停状态则定时器内部的totalPrints + 1，将状态置为RUNNING（只是定时器内部的一个状态标志，不是打印机当前状态），记录下当前的时间作为定时器的开始时间并返回true
     * 如果处于暂停状态返回false
     *
     * Spark由于目前暂时没有定时器，故M75暂不做实现
     */
    return 1;
}

S32 GcodeM75Reply(VOID *ReplyResult, S8 *Buff)
{
    S32 Len = 0;
    if ( !Buff )
        return Len;

    return Len;
}
