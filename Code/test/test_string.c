#include "stdio.h"
#include "string.h"

int test_string_001(void)
{
    int ret = 0;
    int index = 0;
    char * pSubString[32] = {0};
    char TestBuffer[] = "0,0,1,1";

    ret = string_split(TestBuffer, ',', pSubString, 32);
    for (index = 0 ; index < ret; index ++)
    {
        printf("[%d/%d]%s;\r\n", index, ret, pSubString[index]);
    }
    
    return 0;
}
