/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-12-01     cx2470       the first version
 */
#ifndef _CRPARAMCONFIG_H_
#define _CRPARAMCONFIG_H_

#include "CrInc/CrType.h"
#include "CrInc/CrConfig.h"
#include "CrInc/CrMsgType.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 CrConfigSave();    //在保存参数前要进行
VOID CrConfigRecover();
S32 CrConfigLoad();     //在程序开始时要进行一次加载

VOID CrConfigAbsSave();
VOID CrConfigAbsRecover();
S32 CrConfigAbsLoad();

VOID CrConfigPlaSave();
VOID CrConfigPlaRecover();
S32 CrConfigPlaLoad();

struct ParamFanSpeed_t *GetFansParam();
struct ParamFanSpeed_t *GetFansDefauleParam();
S32 SetFansParam(struct ParamFanSpeed_t *Param);

struct ParamMaterialTemperSet_t *GetTemperSet();
struct ParamMaterialTemperSet_t *GetDefauleTemperSet();
S32 SetTemperSet(struct ParamMaterialTemperSet_t *Set);

struct ParamMotionParam_t *GetMotionParam();
struct ParamMotionParam_t *GetDefauleMotionParam();
S32 SetMotionParam(struct ParamMotionParam_t *Param);

FLOAT *GetFilementConfig();
VOID SetFilementConfig(FLOAT Filement);
BOOL GetFilamentVolumePattern();
BOOL GetFilamentDefaultVolumePattern();
S32  SetFilamentVolumePattern(BOOL Opened);

#ifdef __cplusplus
}
#endif

#endif /* _CRCONFIGSTORE_H_ */
