/****************************************************************************
  Copyright(C)    2020,      Creality Co.Ltd.
  File name :     Type.h
  Author :        WangJiao
  Version:        1.0
  Description:    定义类型
  Other:
  Mode History:
          <author>        <time>      <version>   <desc>
          wangjaio     2020-7-20     V1.0.0.1
****************************************************************************/
#ifndef _TYPE_H_
#define _TYPE_H_

#ifdef __cplusplus
extern "C" {
#endif
/****************************************************************************/

typedef char               S8;
typedef unsigned char      U8;
typedef short              S16;
typedef unsigned short     U16;
typedef int                S32;
typedef unsigned int       U32;
typedef long long          S64;
typedef unsigned long long U64;
typedef unsigned char      BIT;
typedef unsigned char      BOOL;
typedef void               VOID;
typedef float              FLOAT;
typedef double             DOUBLE;



#define USING_OS_RTTHREAD

/****************************************************************************/
#ifdef __cplusplus
}
#endif
#endif  //ifndef _TYPE_H_
/****************************************************************************/
