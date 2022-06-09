#ifndef _MOVEAXISMENUITEM_H
#define _MOVEAXISMENUITEM_H

#include "CrInc/CrType.h"

#define MAX_MOVE_UNIT (10.0)    //最大的移动单元(单位：毫米(mm))
#define MID_MOVE_UNIT (1.0)     //中等的移动单元(单位：毫米(mm))
#define MIN_MOVE_UNIT (0.1)     //最小的移动单元(单位：毫米(mm))
#define Z_MIN_MOVE_UNIT (0.025)     //z轴最小的移动单元(单位：毫米(mm))

#ifdef __cplusplus
extern "C"{
#endif

S32 MoveAxisMenuInit();

#if 0
S32 SetAxisPosition(float X_Pos, float Y_Pos, float Z_Pos, float E_Pos);
#endif

VOID* X_AixsMaxUnitMoveMenuUpAction(VOID *ArgOut, VOID *ArgIn);     //X轴最大移动单元移动菜单的Up动作
VOID* X_AixsMidUnitMoveMenuUpAction(VOID *ArgOut, VOID *ArgIn);     //X轴中等移动单元移动菜单的Up动作
VOID* X_AixsMinUnitMoveMenuUpAction(VOID *ArgOut, VOID *ArgIn);     //X轴最小移动单元移动菜单的Up动作
VOID* X_AixsMaxUnitMoveMenuDownAction(VOID *ArgOut, VOID *ArgIn);   //X轴最大移动单元移动菜单的down动作
VOID* X_AixsMidUnitMoveMenuDownAction(VOID *ArgOut, VOID *ArgIn);   //X轴中等移动单元移动菜单的down动作
VOID* X_AixsMinUnitMoveMenuDownAction(VOID *ArgOut, VOID *ArgIn);   //X轴最小移动单元移动菜单的down动作

VOID* Y_AixsMaxUnitMoveMenuUpAction(VOID *ArgOut, VOID *ArgIn);     //Y轴最大移动单元移动菜单的Up动作
VOID* Y_AixsMidUnitMoveMenuUpAction(VOID *ArgOut, VOID *ArgIn);     //Y轴中等移动单元移动菜单的Up动作
VOID* Y_AixsMinUnitMoveMenuUpAction(VOID *ArgOut, VOID *ArgIn);     //Y轴最小移动单元移动菜单的Up动作
VOID* Y_AixsMaxUnitMoveMenuDownAction(VOID *ArgOut, VOID *ArgIn);   //Y轴最大移动单元移动菜单的down动作
VOID* Y_AixsMidUnitMoveMenuDownAction(VOID *ArgOut, VOID *ArgIn);   //Y轴中等移动单元移动菜单的down动作
VOID* Y_AixsMinUnitMoveMenuDownAction(VOID *ArgOut, VOID *ArgIn);   //Y轴最小移动单元移动菜单的down动作


VOID* Z_AixsMaxUnitMoveMenuUpAction(VOID *ArgOut, VOID *ArgIn);     //Z轴最大移动单元移动菜单的Up动作
VOID* Z_AixsMidUnitMoveMenuUpAction(VOID *ArgOut, VOID *ArgIn);     //Z轴中等移动单元移动菜单的Up动作
VOID* Z_AixsMinUnitMoveMenuUpAction(VOID *ArgOut, VOID *ArgIn);     //Z轴最小移动单元移动菜单的Up动作
VOID* Z_AixsMaxUnitMoveMenuDownAction(VOID *ArgOut, VOID *ArgIn);   //Z轴最大移动单元移动菜单的down动作
VOID* Z_AixsMidUnitMoveMenuDownAction(VOID *ArgOut, VOID *ArgIn);   //Z轴中等移动单元移动菜单的down动作
VOID* Z_AixsMinUnitMoveMenuDownAction(VOID *ArgOut, VOID *ArgIn);   //Z轴最小移动单元移动菜单的down动作
VOID* Z_AixsLeastUnitMoveMenuDownAction(VOID *ArgOut, VOID *ArgIn);
VOID* Z_AixsLeastUnitMoveMenuUpAction(VOID *ArgOut, VOID *ArgIn);

VOID* ExtruderMaxUnitMoveMenuUpAction(VOID *ArgOut, VOID *ArgIn);     //挤出头最大移动单元移动菜单的Up动作
VOID* ExtruderMidUnitMoveMenuUpAction(VOID *ArgOut, VOID *ArgIn);     //挤出头中等移动单元移动菜单的Up动作
VOID* ExtruderMinUnitMoveMenuUpAction(VOID *ArgOut, VOID *ArgIn);     //挤出头最小移动单元移动菜单的Up动作
VOID* ExtruderMaxUnitMoveMenuDownAction(VOID *ArgOut, VOID *ArgIn);   //挤出头最大移动单元移动菜单的down动作
VOID* ExtruderMidUnitMoveMenuDownAction(VOID *ArgOut, VOID *ArgIn);   //挤出头中等移动单元移动菜单的down动作
VOID* ExtruderMinUnitMoveMenuDownAction(VOID *ArgOut, VOID *ArgIn);   //挤出头最小移动单元移动菜单的down动作

S32 X_AixsMoveMenuItemShow(VOID *Arg);   //Z轴移动菜单项的显示
S32 Y_AixsMoveMenuItemShow(VOID *Arg);   //Z轴移动菜单项的显示
S32 Z_AixsMoveMenuItemShow(VOID *Arg);   //Z轴移动菜单项的显示
S32 Z_AixsLeastMoveMenuItemShow(VOID *Arg);
S32 ExtruderMoveMenuItemShow(VOID *Arg);   //挤出头移动菜单项的显示

//X,Y,Z轴单元的运动的OK Action是默认的OK Action的动作就可以了
VOID* GoHomeMenuItemOKAction(VOID *ArgOut, VOID *ArgIn);   //回原点菜单项的内容

#ifdef __cplusplus
}
#endif



#endif
