#ifndef _GCODELIST_H
#define _GCODELIST_H

#include "CrInc/CrType.h"

struct GcodeItem_t
{
    S8  *Cmd;  //命令
    S32 (*ParseFunc)(const S8*, VOID*);       //解析函数接口, 前面的部分是命令字符串， 后面是结构体变量指针（它是输出）
    S32 (*ExecFunc)(VOID*, VOID*);      //执行函数接口(第一个参数是命令参数，第二个命令执行的回复结果缓冲区)
    S32 (*ReplyFunc)(VOID*, S8*);     //回复函数接口，在解析类中加标志处理(第一个参数是命令执行的回复结果缓冲区指针，第二个参数是组织好的回复信息缓冲区指针)
};

//extern struct GcodeItem_t *GcodeList;
extern const struct GcodeItem_t GcodeList[];

#endif
