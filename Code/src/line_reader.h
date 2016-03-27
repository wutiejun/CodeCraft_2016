/*
    基于文件的行读取机制
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef __LINE_READER__
#define __LINE_READER__

typedef int (*LINE_READER_CALLBACK)(char Buffer[256], int LineNumber, void * pUserData);

#define INVALID_LINE        0
#define VALID_LINE          1
#define LINE_BUFFER_SIZE    256
#define EXIT_READ           1

#ifndef PUBLIC
#define PUBLIC
#endif

#ifndef PRIVATE
#define PRIVATE
#endif

/* 
    从文件中一行一行的读取内容，并回调pCallBack
    @pFilePath  文件路径
    @pCallBack  从文件中读取到一行有效数据时的回调函数
    @pUserData  用户自己定义的参数，直接传给回调函数
    @Falg       扩展标记，用于标记什么样的行是无效的，默认空行和以#开头的行是无效的
    &return     返回值，如果成功返回0，否则返回非0
*/
PUBLIC int line_reader_read(const char * pFilePath, LINE_READER_CALLBACK pCallBack, void * pUserData, int Falg);

#endif
