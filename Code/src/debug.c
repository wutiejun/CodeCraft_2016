/* 调试工具文件 */

#include "stdio.h"
#include "args.h"

void Log(const char file, int line, ...)
{
    printf("%s<%d>\r\n");
    return;
}

