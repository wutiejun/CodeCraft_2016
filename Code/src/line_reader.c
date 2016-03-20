#include <stdio.h>
#include "stdlib.h"
#include "string.h"

// FILE *fopen(const char *path, const char *mode);
// char *fgets(char *s, int size, FILE *stream);

typedef int (*LINE_READER_CALLBACK)(char Buffer[256], int LineNumber, void * pUserData);

#define INVALID_LINE        0
#define VALID_LINE          1
#define LINE_BUFFER_SIZE    256
#define EXIT_READ           1

#define PUBLIC
#define PRIVATE

/* ��⵱ǰ���Ƿ�����Ч�����룬�ǿ��У���һ������'#' */
PRIVATE int CheckLine(char Buffer[256])
{
    int Strlen = 0;
    if (Buffer == NULL)
    {
        return INVALID_LINE;
    }
    
    if (Buffer[0] == '#')
    {
        return INVALID_LINE;
    }

    Strlen = strlen(Buffer);
    if (Strlen <= 0)
    {
        return INVALID_LINE;
    }
	return VALID_LINE;
}

/* ����API���ṩ�ļ����ж�ȡ���� */
PUBLIC int ReadFile(const char * pFilePath, LINE_READER_CALLBACK pCallBack, void * pUserData, int Falg)
{
	char Buffer[LINE_BUFFER_SIZE] = {0};	
   	FILE * pFile = NULL;
    char * ReadRet = 0;	
	int LineNumber = 0;
    int Ret = 0;

    if (pCallBack == NULL)
    {
        return 0;
    }

    /* ��ֻ����ʽ���ļ�������ļ������ڣ����ش��� */
    pFile = fopen(pFilePath, "r");
    if (pFile == NULL)
    {
        return 0;
    }

    ReadRet = fgets(pFile, Buffer, LINE_BUFFER_SIZE - 1);
	while (ReadRet != EOF)
	{
		LineNumber ++;
		if (CheckLine(Buffer) == INVALID_LINE)
		{
			continue;
		}
        Ret = pCallBack(Buffer, LineNumber,pUserData);
        if (Ret == EXIT_READ)
        {
            break;
        }

        memset(Buffer, 0, LINE_BUFFER_SIZE);
        ReadRet = fgets(pFile, Buffer, LINE_BUFFER_SIZE - 1);
	};

    close(pFile);
	return Ret;
}


