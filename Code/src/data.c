#include "stdio.h"
#include "string.h"

#define INPUT_FILE_TOPO "topo.csv"
#define INPUT_FILE_COMMAND "0,1,2|3"

//LinkID,SourceID,DestinationID,Cost
typedef struct Edge_
{
    int LinkID;
    int SourceID;
    int DesID;
    int Cost;
} Edge;

//SourceID,DestinationID,IncludingSet
typedef struct Path_
{
    int SourceID;
    int DesID;
    
} Path;

int string_split(char * pString, char Flag, char * pSubString[32], int MaxNum)
{
    int StrIndex = 0;
    int PtrIndex = 0;
    
    if (pString == NULL || pSubString == NULL || MaxNum == 0)
    {
        return -1;
    }

    pSubString[PtrIndex] = pString;
    while (pString[StrIndex] != '\0')
    {    
        if (pString[StrIndex] == Flag)
        {
            PtrIndex ++;
            pString[StrIndex] = '\0';
            pSubString[PtrIndex] = &pString[StrIndex + 1];
        }
        StrIndex ++;
    }
    return PtrIndex + 1;
}

int main(int argc, char * argv[])
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
