/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-12-03     RT-Thread    first version
 */

#include <rtthread.h>
#include <stm32f4xx_hal.h>
#include <rtdevice.h>
#include <drivers/pin.h>
#include <drv_common.h>
#include "CrInc/CrConfig.h"
#include "CrController/CrController.h"
#include "CrView/CrShowView.h"
#include "Common/Include/CrThread.h"
#include "Common/Include/CrSleep.h"
#include "CrMenu/Show/Lcm12864Driver.h"
#include "CrMenu/Show/Lcm12864Show.h"
#include "CrMenu/Show/ShowAction.h"
#include "CrLcd/KnobEvent/CrKnobEvent.h"
#include "CrLcd/LcdDriver/Src/CrLcdSpi.h"
#include "CrHeater/Adc/CrAdc.h"
#include "CrHeater/Pid/CrPid.h"
#include "CrHeater/Pwm/CrHeater.h"
#include "CrHeater/Sensor/CrSensor.h"
#include "CrHeater/HeaterDriver/CrHeaterDriver.h"
#include "CrHeater/Pwm/PwmCallBack.h"
#include "CrHeater/Fan/CrFanCtrl.h"
#include "CrHeater/Pwm/FanPwmCallback.h"
#include "CrModel/CrFans.h"
#include "CrModel/CrGcode.h"
#include "CrModel/CrMotion.h"
#include "CrModel/CrParam.h"
#include "CrModel/CrHeaterManager.h"
#include "CrModel/CrPrinter.h"
#include "CrModel/CrSdCard.h"
#include "CrSdCard/Driver/CrSdCardDriver.h"
#include "CrMsgQueue/QueueCommonFuns.h"
#include "CrModel/GcodeSource.h"
#include "CrSerial/CrSerial.h"

#include "CrGcode/GcodeHandle/GcodeCmd/CmdG/GcodeG0Handle.h"
#include "CrGcode/GcodeHandle/GcodeCmd/CmdG/GcodeG1Handle.h"
#include "CrGcode/GcodeHandle/GcodeCmd/CmdG/GcodeG28Handle.h"
#include "CrGcode/GcodeHandle/GcodeCmd/CmdG/GcodeG90Handle.h"
#include "CrGcode/GcodeHandle/GcodeCmd/CmdG/GcodeG91Handle.h"
#include "CrGcode/GcodeHandle/GcodeCmd/CmdG/GcodeG92Handle.h"
#include "CrGcode/GcodeHandle/GcodeCmd/CmdM/GcodeM104Handle.h"
#include "CrGcode/GcodeHandle/GcodeCmd/CmdM/GcodeM109Handle.h"
#include "CrGcode/GcodeHandle/GcodeCmd/CmdM/GcodeM140Handle.h"
#include "CrGcode/GcodeHandle/GcodeCmd/CmdM/GcodeM190Handle.h"
#include "CrGcode/GcodeHandle/GcodeCmd/CmdM/GcodeM107Handle.h"
#include "CrGcode/GcodeHandle/GcodeCmd/CmdM/GcodeM106Handle.h"
#include "CrGcode/GcodeHandle/GcodeCmd/CmdM/GcodeM710Handle.h"
#include "CrGcode/GcodeHandle/GcodeCmd/CmdM/GcodeM24Handle.h"
#include "CrGcode/GcodeHandle/GcodeCmd/CmdM/GcodeM145Handle.h"
#include "CrGcode/GcodeHandle/GcodeCmd/CmdM/GcodeM155Handle.h"

#include "CrMotion/CrEndStop.h"
#include "CrMotion/StepperApi.h"
#include "CrLog/CrLog.h"
#include "CrMenu/language/Language.h"
/************菜单项的头文件***************/

#include "CrMenu/MenuItem/AboutPrinterMenuItem.h"
#include "CrMenu/MenuItem/AccelerationMenuItem.h"
#include "CrMenu/MenuItem/DisableStepperMenuItem.h"
#include "CrMenu/MenuItem/FilamentMenuItem.h"
#include "CrMenu/MenuItem/HomeOffsetMenuItem.h"
#include "CrMenu/MenuItem/JerkMenuItem.h"
#include "CrMenu/MenuItem/MoveAxisMenuItem.h"
#include "CrMenu/MenuItem/ParamMenuItem.h"
#include "CrMenu/MenuItem/PreheatMaterialMenuItem.h"
#include "CrMenu/MenuItem/PrintMenuItem.h"
#include "CrMenu/MenuItem/SdCardMenuItem.h"
#include "CrMenu/MenuItem/StepsMenuItem.h"
#include "CrMenu/MenuItem/TemperatureMenuItem.h"
#include "CrMenu/MenuItem/VelocityMenuItem.h"
#include "CrMenu/MenuItem/PrintfSpeed.h"
#include "CrMenu/MenuItem/FlowMenuItem.h"
/************菜单项的头文件***************/


Lcm12864Driver *Driver = NULL;
Lcm12864Show *Show = NULL;
S32 SpiFd = -1;

//model

CrSensor *SensorBed = NULL;
CrSensor *SensorNozzle = NULL;
CrPid *PidBed = NULL;
CrPid *PidNozzle = NULL;
CrHeater *HeaterBed = NULL;
CrHeater *HeaterNozzle = NULL;


CrPthread_t ThreadModel;

VOID CrMotionInitialzation(struct MotionDriver_t *Driver,struct MotionAttr_t *MotionAttr)
{

    /* EndStop Pins.*/
    Driver->EndStopX_PinName = (S8 *)ENDSTOPX_PIN_NAME;
    Driver->EndStopX_TrigLevel = 0;
    Driver->EndStopY_PinName = (S8 *)ENDSTOPY_PIN_NAME;
    Driver->EndStopY_TrigLevel = 0;
    Driver->EndStopZ_PinName = (S8 *)ENDSTOPZ_PIN_NAME;
    Driver->EndStopZ_TrigLevel = 0;

    /* X Axis Pins.*/
    Driver->StepperX_Pin.EnnPinName = (S8 *)X_ENABLE_PIN;
    Driver->StepperX_Pin.DirPinName = (S8 *)X_DIR_PIN;
    Driver->StepperX_Pin.StepPinName = (S8 *)X_STEP_PIN;
    Driver->StepperX_Pin.EnableLevel = 0;
    Driver->StepperX_Pin.PositiveDirLevel = 1;
    Driver->StepperX_Pin.PdUartName = 0;

    /* Y Axis Pins.*/
    Driver->StepperY_Pin.EnnPinName = (S8 *)Y_ENABLE_PIN;
    Driver->StepperY_Pin.DirPinName = (S8 *)Y_DIR_PIN;
    Driver->StepperY_Pin.StepPinName = (S8 *)Y_STEP_PIN;
    Driver->StepperY_Pin.EnableLevel = 0;
    Driver->StepperY_Pin.PositiveDirLevel = 1;
    Driver->StepperY_Pin.PdUartName = 0;

    /* Z Axis Pins.*/
    Driver->StepperZ_Pin.EnnPinName = (S8 *)Z_ENABLE_PIN;
    Driver->StepperZ_Pin.DirPinName = (S8 *)Z_DIR_PIN;
    Driver->StepperZ_Pin.StepPinName = (S8 *)Z_STEP_PIN;
    Driver->StepperZ_Pin.EnableLevel = 0;
    Driver->StepperZ_Pin.PositiveDirLevel = 0;
    Driver->StepperZ_Pin.PdUartName = 0;

    /* E Axis Pins.*/
    Driver->StepperE_Pin.EnnPinName = (S8 *)E_ENABLE_PIN;
    Driver->StepperE_Pin.DirPinName = (S8 *)E_DIR_PIN;
    Driver->StepperE_Pin.StepPinName = (S8 *)E_STEP_PIN;
    Driver->StepperE_Pin.EnableLevel = 0;
    Driver->StepperE_Pin.PositiveDirLevel = 1;
    Driver->StepperE_Pin.PdUartName = 0;

    /* X Axis Attr.*/
    MotionAttr->AttrX.StepsPerMm =80;
    MotionAttr->AttrX.Perimeter = 40;
    MotionAttr->AttrX.StepAngle = 1.8;
    MotionAttr->AttrX.SubDivision = 16;

    /* Y Axis Attr.*/
    MotionAttr->AttrY.StepsPerMm =80;
    MotionAttr->AttrY.Perimeter = 40;
    MotionAttr->AttrY.StepAngle = 1.8;
    MotionAttr->AttrY.SubDivision = 16;

    /* Z Axis Attr.*/
    MotionAttr->AttrZ.StepsPerMm =400;
    MotionAttr->AttrZ.Perimeter = 8;
    MotionAttr->AttrZ.StepAngle = 1.8;
    MotionAttr->AttrZ.SubDivision = 16;

    /* E Axis Attr.*/
    MotionAttr->AttrE.StepsPerMm =93;
    MotionAttr->AttrE.Perimeter = 34.4;
    MotionAttr->AttrE.StepAngle = 1.8;
    MotionAttr->AttrE.SubDivision = 16;

    /* X Axis Motion Attr.*/
    MotionAttr->MotionAttrX.Acceleration = 1000;//1000;
    MotionAttr->MotionAttrX.Deceleration = 1000;//1000;
    MotionAttr->MotionAttrX.Jerk = 10;
    MotionAttr->MotionAttrX.MaxVelocity = 50;
    MotionAttr->MotionAttrX.MinVelocity = 0;
    MotionAttr->MotionAttrX.Velocity = 50;

    /* Y Axis Motion Attr.*/
    MotionAttr->MotionAttrY.Acceleration = 1000;
    MotionAttr->MotionAttrY.Deceleration = 1000;
    MotionAttr->MotionAttrY.Jerk = 10;
    MotionAttr->MotionAttrY.MaxVelocity = 50;
    MotionAttr->MotionAttrY.MinVelocity = 0;
    MotionAttr->MotionAttrY.Velocity = 50;

    /* Z Axis Motion Attr.*/
    MotionAttr->MotionAttrZ.Acceleration = 1000;
    MotionAttr->MotionAttrZ.Deceleration = 1000;
    MotionAttr->MotionAttrZ.Jerk = 5;
    MotionAttr->MotionAttrZ.MaxVelocity = 5;
    MotionAttr->MotionAttrZ.MinVelocity = 0;
    MotionAttr->MotionAttrZ.Velocity = 5;

    /* E Axis Motion Attr.*/
    MotionAttr->MotionAttrE.Acceleration = 3000;
    MotionAttr->MotionAttrE.Deceleration = 3000;
    MotionAttr->MotionAttrE.Jerk = 10;
    MotionAttr->MotionAttrE.MaxVelocity = 10;
    MotionAttr->MotionAttrE.MinVelocity = 0;
    MotionAttr->MotionAttrE.Velocity = 10;

    /* X Position.*/
    MotionAttr->PosX.CurPosition = 0;
    MotionAttr->PosX.EndStopped = 0;

    /* Y Position.*/
    MotionAttr->PosY.CurPosition = 0;
    MotionAttr->PosY.EndStopped = 0;

    /* Z Position.*/
    MotionAttr->PosZ.CurPosition = 0;
    MotionAttr->PosZ.EndStopped = 0;

    /* E Position.*/
    MotionAttr->PosE.CurPosition = 0;
    MotionAttr->PosE.EndStopped = 0;

}

VOID MotionParamSet(struct ViewMotionMsg_t Param, struct MotionAttr_t *Motion)
{

    Motion->MotionAttrX.Acceleration = Param.MotionParam.StepperXAttr.MaxAccel == 0 ? ACCELERATION_MAX_X : Param.MotionParam.StepperXAttr.MaxAccel;
    Motion->MotionAttrX.Deceleration = Param.MotionParam.StepperXAttr.MaxAccel == 0 ? ACCELERATION_MAX_X : Param.MotionParam.StepperXAttr.MaxAccel;
    Motion->MotionAttrX.Jerk = (S32)Param.MotionParam.StepperXAttr.Jerk == 0 ? 1 : (S32)Param.MotionParam.StepperXAttr.Jerk;
    Motion->MotionAttrX.MaxVelocity = Param.MotionParam.StepperXAttr.MaxVel == 0 ? VELOCITY_V_MAX_X : Param.MotionParam.StepperXAttr.MaxVel;
    Motion->MotionAttrX.MinVelocity = VELOCITY_V_MIN;
    Motion->MotionAttrX.Velocity = VELOCITY_V_MAX_X;

    Motion->MotionAttrY.Acceleration = Param.MotionParam.StepperYAttr.MaxAccel == 0 ? ACCELERATION_MAX_Y : Param.MotionParam.StepperYAttr.MaxAccel;
    Motion->MotionAttrY.Deceleration = Param.MotionParam.StepperYAttr.MaxAccel == 0 ? ACCELERATION_MAX_Y : Param.MotionParam.StepperYAttr.MaxAccel;
    Motion->MotionAttrY.Jerk = (S32)Param.MotionParam.StepperYAttr.Jerk == 0 ? 1 : (S32)Param.MotionParam.StepperYAttr.Jerk;
    Motion->MotionAttrY.MaxVelocity = Param.MotionParam.StepperYAttr.MaxVel == 0 ? VELOCITY_V_MAX_Y : Param.MotionParam.StepperYAttr.MaxVel;
    Motion->MotionAttrY.MinVelocity = VELOCITY_V_MIN;
    Motion->MotionAttrY.Velocity = VELOCITY_V_MAX_Y;

    Motion->MotionAttrZ.Acceleration = Param.MotionParam.StepperZAttr.MaxAccel == 0 ? ACCELERATION_MAX_Z : Param.MotionParam.StepperZAttr.MaxAccel;
    Motion->MotionAttrZ.Deceleration = Param.MotionParam.StepperZAttr.MaxAccel == 0 ? ACCELERATION_MAX_Z : Param.MotionParam.StepperZAttr.MaxAccel;
    Motion->MotionAttrZ.Jerk = (S32)Param.MotionParam.StepperZAttr.Jerk == 0 ? 1 : (S32)Param.MotionParam.StepperZAttr.Jerk;
    Motion->MotionAttrZ.MaxVelocity = Param.MotionParam.StepperZAttr.MaxVel == 0 ? VELOCITY_V_MAX_Z : Param.MotionParam.StepperZAttr.MaxVel;
    Motion->MotionAttrZ.MinVelocity = VELOCITY_V_MIN;
    Motion->MotionAttrZ.Velocity = VELOCITY_V_MAX_Z;

    Motion->MotionAttrE.Acceleration = Param.MotionParam.StepperEAttr.Param.MaxAccel == 0 ? ACCELERATION_MAX_E : Param.MotionParam.StepperEAttr.Param.MaxAccel;
    Motion->MotionAttrE.Deceleration = Param.MotionParam.StepperEAttr.RetractAccel == 0 ? ACCELERATION_MAX_E : Param.MotionParam.StepperEAttr.RetractAccel;
    Motion->MotionAttrE.Jerk = (S32)Param.MotionParam.StepperEAttr.Param.Jerk == 0 ? 1 : (S32)Param.MotionParam.StepperEAttr.Param.Jerk;
    Motion->MotionAttrE.MaxVelocity = Param.MotionParam.StepperEAttr.Param.MaxVel == 0 ? VELOCITY_V_MAX_E : Param.MotionParam.StepperEAttr.Param.MaxVel;
    Motion->MotionAttrE.MinVelocity = VELOCITY_V_MIN;
    Motion->MotionAttrE.Velocity = VELOCITY_V_MAX_E;

    struct FeedrateAttr_t Attr = {0};
    /*get all attributes befpre change some attributes you need.*/
    StepperGetFeedrateAttr(&Attr);
    Attr.VelMinXYZ = Param.MotionParam.VelMinXYZ;
    if (Param.MotionParam.TravelAccelXYZ > 0)
        Attr.TravelAccelXYZ = Param.MotionParam.TravelAccelXYZ;
    else
        Attr.TravelAccelXYZ = Motion->MotionAttrX.Acceleration == 0 ? ACCELERATION_TRAVEL : Motion->MotionAttrX.Acceleration;
    if ( Param.MotionParam.AccelXYZ > 0)
        Attr.AccelXYZ = Param.MotionParam.AccelXYZ;
    else
        Attr.AccelXYZ = Motion->MotionAttrX.Acceleration == 0 ? ACCELERATION_ACCEL : Motion->MotionAttrX.Acceleration;
    
    Attr.TravelVelMinXYZ = Param.MotionParam.TravelVelMinXYZ == 0 ? VELOCITY_V_TRAV_MIN : Param.MotionParam.TravelVelMinXYZ;
    Attr.Feedrate = VELOCITY_V_TRAV;
    
    StepperSetFeedrateAttr(&Attr);
}

S32 CreateModel()
{
    GcodeModel = new CrGcode((S8 *)"/GcodeQueue");
    if ( !GcodeModel )
        return 0;

    S32 FdBed = CrHeaterOpen((S8 *)BED_HEATTING_PIN_NAME, 0, 0);
    if ( FdBed <= 0 )
        return 0;
    S32 level = 1;
    CrHeaterIoctl(FdBed, HEATER_HEAT_POWERON, &level);

    S32 Fd = CrHeaterOpen((S8 *)NOZZLE_HEATTING_PIN_NAME, 0, 0);
    if ( Fd <= 0 )
    {
        CrHeaterClose(FdBed);
        rt_kprintf("create heater model failed\n");
        return 0;
    }

    CrHeaterIoctl(Fd, HEATER_HEAT_POWERON, &level);

    struct PidAttr_t BedAttr = {109.36, 1.23 ,1900.92};
    struct PidParam_t BedParam = {0, 3, 255};
    PidBed = new CrPositionalPid(BedAttr, BedParam);

    SensorBed = new CrTemperSensor((S8 *)SENSOR_BED_NAME, (S8 *)BED_PIN_NAME, 3.3, NULL, NULL);
    HeaterBed = new CrHeater(FdBed, HeartPwmRun);

    BedHeater = new CrHeaterManager(PidBed, HeaterBed, SensorBed,\
            (S8 *)TEMPER_BED_REPORT_QUEUE, (S8 *)TEMPER_BED_REQUEST_QUEUE);

#if 1
    if ( (HOTENDS) >= 1 )
    {
        struct PidAttr_t Attr = {20.24, 0.31 ,320.78};//{8.137, 0.1588 ,109.2269};
        struct PidParam_t Param = {0.1, 10, 255};


        PidNozzle = new CrPositionalPid(Attr, Param);
        SensorNozzle = new CrTemperSensor((S8 *)SENSOR_NOZZLE_NAME, (S8 *)NOZZLE_PIN_NAME, 3.3, NULL, NULL);
        HeaterNozzle = new CrHeater(Fd, HeartPwmRun);
        HotEndArr[0] = new CrHeaterManager(PidNozzle, HeaterNozzle, SensorNozzle, \
                (S8 *)TEMPER_NOZZLE_REPORT_QUEUE, (S8 *)TEMPER_NOZZLE_REQUEST_QUEUE);
    }
#endif

    struct SdQueueName_t Name
    {
        SD_LIST_REPORT_QUEUE,
        SD_LIST_REQUEST_QUEUE,
        SD_STATUS_REPORT_QUEUE,
        SD_STATUS_REQUEST_QUEUE,
        SD_PRINTFILE_REPORT_QUEUE,
        SD_PRINTFILE_REQ_QUEUE,
        SD_PRINTFILE_SET_QUEUE,
        MESSAGE_LINE_INFO_QUEUE
    };

    /*modify by Charley : Make sure to mount the SD card first before Param Model !*/
    SdCardModel = new CrSdCard(Name, (S8 *)SD_DETECT_PIN);

    ParamModel = new CrParam();
    if ( (HOTENDS) >= 1 )
    {
        Printer = new CrPrinter(GcodeModel, HotEndArr[0], BedHeater);
    }
    else
    {
        Printer = new CrPrinter(GcodeModel, NULL, BedHeater);
    }

    SdCardModel->SetPrint(Printer);

    struct MotionDriver_t MotionDriver = {0};
    struct MotionAttr_t MotionAttr = {0};

    CrMotionInitialzation(&MotionDriver, &MotionAttr);
    MotionParamSet(ParamModel->MotionParmGet(), &MotionAttr);
    MotionModel = new CrMotion(MotionDriver, MotionAttr);

    /*Fans control*/
    if ( PWM_FAN_COUNT )
    {
        PwmFan[0] = new CrFans(CrFanOpen((S8 *)MODEL_FAN_PIN, 0, 0), GPIO_PIN_SET, \
            (S8 *)FANS_REPORT_QUEUE, (S8 * )FANS_REQUEST_QUEUE, FanPwmRun);
    }

    return 1;
}


VOID GcodeCmdInit()
{
    GcodeM24Init();
    GcodeM109Init();
    GcodeM190Init();
    GcodeM145Init();
    GcodeM155Init();

    return ;
}


S32 CreateView()
{
    Languages::LanguageMapInit();
    S32 SpiFd = CrLcdSpiOpen((S8 *)SPI1_NAME, 0, 0);
    if ( SpiFd < 0 )
    {
        return 0;
    }

    CrLcdSpiIoctl(SpiFd, SPI_INIT, NULL);  //初始化SPI

    Driver = new Lcm12864Driver(SpiFd, 5, 22);
    Show = new Lcm12864Show(Driver);

    if( Show->Open((S8 *)"Lcm12864", O_CREAT, O_RDWR) < 0 )
    {
        delete Driver;
        delete Show;
        Driver = NULL;
        Show = NULL;
        return 0;
    }

    ShowInit(Show, 1);
    CrShowViewInit();

    return 1;
}

S32 CreateController()
{
    S32 KnobFd = CrKnobOpen((S8 *)"knobevent", 0, 0);
    if ( KnobFd <= 0 )
        return 0;

    struct KnobAttr_t Attr = {EC11_PINA_NAME, EC11_PINB_NAME, EC11_PINBOTTUN_NAME };

    if( CrKnobIoctl(KnobFd, KNOB_CMD_SET_KNOB_ATTR, &Attr) <= 0)
    {
        CrKnobClose(KnobFd);
        return 0;
    }
    CrKnobIoctl(KnobFd, KNOB_CMD_SCAN_START, NULL);
    CrControllerInit(KnobFd);

    return 1;
}

VOID CreateUart()
{
    /******************/
//    S8 Cmd[32] = {0};
    S32 Fd = CrSerialOpen("uart3", RT_DEVICE_FLAG_INT_RX, 0);   //注意发送不要用中断模式，如果发送的内容多不停的发生中断。
    //S32 Fd = CrSerialOpen("uart3", RT_DEVICE_FLAG_DMA_RX, 0);
    GcodeUart *Uart  = new GcodeUart(Fd);
    /******************/
    if ( Printer )
        Printer->SetUart(Uart);
}

S32 InitMemuItem()
{
    VelocityQueueInit();
    TemperQueueInit();
    StepQueueInit();
    SdCardFileListInit();
    PrintOptMenuInit();
    TargetTemperQueueInit();
    ParamOptMenuInit();
    MoveAxisMenuInit();
    JerkQueueInit();
    FilamentQueueInit();
    DisableStepperMenuInit();
    AccelQueueInit();
    PrintfSpeedQueueInit();
    FlowQueueInit();

    return 1;
}
#include "stm32f4xx_hal_uart.h"
void MoveHomeTest();
void testPrint();

int main(void)
{
    int DelayCnt = 0;

//    if ( CreateView() <= 0)
//        return 0;
//
//    ShowBootScreen(NULL);
//
//    InitMemuItem();
//
//    if ( CreateController() <= 0)/* pth00 */
//        return 0;

    if ( CreateModel() <= 0 )/* pth01 */
        return 0;
//    if ( CrLogOpen((S8 *)"./",LOG_TO_FILE) < 0)
//    {
//        printf("CrLog Open Failed! sizeof(HeaterManagerAttr_t) = %d, sizeof(ViewHeaterMsg_t) = %d\n", sizeof(HeaterManagerAttr_t), sizeof(ViewHeaterMsg_t));
//    }

    rt_kprintf("========Start========\n");
    //LOG("LOG_Start");

    CreateUart();

    GcodeCmdInit();

    while ( 1 )
    {
        ++ DelayCnt;
#if 1
        if (DelayCnt == 100)
        {
            DelayCnt = 0;
            BedHeater->Exec();

            if ( (HOTENDS) >= 1 )
            {
                if ( HotEndArr[0] )
                    HotEndArr[0]->Exec();
            }

            if ( (PWM_FAN_COUNT) >= 1 )
                if (PwmFan[0])
                    PwmFan[0]->Exec();

            ParamModel->Exec();
            Printer->Exec();
            SdCardModel->Exec();
            MotionModel->Exec();
        }
#endif
//        EndStopHandler();


        CrM_Sleep(10);
    }

    return RT_EOK;
}
void PrintFile()
{
    Printer->SetPrintStatus(PRINTING);
}
void MoveHomeTest()
{
    U8 Mark = (HOME_OPT_X | HOME_OPT_Y | HOME_OPT_Z);
    MotionModel->AutoMoveHome(Mark);
}
void stepperEntest()
{
    MotionModel->EnableStepper(StepperX);
    MotionModel->EnableStepper(StepperY);
    MotionModel->EnableStepper(StepperZ);
    MotionModel->EnableStepper(StepperE);
}
void stepperDistest()
{
    MotionModel->DisableAllSteppers();
}
void PipFanOpen()
{
    rt_pin_mode(GET_PIN(E,5), PIN_MODE_OUTPUT);
    rt_pin_write(GET_PIN(E,5), PIN_HIGH);
}

MSH_CMD_EXPORT(PipFanOpen,PipFanOpen);
MSH_CMD_EXPORT(MoveHomeTest,MoveHomeTest);
MSH_CMD_EXPORT(PrintFile,PrintFile);
MSH_CMD_EXPORT(stepperDistest,stepperDistest);
MSH_CMD_EXPORT(stepperEntest,stepperEntest);



void testPrint()
{
    if ( !Printer || !Printer->GetGcodeModel() )
        return;

    static S32 Count = 0;
    switch ( Count )
    {
        case 0:
            Printer->GetGcodeModel()->PutGcodeCmd("G0 X20\n", sizeof("G0 X20\n"), 1);
            break;
        case 1:
            Printer->GetGcodeModel()->PutGcodeCmd("M106 S255\n", sizeof("M106 S255\n"), 1);
            break;
        case 2:
            Printer->GetGcodeModel()->PutGcodeCmd("M105\n", sizeof("M105\n"), 1);
            break;
        case 3:
            Printer->GetGcodeModel()->PutGcodeCmd("G0 Y20\n", sizeof("G0 Y20\n"), 1);
            break;
        case 4:
            Printer->GetGcodeModel()->PutGcodeCmd("G0 Z20\n", sizeof("G0 Z20\n"), 1);
            break;
        case 5:
            Printer->GetGcodeModel()->PutGcodeCmd("G0 X-20 Y-20\n", sizeof("G0 X-20 Y-20\n"), 1);
            break;
        case 6:
            Printer->GetGcodeModel()->PutGcodeCmd("G0 Z-20\n", sizeof("G0 Z-20\n"), 1);
            break;
        case 7:
            Printer->GetGcodeModel()->PutGcodeCmd("M107\n", sizeof("M107\n"), 1);
            break;
    }

    Count = ++Count % 8;

    return;
}



