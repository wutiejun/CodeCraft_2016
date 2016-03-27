/*
    �����ļ����ж�ȡ����
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
    ���ļ���һ��һ�еĶ�ȡ���ݣ����ص�pCallBack
    @pFilePath  �ļ�·��
    @pCallBack  ���ļ��ж�ȡ��һ����Ч����ʱ�Ļص�����
    @pUserData  �û��Լ�����Ĳ�����ֱ�Ӵ����ص�����
    @Falg       ��չ��ǣ����ڱ��ʲô����������Ч�ģ�Ĭ�Ͽ��к���#��ͷ��������Ч��
    &return     ����ֵ������ɹ�����0�����򷵻ط�0
*/
PUBLIC int line_reader_read(const char * pFilePath, LINE_READER_CALLBACK pCallBack, void * pUserData, int Falg);

#endif
