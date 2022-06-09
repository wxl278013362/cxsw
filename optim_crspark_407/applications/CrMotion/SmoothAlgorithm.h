/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date            Author                 Notes
 * 2021-05-17     TingFengXuan       the first version
 */

 #ifndef __SMOOTH_ALGORITHM_H__
 #define __SMOOTH_ALGORITHM_H__

#include "CrInc/CrType.h"
#include "CrMotionPlanner.h"

//平滑算法
class SmoothAlgorithm
{
public:
    virtual VOID SmoothRecalc() = 0;
};

//结果平滑算法
class ResultantSmooth : public SmoothAlgorithm
{
public:
    VOID SmoothRecalc(struct PlanLine_t *LineBuff);   //输入输出都是PlanLineFifo
    virtual VOID SmoothRecalc();  //空函数
private:
    VOID ReverseScan(struct PlanLine_t *LineBuff);   //反向扫描
    VOID ForwardScan(struct PlanLine_t *LineBuff);   //前向扫描
    //VOID SmoothRecalc(struct PlanLine_t *LineBuff);
    volatile U32 SmoothIndex;        //开始平滑规划的LineBuf索引,

};

VOID CalculateForLine(struct PlanLine_t *Line);

extern ResultantSmooth Smooth;
#endif
