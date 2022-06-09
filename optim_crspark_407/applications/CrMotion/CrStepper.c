
#include "stm32f4xx_hal.h"

#include "CrStepper.h"
#include "CrGpio/CrGpio.h"

#include <stdio.h>
#include <string.h>

struct StepperPinFd_t
{
    U32 EnnFd;     //使能引脚
    U32 DirPinFd;  //方向引脚
    U32 StepPinFd; //步进引脚
    U32 PdUartFd;  //串口文件描述符
};

struct StepperInfo_t
{
    S32    StepperFd;
    struct Stepper_t Stepper;
    struct StepperPinFd_t PinFd;
};
enum 
{
    X = 0,
    Y,
    Z,
    E,
    AXIS_MAX
};
    
#define NAME_MAX    (12)
char *StepperName[AXIS_MAX] = {X_STEPPER_NAME,Y_STEPPER_NAME,Z_STEPPER_NAME,E_STEPPER_NAME};

S32 X_Axis = -1,Y_Axis = -1,Z_Axis = -1,E_Axis = -1;

struct StepperInfo_t Info[AXIS_MAX] = {0};

/**
 * 函数功能: 初始化电机,打开电机对应的描述符
 * 函数参数: Name:电机名称,Flag:无实际意义,可忽略,Mode:无实际意义,可忽略
 * 输入: Name:电机名称 相关宏:X_STEPPER_NAME,Y_STEPPER_NAME,Z_STEPPER_NAME,E_STEPPER_NAME
 * 输出: 电机对应的描述符
 * 返回值: 电机对应的描述符,小于0:获取失败
 */
S32 CrStepperOpen(S8 *Name, S32 Flag, S32 Mode)
{
    if (Name == NULL)
    {
        return -1;
    }
    S32 i = 0,m = -1;

    while (i  < AXIS_MAX)
    {
        if ( 0 == strlen(Info[i].Stepper.Name) && ( m == -1 ))
        {
            m = i;
        }
        if ( strcmp(Name,Info[i].Stepper.Name) == 0)
        {
            printf("Stepper has been opened!\n");
            return -1;
        }
        i ++;
    }

    U32 len = strlen(Name);
    if(strlen(Name) > NAME_MAX)
    {
        len = NAME_MAX;
        printf("Name is too long,intercept the first %d bytes!\n",NAME_MAX);
    }
    if (len <= 0)
    {
        printf("Name is NUll\n");
    }
    strncpy(Info[m].Stepper.Name,Name,len);
    Info[m].StepperFd = m;

    return m;
}
/**
 * 函数功能: 关闭电机对应的描述符
 * 函数参数: Fd:电机对应的描述符
 * 输入:
 * 输出: 无
 * 返回值: 0:关闭成功,小于0:关闭失败
 */
S32 CrStepperClose(S32 Fd)
{
    if (Fd < 0 || Fd >= AXIS_MAX)
    {
        return -1;
    }
    U32 EnnFd = Info[Fd].PinFd.EnnFd;
    U32 DirPinFd = Info[Fd].PinFd.DirPinFd;
    U32 StepPinFd = Info[Fd].PinFd.StepPinFd;

    CrGpioClose(EnnFd);
    CrGpioClose(DirPinFd);
    CrGpioClose(StepPinFd);
    
    /*************************************************/
    //U32 PdUartFd = Info[Fd].PinFd.PdUartFd;
    //CrGpioClose(PdUartFd);
    /*************************************************/
    
    memset(&Info[Fd],0,sizeof(struct StepperInfo_t));
    Info[Fd].StepperFd = -1;

    return 0;
}
/**
 * 函数功能: 电机对应的指令操作
 * 函数参数: Fd:电机对应的描述符
 *           Cmd:控制指令(参考CrStepper.h中)
 *           Arg:参数
 * 输入: Fd:电机对应的描述符,Cmd:控制指令,Arg:拓展参数
 * 输出: Arg:拓展参数
 * 返回值: 0:关闭成功,小于0:关闭失败
 */
S32 CrStepperIoctl(S32 Fd, S32 Cmd, VOID *Arg)
{
    
    if (Fd >= AXIS_MAX || Fd < 0 || strlen(Info[Fd].Stepper.Name) == 0)
    {
        printf("Fd: %d Name:%s\n",Fd,Info[Fd].Stepper.Name);
        return -1;
    }
    U32 ret = -1;
    switch (Cmd)
    {
        case CRSTEPPER_SET_PINS:
        {      
            if (Arg == NULL)
            {
                goto invalid; 
            }
            struct StepperPins_t *Para = (struct StepperPins_t *)Arg;

            memcpy(&Info[Fd].Stepper.Pins,Para,sizeof(struct StepperPins_t));
            
            U32 *EnnPinFd = &Info[Fd].PinFd.EnnFd;
            U32 *DirPinFd = &Info[Fd].PinFd.DirPinFd;
            U32 *StepPinFd = &Info[Fd].PinFd.StepPinFd;

            struct PinInitParam_t PinInit = {0};
            PinInit.Mode = GPIO_MODE_OUTPUT_PP;
            PinInit.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
            *EnnPinFd = CrGpioOpen(Para->EnnPinName,0,0);
            *DirPinFd = CrGpioOpen(Para->DirPinName,0,0);
            *StepPinFd = CrGpioOpen(Para->StepPinName,0,0);

            if (CrGpioIoctl(*EnnPinFd, GPIO_INIT, &PinInit) < 0)
            {
                return -1;
            }         
            if( CrGpioIoctl(*DirPinFd, GPIO_INIT, &PinInit) < 0)
            {
                return -1;
            }         
            ret = CrGpioIoctl(*StepPinFd, GPIO_INIT,&PinInit);
        }
        break;
        case CRSTEPPER_GET_ENABLE_LEVEL:
        {   
            if (Arg == NULL)
            {
                goto invalid; 
            }
            U8 *level = (U8 *)Arg;
            *level = Info[Fd].Stepper.Pins.EnableLevel;
            ret = 0;
        }
        break;
        case CRSTEPPER_GET_POSITIVE_DIR_LEVEL:
        {      
            if (Arg == NULL)
            {
                goto invalid; 
            }
            U8 *Interval = (U8 *)Arg;
            *Interval = Info[Fd].Stepper.Pins.PositiveDirLevel;
            ret = 0;
        }
        break;
        case CRSTEPPER_SET_ATTR:
        {      
            if (Arg == NULL)
            {
                goto invalid; 
            }
            struct StepperAttr_t *Para = (struct StepperAttr_t *)Arg;
            memcpy(&Info[Fd].Stepper.Attr,Para,sizeof(struct StepperAttr_t));

            ret = 0;
        }
        break;
        case CRSTEPPER_GET_ATTR:
        {      
            if (Arg == NULL)
            {
                goto invalid; 
            }
            struct StepperAttr_t *Para = (struct StepperAttr_t *)Arg;
            memcpy(Para,&Info[Fd].Stepper.Attr,sizeof(struct StepperAttr_t));

            ret = 0;
        }
        break;
        case CRSTEPPER_SET_MOTION_ATTR:
        {      
            if (Arg == NULL)
            {
                goto invalid; 
            }
            struct StepperMotionAttr_t *Para = (struct StepperMotionAttr_t *)Arg;
            memcpy(&Info[Fd].Stepper.MotionAttr,Para,sizeof(struct StepperMotionAttr_t));

            ret = 0;
        }
        break;
        case CRSTEPPER_GET_MOTION_ATTR:
        {
            if (Arg == NULL)
            {
                goto invalid; 
            }
            struct StepperMotionAttr_t *Para = (struct StepperMotionAttr_t *)Arg;
            memcpy(Para,&Info[Fd].Stepper.MotionAttr,sizeof(struct StepperMotionAttr_t));
            ret = 0;
        }
        break;
        case CRSTEPPER_SET_POS:
        {
            if (Arg == NULL)
            {
                goto invalid; 
            }
            Info[Fd].Stepper.Pos.CurPosition = *(S32 *)Arg;
            ret = 0;
        }
        break;
        case CRSTEPPER_GET_POS:
        {
            if (Arg == NULL)
            {
                goto invalid;
            }
            S32 *Pos = (S32 *)Arg;
            *Pos = Info[Fd].Stepper.Pos.CurPosition;
            ret = 0;
        }      
        break;
        case CRSTEPPER_SET_POSITIVE_DIR:
        {
            U32 *DirPinFd = &Info[Fd].PinFd.DirPinFd;
            U8 Dir = Info[Fd].Stepper.Pins.PositiveDirLevel;
            ret = CrGpioIoctl(*DirPinFd,GPIO_SET_PINSTATE,&Dir);
        }      
        break;
        case CRSTEPPER_SET_NEGATIVE_DIR:
        {
            U32 *DirPinFd = &Info[Fd].PinFd.DirPinFd;
            U8 Dir = !(Info[Fd].Stepper.Pins.PositiveDirLevel);
            ret = CrGpioIoctl(*DirPinFd,GPIO_SET_PINSTATE,&Dir);
        }      
        break;
        case CRSTEPPER_SET_ENABLE:
        {
            U32 *EnnFd = &Info[Fd].PinFd.EnnFd;
            U8 Enn = Info[Fd].Stepper.Pins.EnableLevel;
            ret = CrGpioIoctl(*EnnFd,GPIO_SET_PINSTATE,&Enn);
        }
        break;
        case CRSTEPPER_SET_DISABLE:
        {
            U32 *EnnFd = &Info[Fd].PinFd.EnnFd;
            U8 Dis = !(Info[Fd].Stepper.Pins.EnableLevel);
            ret = CrGpioIoctl(*EnnFd,GPIO_SET_PINSTATE,&Dis);
        }
        break;
        case CRSTEPPER_SET_MOVE_ONE_IMPLUSE:
        {
            U32 *StepPinFd = &Info[Fd].PinFd.StepPinFd;
            U8 Low = 0,High = 1;
            volatile S32 i = 5;
            while(i--);
            ret = CrGpioIoctl(*StepPinFd,GPIO_SET_PINSTATE,&Low);
            if(ret < 0)
            {
                break;
            }
            i = 5;
            while(i--);
                
            ret = CrGpioIoctl(*StepPinFd,GPIO_SET_PINSTATE,&High);
        }
        break;
        case CRSTEPPER_GET_NAME:
        {
            if (Arg == NULL)
            {
                goto invalid;
            }
            S8 *Name = Info[Fd].Stepper.Name;
            S8 *Para = (S8 *)Arg;
            size_t len = strlen(Para);
            if(strlen(Name) > len)
            {
                printf(" Arg is too short,intercept the first %d bytes!\n",len);
            }
            strncpy(Para,Name,len);
            ret = 0;
        }
        break;
        case CRSTEPPER_GET_ENABLE_STATUS:
        {
            if (Arg == NULL)
            {
                goto invalid;
            }
            S8 *Para = (S8 *)Arg;
            U32 *EnnFd = &Info[Fd].PinFd.EnnFd;
            U8 Enn = Info[Fd].Stepper.Pins.EnableLevel;
            struct PinParam_t GpioPara = {0};
            ret = CrGpioIoctl(*EnnFd,GPIO_GET_PINSTATE,&GpioPara);
            if (GpioPara.PinState == Enn)
            {
                *Para = 1;
            }
            else
            {
                *Para = 0;
            }
        }
        break;
        case CRSTEPPER_SET_PIN_STATUS:
        {
            U32 *StepPinFd = &Info[Fd].PinFd.StepPinFd;

            CrGpioIoctl(*StepPinFd,GPIO_SET_PINSTATE,Arg);

            ret = 0;
        }
        break;
        default:

            //ret = 0;
            printf("command not support!\n");
        break;
    }
    return ret;
invalid:
    printf("parameter is invlid\n");
    return -1;
}


#ifdef STEPPER_TEST
extern int step;
VOID StepperTest()
{
    S32 EndStopFd_X = -1,EndStopFd_Y = -1,EndStopFd_Z = -1;
    U8 TragLevel_X = 1,TragLevel_Y = 1,TragLevel_Z = 1;
    
    X_Axis = CrStepperOpen(StepperName[X], 0, 0);
    if (X_Axis < 0)
    {
        printf("Open X Stepper failed!\n");
        return;
    }
    Y_Axis = CrStepperOpen(StepperName[Y], 0, 0);
    if(Y_Axis < 0)
    {
        printf("Open Y Stepper failed!\n");
        return;
    }
    Z_Axis = CrStepperOpen(StepperName[Z], 0, 0);
    if(Z_Axis < 0)
    {
        printf("Open Z Stepper failed!\n");
        return;
    }
    E_Axis = CrStepperOpen(StepperName[E], 0, 0);
    if(E_Axis < 0)
    {
        printf("Open E Stepper failed!\n");
        return;
    }
    printf("X_Axis:%d\n",X_Axis);
    printf("Y_Axis:%d\n",Y_Axis);
    printf("Z_Axis:%d\n",Z_Axis);
    printf("E_Axis:%d\n",E_Axis);

    struct StepperPins_t Pins = {0};
    struct StepperPins_t PinsY = {0};
    struct StepperPins_t PinsZ = {0};
    struct StepperPins_t PinsE = {0};

    Pins.EnnPinName = "PC3";
    Pins.DirPinName = "PB9";
    Pins.StepPinName = "PC2";
    Pins.EnableLevel = 0;
    Pins.PositiveDirLevel = 1;

    PinsY.EnnPinName = "PC3";
    PinsY.DirPinName = "PB7";
    PinsY.StepPinName = "PB8";
    PinsY.EnableLevel = 0;
    PinsY.PositiveDirLevel = 1;

    PinsZ.EnnPinName = "PC3";
    PinsZ.DirPinName = "PB5";
    PinsZ.StepPinName = "PB6";
    PinsZ.EnableLevel = 0;
    PinsZ.PositiveDirLevel = 0;

    PinsE.EnnPinName = "PC3";
    PinsE.DirPinName = "PB3";
    PinsE.StepPinName = "PB4";
    PinsE.EnableLevel = 0;
    PinsE.PositiveDirLevel = 1;

    if (CrStepperIoctl(X_Axis,CRSTEPPER_SET_PINS,&Pins) < 0)
    {
        printf("Set Pins Failed!\n");
        return;
    }
    if (CrStepperIoctl(Y_Axis,CRSTEPPER_SET_PINS,&PinsY) < 0)
    {
        printf("Set Pins Failed!\n");
        return;
    }
    if (CrStepperIoctl(Z_Axis,CRSTEPPER_SET_PINS,&PinsZ) < 0)
    {
        printf("Set Pins Failed!\n");
        return;
    }
    if (CrStepperIoctl(E_Axis,CRSTEPPER_SET_PINS,&PinsE) < 0)
    {
        printf("Set Pins Failed!\n");
        return;
    }
    struct StepperAttr_t Attr = {0};
    Attr.Perimeter = 40;
    Attr.StepAngle = 1.8;
    Attr.SubDivision = 16;
    Attr.StepsPerMm = 80;
    struct StepperAttr_t AttrY = {0};
    AttrY.Perimeter = 40;
    AttrY.StepAngle = 1.8;
    AttrY.SubDivision = 16;
    AttrY.StepsPerMm = 80;
    struct StepperAttr_t AttrZ = {0};
    AttrZ.Perimeter = 8;
    AttrZ.StepAngle = 1.8;
    AttrZ.SubDivision = 16;
    AttrZ.StepsPerMm = 400;
    struct StepperAttr_t AttrE = {0};
    AttrE.Perimeter = 34.4;
    AttrE.StepAngle = 1.8;
    AttrE.SubDivision = 16;
    AttrE.StepsPerMm = 93;
    if (CrStepperIoctl(X_Axis,CRSTEPPER_SET_ATTR,&Attr) < 0)
    {
        printf("Set Attr failed!\n");
        return;
    }
    if (CrStepperIoctl(Y_Axis,CRSTEPPER_SET_ATTR,&AttrY) < 0)
    {
        printf("Set Attr failed!\n");
        return;
    }
    if (CrStepperIoctl(Z_Axis,CRSTEPPER_SET_ATTR,&AttrZ) < 0)
    {
        printf("Set Attr failed!\n");
        return;
    }
    if (CrStepperIoctl(E_Axis,CRSTEPPER_SET_ATTR,&AttrE) < 0)
    {
        printf("Set Attr failed!\n");
        return;
    }
    struct StepperMotionAttr_t MotionAttr = {0};
    MotionAttr.Acceleration = 1000;
    MotionAttr.Deceleration = 1000;
    MotionAttr.Jerk = 10;
    MotionAttr.MaxVelocity = 500;
    MotionAttr.MinVelocity = 0;
    MotionAttr.Velocity = 100;

    struct StepperMotionAttr_t MotionAttrY = {0};
    MotionAttrY.Acceleration = 1000;
    MotionAttrY.Deceleration = 1000;
    MotionAttrY.Jerk = 10;
    MotionAttrY.MaxVelocity = 500;
    MotionAttrY.MinVelocity = 0;
    MotionAttrY.Velocity = 100;
    struct StepperMotionAttr_t MotionAttrZ = {0};
    MotionAttrZ.Acceleration = 1000;
    MotionAttrZ.Deceleration = 1000;
    MotionAttrZ.Jerk = 5;
    MotionAttrZ.MaxVelocity = 50;
    MotionAttrZ.MinVelocity = 0;
    MotionAttrZ.Velocity = 25;
    struct StepperMotionAttr_t MotionAttrE = {0};
    MotionAttrE.Acceleration = 1000;
    MotionAttrE.Deceleration = 1000;
    MotionAttrE.Jerk = 1;
    MotionAttrE.MaxVelocity = 50;
    MotionAttrE.MinVelocity = 0;
    MotionAttrE.Velocity = 25;
    if (CrStepperIoctl(X_Axis, CRSTEPPER_SET_MOTION_ATTR, &MotionAttr) < 0)
    {
        printf("Set Motion Attr Failed!\n");
        return;
    }
    if (CrStepperIoctl(Y_Axis, CRSTEPPER_SET_MOTION_ATTR, &MotionAttrY) < 0)
    {
        printf("Set Motion Attr Failed!\n");
        return;
    }
    if (CrStepperIoctl(Z_Axis, CRSTEPPER_SET_MOTION_ATTR, &MotionAttrZ) < 0)
    {
        printf("Set Motion Attr Failed!\n");
        return;
    }
    if (CrStepperIoctl(E_Axis, CRSTEPPER_SET_MOTION_ATTR, &MotionAttrE) < 0)
    {
        printf("Set Motion Attr Failed!\n");
        return;
    }
    /* Enable Stepper.*/
    if (CrStepperIoctl(X_Axis, CRSTEPPER_SET_ENABLE, NULL) < 0)
    {
        printf("Set Stepper Enable Failed!\n");
        return;
    }
    CrStepperIoctl(X_Axis, CRSTEPPER_SET_POSITIVE_DIR, NULL);
    CrStepperIoctl(Y_Axis, CRSTEPPER_SET_POSITIVE_DIR, NULL);
    CrStepperIoctl(Z_Axis, CRSTEPPER_SET_POSITIVE_DIR, NULL);


    if ((EndStopFd_X = CrEndStopOpen(ENDSTOPX_PIN_NAME, 0, 0)) < 0)
    {
        return;
    }
    if ((EndStopFd_Y = CrEndStopOpen(ENDSTOPY_PIN_NAME, 0, 0)) < 0)
    {
        return;
    }
    if ((EndStopFd_Z = CrEndStopOpen(ENDSTOPZ_PIN_NAME, 0, 0)) < 0)
    {
        return;
    }
    
    if (CrEndStopIoctl(EndStopFd_X , CRENDSTOP_SET_TRAGLEVEL, &TragLevel_X) < 0)
    {
        return;
    }
    if (CrEndStopIoctl(EndStopFd_Y , CRENDSTOP_SET_TRAGLEVEL, &TragLevel_Y) < 0)
    {
        return;
    }
    if (CrEndStopIoctl(EndStopFd_Z , CRENDSTOP_SET_TRAGLEVEL, &TragLevel_Z) < 0)
    {
        return;
    }
    CrMotionInit(X_Axis,EndStopFd_X,Y_Axis,EndStopFd_Y,Z_Axis,EndStopFd_Z,E_Axis);
    

    struct PlanAttr_t Ratio = {0};
    Ratio.Flow = 1.0f;
    CrPlanInit(X_Axis,Y_Axis,Z_Axis,E_Axis,&Ratio);
}
VOID EnnableTest()
{
    U32 i = 3200;
    while (i --)
    {
        if (CrStepperIoctl(X_Axis, CRSTEPPER_SET_MOVE_ONE_IMPLUSE, NULL) < 0)
        {
            printf("Move X Axis failed!\n");
            return;
        }
        if (CrStepperIoctl(Y_Axis, CRSTEPPER_SET_MOVE_ONE_IMPLUSE, NULL) < 0)
        {
            printf("Move X Axis failed!\n");
            return;
        }
        if (CrStepperIoctl(Z_Axis, CRSTEPPER_SET_MOVE_ONE_IMPLUSE, NULL) < 0)
        {
            printf("Move X Axis failed!\n");
            return;
        }
        rt_hw_us_delay(500);
    }
}

VOID CrPlanTest()
{
    struct Coordinate3d_t Begin = {0};
    Begin.X = 0.0;
    Begin.Y = 0.0;
    Begin.Z = 0.0;
    Begin.E = 0.0;
    struct Coordinate3d_t End = {0};
    End.X = 109.431;
    End.Y = 102.586;
    End.Z = 2;
    End.E = 1.08611;

    CrPlanCalc(&Begin, &End);

    printf("Pos-->X:");
    PrintFloat(Begin.X);
    printf("Y: ");
    PrintFloat(Begin.Y);
    printf("Z: ");
    PrintFloat(Begin.Z);
    printf("E: ");
    PrintFloat(Begin.E);
    printf("\n");
    
    End.X = 0;
    End.Y = 150;
    End.Z = 100;
    End.E = 10.0;

    printf("Pos-->X:");
    PrintFloat(Begin.X);
    printf("Y: ");
    PrintFloat(Begin.Y);
    printf("Z: ");
    PrintFloat(Begin.Z);
    printf("E: ");
    PrintFloat(Begin.E);
    printf("\n");
  
}

//MSH_CMD_EXPORT(StepperTest, Stepper sample);
//MSH_CMD_EXPORT(EnnableTest, Stepper sample);
//MSH_CMD_EXPORT(CrPlanTest, Stepper sample);

#endif

