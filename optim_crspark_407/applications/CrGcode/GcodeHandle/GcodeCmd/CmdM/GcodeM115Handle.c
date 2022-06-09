#include "GcodeM115Handle.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../../GcodeCommonFunc.h"
#include "../../GcodeType.h"
#include "CrInc/CrConfig.h"
#include "Common/Include/CrGlobal.h"

#define CMD_M115  "M115"
#define STR_M115_REPORT "FIRMWARE_NAME:C-SPARK " DETAILED_BUILD_VERSION " (" FIRMWARE_DATE ") SOURCE_CODE_URL:" SOURCE_CODE_URL " PROTOCOL_VERSION:" PROTOCOL_VERSION " MACHINE_TYPE:" MACHINE_NAME " EXTRUDER_COUNT:" STRINGIFY(EXTRUDER_NUM) " UUID:" MACHINE_UUID

static VOID CapLine(S8 * const Name, BOOL Enable)
{
    printf("Cap : %s : %d\n", Name, Enable ? 1 : 0 );

    return;
}

S32 GcodeM115Parse(const S8 *CmdBuff, VOID *Arg)
{
    if ( !CmdBuff || !Arg )
        return 0;

    S32 Len = strlen(CmdBuff);

    if ( Len <= 1 || (CmdBuff[0] != GCODE_CMD_M) )
        return 0;

    S8 Cmd[CMD_BUFF_LEN] = { 0 };
    if ( GetGcodeCmd(CmdBuff, Cmd) <= 0 )
        return 0;

    // M710
    if ( strcmp(Cmd, CMD_M115) != 0 )
        return 0;

    return 1;
}

S32 GcodeM115Exec(VOID *ArgPtr, VOID *ReplyResult)
{

    printf(STR_M115_REPORT);

//      // PAREN_COMMENTS
//      TERN_(PAREN_COMMENTS, cap_line(PSTR("PAREN_COMMENTS"), true));
//
//      // QUOTED_STRINGS  （引用的字符串）
//      TERN_(GCODE_QUOTED_STRINGS, cap_line(PSTR("QUOTED_STRINGS"), true));

      // SERIAL_XON_XOFF   ( 串口控制 )
    CapLine(("SERIAL_XON_XOFF"), EnableSerialOnOff);

      // BINARY_FILE_TRANSFER (M28 B1) 二进制文件
    CapLine(("BINARY_FILE_TRANSFER"), EnableBinaryFileTransfer);

      // EEPROM (M500, M501)
    CapLine(("EEPROM"), EnableEeprom);

      // Volumetric Extrusion (M200)
    CapLine(("VOLUMETRIC"), EnableVolumetric);

      // AUTOREPORT_TEMP (M155)
    CapLine(("AUTOREPORT_TEMP"), EnableAutoReportTemper);

      // PROGRESS (M530 S L, M531 <file>, M532 X L)
    CapLine(("PROGRESS"), 0);

      // Print Job timer M75, M76, M77
    CapLine(("PRINT_JOB"), 1);

      // AUTOLEVEL (G29)
    CapLine(("AUTOLEVEL"), EnableAutoLevel);

      // RUNOUT (M412, M600)
    CapLine(("RUNOUT"), EnableRunOutSensor);

      // Z_PROBE (G30)
    CapLine(("Z_PROBE"), EnableBedProbe);

      // MESH_REPORT (M420 V)  网格
    CapLine(("LEVELING_DATA"), EnableMeshLeveling);

      // BUILD_PERCENT (M73)  人工设置LCD的打印进度
    CapLine(("BUILD_PERCENT"), EnableSetProgressManually);

      // SOFTWARE_POWER (M80, M81) 电源控制
    CapLine(("SOFTWARE_POWER"), EnablePsuCtrl);

      // TOGGLE_LIGHTS (M355)    切换灯
    CapLine(("TOGGLE_LIGHTS"), EnableCaseLight);
    CapLine(("CASE_LIGHT_BRIGHTNESS"), EnableCaseLightBrightness);

      // EMERGENCY_PARSER (M108, M112, M410, M876) 紧急情况解析器
    CapLine(("EMERGENCY_PARSER"), EnableEmergencyParser);

      // PROMPT SUPPORT (M876)  迅速支持
    CapLine(("PROMPT_SUPPORT"), EnablePromptSupport);

      // SDCARD (M20, M23, M24, etc.)  支持SD卡
    CapLine(("SDCARD"), EnableSdSupport);

      // AUTOREPORT_SD_STATUS (M27 extension)    自动上报sd卡状态
    CapLine(("AUTOREPORT_SD_STATUS"), EnableAutoReportSdStatus);

      // LONG_FILENAME_HOST_SUPPORT (M33)  长文件名
    CapLine(("LONG_FILENAME"), EnableLongFileNameSupport);

      // THERMAL_PROTECTION  过热保护
    CapLine(("THERMAL_PROTECTION"), EnableThermallySafe);

      // MOTION_MODES (M80-M89)   运动模式
    CapLine(("MOTION_MODES"), EnableMotionModes);

      // ARC_SUPPORT (G2-G3)   曲线运动
    CapLine(("ARCS"), EnableArc);

      // BABYSTEPPING (M290)   微步进
    CapLine(("BABYSTEPPING"), EnableBabySteps);

      // CHAMBER_TEMPERATURE (M141, M191)   腔室温度
    CapLine(("CHAMBER_TEMPERATURE"), EnableHeatedChamber);

      // Machine Geometry  (X,Y,Z的最大的大小)
    if ( EnableGeometryReport )
    {
//        const xyz_pos_t dmin = { X_MIN_POS, Y_MIN_POS, Z_MIN_POS },
//                        dmax = { X_MAX_POS, Y_MAX_POS, Z_MAX_POS };
//        xyz_pos_t cmin = dmin, cmax = dmax;
//        apply_motion_limits(cmin);
//        apply_motion_limits(cmax);
//        const xyz_pos_t lmin = dmin.asLogical(), lmax = dmax.asLogical(),
//                        wmin = cmin.asLogical(), wmax = cmax.asLogical();
//        SERIAL_ECHOLNPAIR(
//          "area:{"
//            "full:{"
//              "min:{x:", lmin.x, ",y:", lmin.y, ",z:", lmin.z, "},"
//              "max:{x:", lmax.x, ",y:", lmax.y, ",z:", lmax.z, "}"
//            "},"
//            "work:{"
//              "min:{x:", wmin.x, ",y:", wmin.y, ",z:", wmin.z, "},"
//              "max:{x:", wmax.x, ",y:", wmax.y, ",z:", wmax.z, "}",
//            "}"
//          "}"
//        );
    }

    return 1;
}

S32 GcodeM115Reply(VOID *ReplyResult, S8 *Buff)
{

    return 1;
}
