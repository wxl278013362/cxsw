/****************************************************************************
  Copyright(C)    2020,      Creality Co.Ltd.
  File name :     CrGpio.c
  Author :        chenli
  Version:        1.0
  Description:    Gpio Posix
  Other:
  Mode History:
          <author>        <time>      <version>   <desc>
          chenli          2020-9-30    V1.0.0.1
****************************************************************************/

#include "CrGpio.h"
#include <string.h>
#include <stdlib.h>
#include "CrInc/CrLcdShowStruct.h"
//#include <rtthread.h>

#define MAX_GPIO_PIN_NUM  48


struct GpioGroupParam
{
    S8              Name[MAX_NAME_SIZE];    //GPIO组的名称
    S8              Offset;                 //
    GPIO_TypeDef    *GpioGroup;             //GPIO组的TypeDef变量指针
};

struct GpioGroupParam GpioArray[] =
{
    {GPIO_PA, 16, GPIOA},
    {GPIO_PB, 17, GPIOB},
    {GPIO_PC, 18, GPIOC},
    {GPIO_PD, 19, GPIOD},
    {GPIO_PE, 20, GPIOE},
    {GPIO_PF, 21, GPIOF},
    {GPIO_PG, 22, GPIOG},
};

static S32 RegArrary[MAX_GPIO_PIN_NUM] = {0};

static S32 GpioGroupCount(void)
{
    return sizeof(GpioArray) / sizeof(GpioArray[0]);
}

static S32 IsFdHighBitsVaild(S32 Fd)
{
    S32 Ret = -1;

    S32 Temp = Fd;
    Temp >>= 16;

    U8 CountH = 0;          //

    while(Temp){
        Temp = Temp & (Temp - 1);
        CountH++;
    };

    return ( CountH == 1 ) ? CountH : Ret;
}


static S32 IsFdLowBitsVaild(S32 Fd)
{
    S32 Ret = -1;

    S32 Temp = Fd;
    Temp &= 0xFFFF;

    U8 CountL = 0;          //

    while(Temp){
        Temp = Temp & (Temp - 1);
        CountL++;
    };

    return (CountL > 0) ? CountL : Ret; //
}


static S32 IsFdRegistered(S32 Fd)
{
    S32 Ret = -1;
    U8 Flag = 0;

    for ( Flag = 0; Flag < MAX_GPIO_PIN_NUM; Flag++ )
    {
        if ( Fd == RegArrary[Flag] )
        {
            Ret = Flag;
            break;
        }
    }

    return Ret;
}


S32 IsGpioFdVaild(S32 Fd)
{
    S32 Ret = 0;

    if ( Fd < 65536 )   //0x01 << 16
        return -1;

    if (( IsFdHighBitsVaild(Fd) < 0 ) || ( IsFdLowBitsVaild(Fd) < 0 ) || ( IsFdRegistered(Fd) < 0 ))
        Ret = -1;
    
    return Ret;
}


static S32 GetGpioGroupIndex(S32 Fd)
{
    S32 Ret = -1;
    U8 Flag = 0;
    U8 OffSet = 0;
    S32 Temp = Fd; 

    for ( Flag = 0; Flag <= 8 * sizeof(S32); Flag++, Temp >>= 1 )
    {
        if ((Temp & 0x01) == 0x01) 
        {
            OffSet = Flag;
        }
    }

    for ( Flag = 0; Flag < GpioGroupCount(); Flag++ )
    {
        if ( OffSet == GpioArray[Flag].Offset )
        {
            Ret = Flag;
            break;
        }
    }

    return Ret;
}


S32 CrGpioOpen(S8 *Name, U32 Flag, U32 Mode)
{
    S32 Ret = -1;

    if ( !Name )
        return Ret;

    S8 *Ptr = Name + 2;

    S32 i = 0, j = 0;
    for ( i = 0; i < GpioGroupCount(); i++ )
    {
        if ( strncmp(Name, GpioArray[i].Name, 2) == 0 )
        {
            if ( strncmp(Ptr, "ALL", 3) == 0 )
            {
                Ret = (0x01 << GpioArray[i].Offset) | 0xFFFF;
            }
            else
            {
                S32 PinNum = atoi(Ptr);
                Ret = (0x01 << GpioArray[i].Offset) | (0x01 << PinNum);
            }
            for ( j = 0; j < MAX_GPIO_PIN_NUM; j++ )
            {
                if ( RegArrary[j] == 0 || RegArrary[j] == Ret)
                {
                    RegArrary[j] = Ret;
                    break;
                }
            }
            break;
        }
    }

    return Ret;
}


S32 CrGpioWrite(S32 Fd, S8 *Buff, S32 Len)
{
    S32 Ret = -1;

    if ( !Buff )
        return Ret;

    S32 Temp = Fd;
    if ( IsGpioFdVaild(Temp) < 0 )
        return Ret;

    S32 SubScript = GetGpioGroupIndex(Temp);
    if ( SubScript < 0 || SubScript >= GpioGroupCount() )
        return Ret; 

    Temp &= 0xFFFF;//
    HAL_GPIO_WritePin(GpioArray[SubScript].GpioGroup, (U16)Temp, ((struct PinParam_t *)Buff)->PinState);

    return 0;
}


VOID CrGpioClose(S32 Fd)
{
    if ( IsGpioFdVaild(Fd) < 0 )
        return ;

    U8 Flag = 0;
    for ( Flag = 0; Flag < MAX_GPIO_PIN_NUM; Flag++ )
    {
        if ( Fd == RegArrary[Flag] )
        {
            RegArrary[Flag] = 0;
            break;
        }
    }

    S32 Temp = Fd;
    S32 SubScript = GetGpioGroupIndex(Temp);
    Temp &= 0xFFFF;//
    HAL_GPIO_DeInit(GpioArray[SubScript].GpioGroup, Temp);

    return ;
}


S32 CrGpioRead(S32 Fd, S8 *Buff, S32 Len)
{
    S32 Ret = -1;
    S32 Temp = Fd;

    if ( ( !Buff ) ||  IsGpioFdVaild(Temp) < 0 )
        return Ret;

    S32 SubScript = -1;
    SubScript = GetGpioGroupIndex(Temp);
    if ( SubScript < 0 || SubScript >= GpioGroupCount() )
        return Ret; 

    Temp &= 0xFFFF;
    ((struct PinParam_t *)Buff)->PinState = HAL_GPIO_ReadPin(GpioArray[SubScript].GpioGroup, (U16)Temp);

    return 0;
}


static VOID CrGpioInit(S32 Fd, VOID *Param)
{
    if ( ( Fd < 0 ) ||  ( !Param ) )
        return ;

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    S32 Temp = Fd;
    S32 SubScript = -1;
    SubScript = GetGpioGroupIndex(Temp);
    if ( SubScript < 0 || SubScript >= GpioGroupCount())
        return ; 

    switch ( SubScript )
    {
        case 0:
        {
            __HAL_RCC_GPIOA_CLK_ENABLE(); 
            break;
        }
        case 1:
        {
            __HAL_RCC_GPIOB_CLK_ENABLE();
            break;
        }
        case 2: 
        {
            __HAL_RCC_GPIOC_CLK_ENABLE(); 
            break;
        }
        case 3: 
        {
            __HAL_RCC_GPIOD_CLK_ENABLE(); 
            break;
        }
        default:
             break;
    }

    GPIO_InitStruct.Pin = (Temp & 0xFFFF);
    GPIO_InitStruct.Mode = ((struct PinInitParam_t *)Param)->Mode;
    GPIO_InitStruct.Pull = ((struct PinInitParam_t *)Param)->Pull;
    GPIO_InitStruct.Speed = ((struct PinInitParam_t *)Param)->Speed;
    HAL_GPIO_Init(GpioArray[SubScript].GpioGroup, &GPIO_InitStruct);

    return ;
}

S32 CrGpioIoctl(S32 Fd, S32 Cmd, VOID *Param)
{
    S32 Ret = -1;
    S32 Temp = Fd;

    if ( ( !Param ) || ( IsGpioFdVaild(Temp) < 0 ) )
        return Ret;

    switch ( Cmd )
    {
        case GPIO_INIT: 
        {
            CrGpioInit(Temp, Param);
            break;
        }
        case GPIO_GET_PINSTATE: 
        {
            CrGpioRead(Temp, (S8 *)Param, 0);
            break;
        }
        case GPIO_SET_PINSTATE:
        {
            CrGpioWrite(Temp, (S8 *)Param, 0);
            break;
        }
        default:
            break;
    }

    return 0;
}


S32 CrGpioGetPinName(S32 PinIndex, S8 *NameBuff)
{
    if ( (PinIndex < 0) || !NameBuff )
        return -1;

    return 0;
}


#ifdef CRGPIO_TEST

static VOID Beep(S32 Fd)
{
    struct PinParam_t pin_st = {GPIO_PIN_RESET};
    pin_st.PinState = GPIO_PIN_SET;
    CrGpioWrite(Fd, (S8 *)&pin_st, 0);
    HAL_Delay(1000);
    pin_st.PinState = GPIO_PIN_RESET;
    CrGpioWrite(Fd, (S8 *)&pin_st, 0);
    HAL_Delay(1000);
    CrGpioClose(Fd);

    return ;
}

static S32 BeepInit()
{
    struct PinInitParam_t pin_init = {0};

    pin_init.Mode = GPIO_MODE_OUTPUT_PP;
    pin_init.Pull = GPIO_NOPULL;
    pin_init.Speed = GPIO_SPEED_FREQ_HIGH;

    S32 fd = CrGpioOpen("PC6", 0, 0);
    CrGpioIoctl(fd, GPIO_INIT, (VOID *)&pin_init);
    if (fd < 0)
    {
        printf("BeepInit open fail!\n");
    }

    return fd;
}

S32 CrGpioTestMain(VOID)
{
    S32 BeepFd = BeepInit();
    Beep(BeepFd);
}

#endif /*CRGPIO_TEST*/
