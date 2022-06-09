#ifndef _GCODEM860HANDLE_H_
#define _GCODEM860HANDLE_H_

#include "CrInc/CrType.h"
#include "CrMotion/CrStepperRun.h"

#ifdef __cplusplus
extern "C" {
#endif

#define I2CPE_PARSE_ERR               1
#define I2CPE_PARSE_OK                0

#define I2CPE_MAG_SIG_GOOD            0
#define I2CPE_MAG_SIG_MID             1
#define I2CPE_MAG_SIG_BAD             2

#define I2CPE_ENCODER_CNT             1

// Default I2C addresses
#define I2CPE_PRESET_ADDR_X           30
#define I2CPE_PRESET_ADDR_Y           31
#define I2CPE_PRESET_ADDR_Z           32
#define I2CPE_PRESET_ADDR_E           33

#define I2CPE_DEF_AXIS                X
#define I2CPE_DEF_ADDR                I2CPE_PRESET_ADDR_X

// Encoder types
#define I2CPE_ENC_TYPE_ROTARY         0
#define I2CPE_ENC_TYPE_LINEAR         1

#define I2CPE_MAG_SIG_NF              255

// Default settings for encoders which are enabled, but without settings configured above.
#define I2CPE_DEF_ENC_TICKS_UNIT      2048
#define I2CPE_DEF_TYPE                I2CPE_ENC_TYPE_LINEAR
#define I2CPE_DEF_TICKS_REV           (16 * 200)
#define I2CPE_DEF_EC_THRESH           0.1

#define I2CPEADDR_MIN   30
#define I2CPEADDR_MAX   30

#define CONSTRAIN(value, arg_min, arg_max) ((value) < (arg_min) ? (arg_min) :((value) > (arg_max) ? (arg_max) : (value)))
#define WITHIN(A,B,C)  ((A >= B) && (A <= C))

struct I2CPositionEncoder_t
{
    enum Axis_t    EncoderAxis;
    U8          I2CAddress;
    U8          Type;
    S32         EncoderTicksPerUnit;
    S32         StepperTicks;
    S32         ZeroOffset;
    S32         ErrorCount;
    S32         Position;
    S32         ErrorPrev;
    U8          H;// Magnetic field strength
    BOOL        Ec;
    BOOL        Active;
    FLOAT       EcThreshold;
};

S8 Parse(VOID *ArgPtr);
U8 IdxFromAxis(enum Axis_t Axis);
S32 GetRawCount(struct I2CPositionEncoder_t Encoders);
S8 IdxFromAddr(const U8 Addr);
U8 EncodersGetAddress(U8 Index);
enum Axis_t EncoderGetAxis(U8 Index); 
VOID ChangeModuleAddress(const U8 oldAddr, const U8 newAddr);
VOID TestAxis(const U8 Idx);
VOID ReportModuleFirmware(U8 Addr);
VOID ResetErrorCount(const U8 Idx, const enum Axis_t Axis);
VOID ReportErrorCount(const U8 Idx, const enum Axis_t Axis);
VOID EnableEc(U8 Idx, const BOOL Enabled, enum Axis_t Axis);
VOID ReportError(const U8 Idx);

S32 GcodeM860Parse(const S8 *CmdBuff, VOID *Arg);     //M860命令的解析函数
S32 GcodeM860Exec(VOID *ArgPtr, VOID *ReplyResult);   //M860命令的执行函数
S32 GcodeM860Reply(VOID *ReplyResult, S8 *Buff);      //M860命令的回复函数

extern U8 I2CPEAddr;
extern U8 I2CPEIdx;
extern BOOL I2CPEAnyaxis;
extern S8 AxisCodes[4];
extern struct I2CPositionEncoder_t Encoders[I2CPE_ENCODER_CNT];

#ifdef __cplusplus
}
#endif

#endif /* _GCODEM860HANDLE_H_ */
