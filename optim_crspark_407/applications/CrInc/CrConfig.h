#ifndef _CRCONFIG_H
#define _CRCONFIG_H

#define USING_RTTHREAD 1

#if USING_RTTHREAD
#define USING_OS_RTTHREAD       //定义使用rtthread接口的宏
#else
#define USING_OS_LINUX          //定义使用linux接口的宏
#endif

#define LCD_LANGUAGE_EN


#define MENUITEM_STAY_TIME  10000
#define ROOT_SHOW_INTERVAL 1000

/*************************消息队列名称***********************************/

#define PARAM_OPT_REPORT_QUEUE    "/OptRep"    //Rep表示report
#define PARAM_OPT_SET_QUEUE       "/OptSet"
#define PARAM_TEMPER_REPORT_QUEUE "/TemperRep"
#define PARAM_TEMPER_SET_QUEUE    "/TemperSet"
#define PARAM_MOTION_REPORT_QUEUE "/MotionRep"
#define PARAM_MOTION_SET_QUEUE    "/MotionSet"
#define PARAM_FANS_REPORT_QUEUE   "/FanRep"
#define PARAM_FANS_REQUEST_QUEUE  "/FanReq"    //进行获取和设置

#define MOTION_MOVE_OPT_QUEUE     "/MoveOpt"
#define MOTION_POS_REPORT_QUEUE   "/RepPos"
#define MOTION_POS_REQ_QUEUE      "/ReqPos"
#define MOTION_ATTR_REPORT_QUEUE  "/RepAttr"
#define MOTION_ATTR_SET_QUEUE     "/SetAttr"

#define TEMPER_BED_REPORT_QUEUE       "/BedRep"
#define TEMPER_BED_REQUEST_QUEUE      "/BedReq"    //进行获取和设置
#define TEMPER_NOZZLE_REPORT_QUEUE    "/NozRep"
#define TEMPER_NOZZLE_REQUEST_QUEUE   "/NozReq"    //进行获取和设置
#define FANS_REPORT_QUEUE             "/FansRep"
#define FANS_REQUEST_QUEUE            "/FansReq"    //进行获取和设置

#define SD_LIST_REPORT_QUEUE      "/ListRep"    //文件列表
#define SD_LIST_REQUEST_QUEUE     "/ListReq"
#define SD_STATUS_REPORT_QUEUE    "/StatusRep"   //SD卡状态
#define SD_STATUS_REQUEST_QUEUE   "/StatusReq"
#define SD_PRINTFILE_REPORT_QUEUE "/FileRep"
#define SD_PRINTFILE_REQ_QUEUE    "/FileReq"
#define SD_PRINTFILE_SET_QUEUE    "/FileSet"    //打印文件名设置

#define PRINT_INFO_REPORT_QUEUE  "/InfoRep"  //打印信息上报
#define PRINT_INFO_REQ_QUEUE  "/InfoReq"
#define PRINT_PRINTOPT_QUEUE     "/PrintOpt"

#define MESSAGE_LINE_INFO_QUEUE  "/MsgLine"


/*************************消息队列名称***********************************/

/**************************其他信息*********************************/

#ifndef SHORT_BUILD_VERSION
  #define SHORT_BUILD_VERSION "2.0.0"
#endif

#ifndef DETAILED_BUILD_VERSION
  #define DETAILED_BUILD_VERSION SHORT_BUILD_VERSION
#endif

#ifndef PROTOCOL_VERSION
  #define PROTOCOL_VERSION "1.0"
#endif

#ifndef MACHINE_NAME
  #define MACHINE_NAME "C-Spark"
#endif

#ifndef SOURCE_CODE_URL
  #define SOURCE_CODE_URL "https://github.com/MarlinFirmware/Marlin"
#endif

#ifndef DEFAULT_MACHINE_UUID
  #define DEFAULT_MACHINE_UUID "cede2a2f-41a2-4748-9b12-c55c62f367ff"
#endif

#ifdef DEFAULT_SOURCE_CODE_URL
  #undef  SOURCE_CODE_URL
  #define SOURCE_CODE_URL DEFAULT_SOURCE_CODE_URL
#endif

#ifdef CUSTOM_MACHINE_NAME
  #undef  MACHINE_NAME
  #define MACHINE_NAME CUSTOM_MACHINE_NAME
#elif defined(DEFAULT_MACHINE_NAME)
  #undef  MACHINE_NAME
  #define MACHINE_NAME DEFAULT_MACHINE_NAME
#endif

#ifndef MACHINE_UUID
  #define MACHINE_UUID DEFAULT_MACHINE_UUID
#endif

// Macros to make a string from a macro
#define STRINGIFY_(M) #M
#define STRINGIFY(M) STRINGIFY_(M)

/**************************其他信息*********************************/

/***************************打印机信息********************************/

#define FIRMWARE_NAME     "Creality C-SPARK"
#define DEVICE_FW_VERSION "Version " SHORT_BUILD_VERSION
#define FIRMWARE_DATE     "2021-11-9"
#define DEVICE_MODE_NAME  MACHINE_NAME
#define COMPANY_DOMAIN    "www.creality.com"
#define EXTRUDER_NUM      1
#define UART_BAUDRATE     115200

/**************************打印机信息*********************************/

/*************************消息队列名称***********************************/

/******************************默认运动参数*******************************************/
/*
 * Preheat PLA Conf
 * */
#define PLA_MATERIAL_PREHEAT_BED_TEMP       60
#define PLA_MATERIAL_PREHEAT_NOZ_TEMP       200
#define PLA_MATERIAL_PRE_FAN_SPEED          100

/*
 * Preheat ABS Conf
 * */
#define ABS_MATERIAL_PREHEAT_BED_TEMP       70
#define ABS_MATERIAL_PREHEAT_NOZ_TEMP       240
#define ABS_MATERIAL_PRE_FAN_SPEED          100

/*
 * Velocity Default
 * */
#define TIMES                               1

#define VELOCITY_V_MAX_X                    50 //35
#define VELOCITY_V_MAX_Y                    50 //35
#define VELOCITY_V_MAX_Z                    (8)   //5
#define VELOCITY_V_MAX_E                    (20*TIMES)  //25
#define VELOCITY_V_RETRACT                  (50*TIMES)  //35
#define VELOCITY_V_TRAV                     50  //35
#define VELOCITY_V_MIN                      0
#define VELOCITY_V_TRAV_MIN                 0

/*
 *  Acceleration Default
 * */
#define ACCELERATION_ACCEL                  900//950//500//850//750
#define ACCELERATION_RETRACT                1000//1850//1000//1700//1500
#define ACCELERATION_TRAVEL                 1000//1850//1000//1700//1500
#define ACCELERATION_MAX_X                  800//1700//1850//1000//1700//1500
#define ACCELERATION_MAX_Y                  800//1700//1850//1000//1700//1500
#define ACCELERATION_MAX_Z                  300
#define ACCELERATION_MAX_E                  800//2000//2000//2000//2000

/*
 *  Jerk Default
 * */
#define VX_JERK                             6 //10 20220211
#define VY_JERK                             6 //10 20220211
#define VZ_JERK                             (TIMES*0.3f)
#define VE_JERK                             (TIMES*5.00f)

/*
 *  Steps Default
 * */
#define X_STEPS_MM                          80.00f
#define Y_STEPS_MM                          80.00f
#define Z_STEPS_MM                          400.00f
#define E_STEPS_MM                          93.00f
/*
 *  Filament Default
 * */
#define FIL_DIA                             (1.750f)   //单位：毫米

/*
 *  PrintRatio       //打印速率
 * */
#define PRINTF_RATIO                        100

/*
 *  FlowRatio        //流量比
 * */
#define FLOW_RATIO                          100

/*
 *  Temperture
 * */
#define NOZZLE_MAX_TEMP                     260
#define BED_MAX_TEMP                        110


/*
 *  Segment length
 * */

#define SEGMENT_LENGTH                      5.0                   //单位mm，即小于5mm，保持原值；大于5mm，最长不超过10mm。
/*****************************默认运动参数************************************/

/******************************喷头相关参数***********************************/
#define HOTENDS 1


/*****************挤出机相关参数***************/
#define EXTRUDERS 1

/************************************************舵机模块*********************************************/
#define HAS_SERVOS                      1      // 是否支持伺服电机
#define NUM_SERVOS                      1      // 舵机的个数
#define Z_PROBE_SERVO_NR                0
//#define EDITABLE_SERVO_ANGLES           1      // TODO:打开该宏CrEepromApi.cpp实现有问题，待解决
#define BLTOUCH

/*****************************默认引脚参数************************************/
//#define X_DIR_PIN       "PF1"
//#define X_STEP_PIN      "PE9"
//#define X_ENABLE_PIN    "PF2"
//
//#define Y_DIR_PIN       "PE8"
//#define Y_STEP_PIN      "PE11"
//#define Y_ENABLE_PIN    "PD7"
//
//#define Z_DIR_PIN       "PC2"
//#define Z_STEP_PIN      "PE13"
//#define Z_ENABLE_PIN    "PC0"
//
//#define E_DIR_PIN       "PE7"  //"PA0"
//#define E_STEP_PIN      "PD15" //"PE14"
//#define E_ENABLE_PIN    "PA3"  //"PC3"

//407ZET6芯片
#define X_DIR_PIN       "PA15"
#define X_STEP_PIN      "PD14"
#define X_ENABLE_PIN    "PD11"

#define Y_DIR_PIN       "PB0"
#define Y_STEP_PIN      "PD15"
#define Y_ENABLE_PIN    "PG5"

#define Z_DIR_PIN       "PB1"
#define Z_STEP_PIN      "PF7"
#define Z_ENABLE_PIN    "PG4"

#define E_DIR_PIN       "PB8"  //"PA0"
#define E_STEP_PIN      "PF8" //"PE14"
#define E_ENABLE_PIN    "PG3"  //"PC3"

/***************************测试的宏***************************************/
//#define MAX_MSG 1024
//#define MSG_SIZE 100

//#define TEST_MSGQUEUE
//#define TEST_THREAD
//#define TEST_CONTROLLER

//#define TEST_TIME

//#define TEMPSENSOR_TEST
//#define STEPPER_TEST
#define CR_ADC_TEST

/***************************测试的宏*************************************/



/***********************************************************************/
#define STR_BUSY_PROCESSING                 "busy: processing"
#define MSG_OK                              "ok"





/***********************************************************************/






#endif
