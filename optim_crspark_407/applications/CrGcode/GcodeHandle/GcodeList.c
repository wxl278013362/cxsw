#include "GcodeList.h"
#include <string.h>
#include <stdio.h>

#include "./GcodeCmd/CmdT/GcodeT_Handle.h"
#include "./GcodeCmd/CmdG/GcodeG0Handle.h"
#include "./GcodeCmd/CmdG/GcodeG1Handle.h"
#include "./GcodeCmd/CmdG/GcodeG4Handle.h"
#include "./GcodeCmd/CmdG/GcodeG27Handle.h"
#include "./GcodeCmd/CmdG/GcodeG60Handle.h"
#include "./GcodeCmd/CmdG/GcodeG61Handle.h"
#include "./GcodeCmd/CmdG/GcodeG28Handle.h"
#include "./GcodeCmd/CmdG/GcodeG20Handle.h"
#include "./GcodeCmd/CmdG/GcodeG21Handle.h"
#include "./GcodeCmd/CmdG/GcodeG90Handle.h"
#include "./GcodeCmd/CmdG/GcodeG91Handle.h"
#include "./GcodeCmd/CmdG/GcodeG92Handle.h"
#include "./GcodeCmd/CmdM/GcodeM16Handle.h"
#include "./GcodeCmd/CmdM/GcodeM17Handle.h"
#include "./GcodeCmd/CmdM/GcodeM18OrM84Handle.h"
#include "./GcodeCmd/CmdM/GcodeM20Handle.h"
#include "./GcodeCmd/CmdM/GcodeM21Handle.h"
#include "./GcodeCmd/CmdM/GcodeM22Handle.h"
#include "./GcodeCmd/CmdM/GcodeM23Handle.h"
#include "./GcodeCmd/CmdM/GcodeM24Handle.h"
#include "./GcodeCmd/CmdM/GcodeM25Handle.h"
#include "./GcodeCmd/CmdM/GcodeM27Handle.h"
#include "./GcodeCmd/CmdM/GcodeM28Handle.h"
#include "./GcodeCmd/CmdM/GcodeM29Handle.h"
#include "./GcodeCmd/CmdM/GcodeM31Handle.h"
#include "./GcodeCmd/CmdM/GcodeM82Handle.h"
#include "./GcodeCmd/CmdM/GcodeM83Handle.h"
#include "./GcodeCmd/CmdM/GcodeM104Handle.h"
#include "./GcodeCmd/CmdM/GcodeM105Handle.h"
#include "./GcodeCmd/CmdM/GcodeM106Handle.h"
#include "./GcodeCmd/CmdM/GcodeM107Handle.h"
#include "./GcodeCmd/CmdM/GcodeM108Handle.h"
#include "./GcodeCmd/CmdM/GcodeM109Handle.h"
#include "./GcodeCmd/CmdM/GcodeM110Handle.h"
#include "./GcodeCmd/CmdM/GcodeM114Handle.h"
#include "./GcodeCmd/CmdM/GcodeM115Handle.h"
#include "./GcodeCmd/CmdM/GcodeM140Handle.h"
#include "./GcodeCmd/CmdM/GcodeM155Handle.h"
#include "./GcodeCmd/CmdM/GcodeM190Handle.h"
#include "./GcodeCmd/CmdM/GcodeM201Handle.h"
#include "./GcodeCmd/CmdM/GcodeM203Handle.h"
#include "./GcodeCmd/CmdM/GcodeM204Handle.h"
#include "./GcodeCmd/CmdM/GcodeM205Handle.h"
#include "./GcodeCmd/CmdM/GcodeM207Handle.h"
#include "./GcodeCmd/CmdM/GcodeM208Handle.h"
#include "./GcodeCmd/CmdM/GcodeM209Handle.h"
#include "./GcodeCmd/CmdM/GcodeM220Handle.h"
#include "./GcodeCmd/CmdM/GcodeM221Handle.h"
#include "./GcodeCmd/CmdM/GcodeM290Handle.h"
#include "./GcodeCmd/CmdM/GcodeM300Handle.h"
#include "./GcodeCmd/CmdM/GcodeM403Handle.h"
#include "./GcodeCmd/CmdM/GcodeM410Handle.h"
#include "./GcodeCmd/CmdM/GcodeM500Handle.h"
#include "./GcodeCmd/CmdM/GcodeM501Handle.h"
#include "./GcodeCmd/CmdM/GcodeM503Handle.h"
#include "./GcodeCmd/CmdM/GcodeM524Handle.h"
#include "./GcodeCmd/CmdM/GcodeM540Handle.h"
#include "./GcodeCmd/CmdM/GcodeM710Handle.h"
#include "./GcodeCmd/CmdM/GcodeM701Handle.h"
#include "./GcodeCmd/CmdM/GcodeM702Handle.h"
#include "./GcodeCmd/CmdM/GcodeM380Handle.h"
#include "./GcodeCmd/CmdM/GcodeM381Handle.h"
#include "./GcodeCmd/CmdM/GcodeM126Handle.h"
#include "./GcodeCmd/CmdM/GcodeM127Handle.h"
#include "./GcodeCmd/CmdM/GcodeM128Handle.h"
#include "./GcodeCmd/CmdM/GcodeM129Handle.h"
#include "./GcodeCmd/CmdM/GcodeM85Handle.h"
#include "./GcodeCmd/CmdM/GcodeM163Handle.h"
#include "./GcodeCmd/CmdM/GcodeM164Handle.h"
#include "./GcodeCmd/CmdM/GcodeM280Handle.h"
#include "./GcodeCmd/CmdM/GcodeM281Handle.h"
#include "./GcodeCmd/CmdM/GcodeM355Handle.h"
#include "./GcodeCmd/CmdM/GcodeM401Handle.h"
#include "./GcodeCmd/CmdM/GcodeM402Handle.h"
#include "./GcodeCmd/CmdM/GcodeM420Handle.h"
#include "./GcodeCmd/CmdM/GcodeM421Handle.h"
#include "./GcodeCmd/CmdM/GcodeM422Handle.h"
#include "./GcodeCmd/CmdG/GcodeG32Handle.h"
#include "./GcodeCmd/CmdG/GcodeG38Handle.h"
#include "./GcodeCmd/CmdM/GcodeM860Handle.h"
#include "./GcodeCmd/CmdM/GcodeM861Handle.h"
#include "./GcodeCmd/CmdM/GcodeM862Handle.h"
#include "./GcodeCmd/CmdM/GcodeM863Handle.h"
#include "./GcodeCmd/CmdM/GcodeM999Handle.h"
#include "./GcodeCmd/CmdM/GcodeM864Handle.h"
#include "./GcodeCmd/CmdM/GcodeM865Handle.h"
#include "./GcodeCmd/CmdM/GcodeM866Handle.h"
#include "./GcodeCmd/CmdM/GcodeM867Handle.h"
#include "./GcodeCmd/CmdM/GcodeM868Handle.h"
#include "./GcodeCmd/CmdM/GcodeM869Handle.h"
#include "./GcodeCmd/CmdM/GcodeM261Handle.h"
#include "./GcodeCmd/CmdM/GcodeM192Handle.h"
#include "./GcodeCmd/CmdM/GcodeM605Handle.h"
#include "./GcodeCmd/CmdM/GcodeM672Handle.h"
#include "./GcodeCmd/CmdM/GcodeM7Handle.h"
#include "./GcodeCmd/CmdM/GcodeM8Handle.h"
#include "./GcodeCmd/CmdM/GcodeM9Handle.h"



const struct GcodeItem_t GcodeList[] =    //元素个数不能为0，为0会报错
{
    {(S8*)"G0", GcodeG0Parse, GcodeG0Exec, GcodeG0Reply},
    {(S8*)"G1", GcodeG1Parse, GcodeG1Exec, GcodeG1Reply},
    {(S8*)"G4", GcodeG4Parse, GcodeG4Exec, GcodeG4Reply},
    {(S8*)"G27", GcodeG27Parse, GcodeG27Exec, GcodeG27Reply},
    {(S8*)"G28", GcodeG28Parse, GcodeG28Exec, GcodeG28Reply},
    {(S8*)"G21", GcodeG21Parse, GcodeG21Exec, GcodeG21Reply},
    {(S8*)"G60", GcodeG60Parse, GcodeG60Exec, GcodeG60Reply},
	{(S8*)"G20", GcodeG20Parse, GcodeG20Exec, GcodeG20Reply},
    {(S8*)"G61", GcodeG61Parse, GcodeG61Exec, GcodeG61Reply},
    {(S8*)"G92", GcodeG92Parse, GcodeG92Exec, GcodeG92Reply},
    {(S8*)"G90", GcodeG90Parse, GcodeG90Exec, GcodeG90Reply},
    {(S8*)"G91", GcodeG91Parse, GcodeG91Exec, GcodeG91Reply},
    {(S8*)"M104", GcodeM104Parse, GcodeM104Exec, GcodeM104Reply},
    {(S8*)"M105", GcodeM105Parse, GcodeM105Exec, GcodeM105Reply},
    {(S8*)"M106", GcodeM106Parse, GcodeM106Exec, GcodeM106Reply},
    {(S8*)"M107", GcodeM107Parse, GcodeM107Exec, GcodeM107Reply},
    {(S8*)"M108", GcodeM108Parse, GcodeM108Exec, GcodeM108Reply},
    {(S8*)"M109", GcodeM109Parse, GcodeM109Exec, GcodeM109Reply},
    {(S8*)"M110", GcodeM110Parse, GcodeM110Exec, GcodeM110Reply},
    {(S8*)"M114", GcodeM114Parse, GcodeM114Exec, GcodeM114Reply},
    {(S8*)"M115", GcodeM115Parse, GcodeM115Exec, GcodeM115Reply},
    {(S8*)"M140", GcodeM140Parse, GcodeM140Exec, GcodeM140Reply},
    {(S8*)"M155", GcodeM155Parse, GcodeM155Exec, GcodeM155Reply},
    {(S8*)"M201", GcodeM201Parse, GcodeM201Exec, GcodeM201Reply},
    {(S8*)"M203", GcodeM203Parse, GcodeM203Exec, GcodeM203Reply},
    {(S8*)"M204", GcodeM204Parse, GcodeM204Exec, GcodeM204Reply},
    {(S8*)"M205", GcodeM205Parse, GcodeM205Exec, GcodeM205Reply},
    {(S8*)"M207", GcodeM207Parse, GcodeM207Exec, GcodeM207Reply},
    {(S8*)"M208", GcodeM208Parse, GcodeM208Exec, GcodeM208Reply},
    {(S8*)"M209", GcodeM209Parse, GcodeM209Exec, GcodeM209Reply},
    {(S8*)"M220", GcodeM220Parse, GcodeM220Exec, GcodeM220Reply},
    {(S8*)"M221", GcodeM221Parse, GcodeM221Exec, GcodeM221Reply},
    {(S8*)"M290", GcodeM290Parse, GcodeM290Exec, GcodeM290Reply},
    {(S8*)"M300", GcodeM300Parse, GcodeM300Exec, GcodeM300Reply},
    {(S8*)"M403", GcodeM403Parse, GcodeM403Exec, GcodeM403Reply},
    {(S8*)"M410", GcodeM410Parse, GcodeM410Exec, GcodeM410Reply},
    {(S8*)"M500", GcodeM500Parse, GcodeM500Exec, GcodeM500Reply},
    {(S8*)"M501", GcodeM501Parse, GcodeM501Exec, GcodeM501Reply},
    {(S8*)"M503", GcodeM503Parse, GcodeM503Exec, GcodeM503Reply},
    {(S8*)"M540", GcodeM540Parse, GcodeM540Exec, GcodeM540Reply},
    {(S8*)"M701", GcodeM701Parse, GcodeM701Exec, GcodeM701Reply},
    {(S8*)"M702", GcodeM702Parse, GcodeM702Exec, GcodeM702Reply},
    {(S8*)"M710", GcodeM710Parse, GcodeM710Exec, GcodeM710Reply},
    {(S8*)"M17", GcodeM17Parse, GcodeM17Exec, GcodeM17Reply},
    {(S8*)"M18", GcodeM18OrM84Parse, GcodeM18OrM84Exec, GcodeM18OrM84Reply},
    {(S8*)"M84", GcodeM18OrM84Parse, GcodeM18OrM84Exec, GcodeM18OrM84Reply},
    {(S8*)"M190", GcodeM190Parse, GcodeM190Exec, GcodeM190Reply},
    {(S8*)"M82", GcodeM82Parse, GcodeM82Exec, GcodeM82Reply},
    {(S8*)"M83", GcodeM83Parse, GcodeM83Exec, GcodeM83Reply},
    {(S8*)"T", GcodeCmdT_Parse, GcodeCmdT_Exec, GcodeCmdT_Reply},     //T命令是工具选择
    {(S8*)"M16", GcodeM16Parse, GcodeM16Exec, GcodeM16Reply},
    {(S8*)"M20", GcodeM20Parse, GcodeM20Exec, GcodeM20Reply},
    {(S8*)"M21", GcodeM21Parse, GcodeM21Exec, GcodeM21Reply},
    {(S8*)"M22", GcodeM22Parse, GcodeM22Exec, GcodeM22Reply},
    {(S8*)"M23", GcodeM23Parse, GcodeM23Exec, GcodeM23Reply},
    {(S8*)"M24", GcodeM24Parse, GcodeM24Exec, GcodeM24Reply},
    {(S8*)"M25", GcodeM25Parse, GcodeM25Exec, GcodeM25Reply},
    {(S8*)"M27", GcodeM27Parse, GcodeM27Exec, GcodeM27Reply},
    {(S8*)"M31", GcodeM31Parse, GcodeM31Exec, GcodeM31Reply},
    {(S8*)"M28", GcodeM28Parse, GcodeM28Exec, GcodeM28Reply},
    {(S8*)"M29", GcodeM29Parse, GcodeM29Exec, GcodeM29Reply},
    {(S8*)"M524", GcodeM524Parse, GcodeM524Exec, GcodeM524Reply},
    {(S8*)"M380", GcodeM380Parse, GcodeM380Exec, GcodeM380Reply},
    {(S8*)"M381", GcodeM381Parse, GcodeM381Exec, GcodeM381Reply},
#if HAS_HEATER_0
    {(S8*)"M126", GcodeM126Parse, GcodeM126Exec, GcodeM126Reply},
    {(S8*)"M127", GcodeM127Parse, GcodeM127Exec, GcodeM127Reply},
#endif
#if HAS_HEATER_1
    {(S8*)"M128", GcodeM128Parse, GcodeM128Exec, GcodeM128Reply},
    {(S8*)"M129", GcodeM129Parse, GcodeM129Exec, GcodeM129Reply},
#endif
    {(S8*)"M85", GcodeM85Parse, GcodeM85Exec, GcodeM85Reply},
#ifdef MIXING_EXTRUDER
    {(S8*)"M163", GcodeM163Parse, GcodeM163Exec, GcodeM163Reply},
    {(S8*)"M164", GcodeM164Parse, GcodeM164Exec, GcodeM164Reply},
#endif
#if HAS_SERVOS
    {(S8*)"M280", GcodeM280Parse, GcodeM280Exec, GcodeM280Reply},
#endif
#ifdef EDITABLE_SERVO_ANGLES
    {(S8*)"M281", GcodeM281Parse, GcodeM281Exec, GcodeM281Reply},
#endif
#ifdef CASE_LIGHT_ENABLE
    {(S8*)"M355", GcodeM355Parse, GcodeM355Exec, GcodeM355Reply},
#endif
#if HAS_BED_PROBE
    {(S8*)"M401", GcodeM401Parse, GcodeM401Exec, GcodeM401Reply},
    {(S8*)"M402", GcodeM402Parse, GcodeM402Exec, GcodeM402Reply},
#endif
#ifdef HAS_LEVELING
    {(S8*)"M420", GcodeM420Parse, GcodeM420Exec, GcodeM420Reply},
#endif
#ifdef AUTO_BED_LEVELING_BILINEAR
    {(S8*)"M421", GcodeM421Parse, GcodeM421Exec, GcodeM421Reply},
#endif
#ifdef Z_STEPPER_AUTO_ALIGN
    {(S8*)"M422", GcodeM422Parse, GcodeM422Exec, GcodeM422Reply},
#endif
    {(S8*)"G32", GcodeG32Parse, GcodeG32Exec, GcodeG32Reply},
    {(S8*)"G38", GcodeG38Parse, GcodeG38Exec, GcodeG38Reply},
    {(S8*)"M999", GcodeM999Parse, GcodeM999Exec, GcodeM999Reply},
    {(S8*)"M860", GcodeM860Parse, GcodeM860Exec, GcodeM860Reply},
    {(S8*)"M861", GcodeM861Parse, GcodeM861Exec, GcodeM861Reply},
    {(S8*)"M862", GcodeM862Parse, GcodeM862Exec, GcodeM862Reply},
    {(S8*)"M863", GcodeM863Parse, GcodeM863Exec, GcodeM863Reply},
    {(S8*)"M864", GcodeM864Parse, GcodeM864Exec, GcodeM864Reply},
    {(S8*)"M865", GcodeM865Parse, GcodeM865Exec, GcodeM865Reply},
    {(S8*)"M866", GcodeM866Parse, GcodeM866Exec, GcodeM866Reply},
    {(S8*)"M867", GcodeM867Parse, GcodeM867Exec, GcodeM867Reply},
    {(S8*)"M868", GcodeM868Parse, GcodeM868Exec, GcodeM868Reply},
    {(S8*)"M869", GcodeM869Parse, GcodeM869Exec, GcodeM869Reply},
#if EXPERIMENTAL_I2CBUS
    {(S8*)"M261", GcodeM261Parse, GcodeM261Exec, GcodeM261Reply},
#endif  // EXPERIMENTAL_I2CBUS
    {(S8*)"M192", GcodeM192Parse, GcodeM192Exec, GcodeM192Reply},
    {(S8*)"M605", GcodeM605Parse, GcodeM605Exec, GcodeM605Reply},
    {(S8*)"M672", GcodeM672Parse, GcodeM672Exec, GcodeM672Reply},
    {(S8*)"M7", GcodeM7Parse, GcodeM7Exec, GcodeM7Reply},
    {(S8*)"M8", GcodeM8Parse, GcodeM8Exec, GcodeM8Reply},
    {(S8*)"M9", GcodeM9Parse, GcodeM9Exec, GcodeM9Reply},
    {NULL, NULL, NULL, NULL}   //结束标志
};

