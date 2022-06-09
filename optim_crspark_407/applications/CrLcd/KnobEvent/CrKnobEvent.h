#ifndef _CRKNOBEVENT_H
#define _CRKNOBEVENT_H

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EC11_PINA_NAME      "PF11"//"PB14"  //PinA引脚名称
#define EC11_PINB_NAME      "PG9"//"PB10"  //PinB引脚名称
#define EC11_PINBOTTUN_NAME "PF12"//"PB2"   //Bottun引脚名称

#define KNOB_SCAN_PERIOD_MS  2       //扫描周期

//旋钮事件控制命令类型
enum KnobCmdType_t
{
    KNOB_CMD_GET_KNOB_EVENT = 0,
    KNOB_CMD_SET_KNOB_ATTR,
    KNOB_CMD_SCAN_START,
    KNOB_CMD_SCAN_STOP,
    KNOB_CMD_MAX_NUM
};

//旋钮值
enum KnobMtion_t
{
    KNOB_MTION_NONE = 0,
    KNOB_MTION_UP,
    KNOB_MTION_DOWN
};

//按钮值
enum KnobButton_t
{
    KNOB_BUTTON_NONE=0,
    KNOB_BUTTON_DOWN
};
//旋钮事件值
struct KnobEvent_t
{
    enum KnobMtion_t KnobMtion;     //旋钮值
    enum KnobButton_t KnobButton;   //按键值
};

//属性成员
struct KnobAttr_t
{
    U8 PinA_Name[5];
    U8 PinB_Name[5];
    U8 PinButtonName[5];
};

S32 CrKnobOpen(S8 *Name, S32 Flag, S32 Mode);   //打开旋钮事件
S32 CrKnobClose(S32 Fd);                        //关闭旋钮事件
S32 CrKnobIoctl(S32 Fd, S32 Cmd, VOID *Arg);    //控制旋钮事件

#ifdef KNOBEVENT_TEST
VOID CrKnobEventTestMain();                     //旋钮事件测试用例
#endif

#ifdef __cplusplus
}
#endif

#endif
