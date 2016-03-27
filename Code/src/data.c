/*
    for code craft. basic competion.
    初级比赛，动态规划的思想，使用记忆搜索就可以搞定了
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linklist.h"
#include "line_reader.h"
#include "data.h"
#include "arithmetic.h"

#define INPUT_FILE_TOPO         "../test-case/topo.csv"
#define INPUT_FILE_COMMAND      "../test-case/demand.csv"
#define OUTPUT_FILE_RESULT      "../test-case/result.csv"


/*
    将字符中按指定的字符分隔，会将指定的字符替换成\0
*/
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
        if (StrIndex >= MaxNum - 1)
        {
            break;
        }
        StrIndex ++;
    }
    return PtrIndex + 1;
}

typedef int (* split_callback)(char * pSubString, void * pUserData);

int string_foreach_split(char * pString, char Flag, split_callback call, void * pUserData)
{
    int StrIndex = 0;
    int PtrIndex = 0;
    char * pSubString = pString;
    
    if (pString == NULL || call == NULL)
    {
        return -1;
    }

    while (pString[StrIndex] != '\0')
    {
        if (pString[StrIndex] == Flag)
        {
            pString[StrIndex] = '\0';
            call(pSubString, pUserData);
            StrIndex ++;
            pSubString = &pString[StrIndex];
            continue;
        }
        StrIndex ++;
    }

    /* 如果最后还有一段数据，也回调一下 */
    if (pSubString != &pString[StrIndex])
    {
        call(pSubString, pUserData);
    }
    
    return 0;
}

Point * data_get_point_by_id(struct list * pAllPoints, int PointID, int AutoAdd)
{
    Point * pPoint = NULL;
    Point * pTempPoint = NULL;
    struct listnode *node = NULL;

    for (ALL_LIST_ELEMENTS_RO(pAllPoints, node, pTempPoint))
    {
        if (pTempPoint == NULL)
        {
            continue;
        }
        if (pTempPoint->PointID == PointID)
        {
            return pTempPoint;
        }
    }

    /* 原来的集合中没有找到点，又不自动添加，就返回NULL */
    if (AutoAdd == 0)
    {
        return NULL;
    }

    pPoint = malloc(sizeof(Point));
    if (pPoint == NULL)
    {
        //error
    }
    memset(pPoint, 0, sizeof(Point));
    pPoint->PointID = PointID;
    pPoint->TotalCost = -1;

    /* 添加到Topo的所有点集合中 */
    listnode_add(pAllPoints, pPoint);
    
    return pPoint;
}

/* LinkID,SourceID,DestinationID,Cost */
int data_read_edge(char Buffer[LINE_BUFFER_SIZE], int LineNum, void * pUserData)
{
    #define VALID_EDGE_SUB_STR_NUM  4   /* 有效的边信息子字符串个数 */
    Topo * pTopoInfo = (Topo *)pUserData;
    struct list * pAllEdges = &pTopoInfo->AllEdges;
    struct list * pAllPoint = &pTopoInfo->AllPoints;
    char * pSubString[8] = {0};
    int SubStringCount = 0;
    Edge * pEdge = NULL;
    Point * pFromPoint = NULL;
    Point * pToPoint = NULL;

    printf("Read edge line:%s", Buffer);
    
    SubStringCount = string_split(Buffer, ',', pSubString, 8);
    if (SubStringCount != VALID_EDGE_SUB_STR_NUM)
    {
        // ERROR
        return 0;
    }

    /* 直接转数字，如果有错误的，暂时不考虑，由输入保证 */
    pEdge = malloc(sizeof(Edge));
    if (pEdge == NULL)
    {
        return 0;
    }
    pEdge->LinkID = atoi(pSubString[0]);
    pEdge->SourceID = atoi(pSubString[1]);
    pEdge->DesID = atoi(pSubString[2]);
    pEdge->Cost = atoi(pSubString[3]);

    /* 将边添加到TOPO的边集中 */
    listnode_add(pAllEdges, pEdge);

    /* 先從所有的點中取一點 */
    pFromPoint = data_get_point_by_id(pAllPoint, pEdge->SourceID, TRUE);
    listnode_add(&pFromPoint->OutEdgeSet, pEdge);

    pToPoint = data_get_point_by_id(pAllPoint, pEdge->DesID, TRUE);
    listnode_add(&pToPoint->InEdgeSet, pEdge);

    return 0;
    
}

void debug_print_edge(struct list * pAllEdges, int ident)
{
    struct listnode *node = NULL;
    Edge *pEdge = NULL;
    int index = 0;
    
    for (ALL_LIST_ELEMENTS_RO(pAllEdges, node, pEdge))
    {
        index ++;
        if(NULL == pEdge)
        {
            continue;
        }
        if (ident) printf("    ");
        printf("[E:%d]ID:%d Source:%d Des:%d Cost:%d\n", index, pEdge->LinkID, pEdge->SourceID, 
                pEdge->DesID, pEdge->Cost);
    }
    return;
}

void debug_print_point(struct list * pAllPoint)
{
    struct listnode *node = NULL;
    Point *pPoint = NULL;
    int index = 0;
    
    for (ALL_LIST_ELEMENTS_RO(pAllPoint, node, pPoint))
    {
        index ++;
        if(NULL == pPoint)
        {
            continue;
        }
        printf("[P:%x]ID:%d TotalCost:%d\n", index, pPoint->PointID, pPoint->TotalCost);
        if (pPoint->pMiniCostEdge != NULL)
        {
            printf("    Min out edge:%d\n", pPoint->pMiniCostEdge->LinkID);  
        }
        printf("    In edges:\n");
        debug_print_edge(&pPoint->InEdgeSet, 1);
        printf("    Out edges:\n");        
        debug_print_edge(&pPoint->OutEdgeSet, 1);
    }
    return;
}

void debug_print_demand(Demand_Path * pDemand)
{
    struct listnode *node = NULL;
    Point *pPoint = NULL;
    int index = 0;

    printf("[Demand]Source:%d Des:%d\n", pDemand->SourceID, pDemand->DesID);
    printf("    include set:");
    for (ALL_LIST_ELEMENTS_RO(&pDemand->IncludeSet, node, pPoint))
    {
        index ++;
        if(NULL == pPoint)
        {
            continue;
        }
        printf("%d ", pPoint->PointID);
    }
    printf("\n");
    return;
}

void debug_print_topo(Topo * pTopoInfo)
{
    printf("\r\n");
    debug_print_edge(&pTopoInfo->AllEdges, 0);
    debug_print_point(&pTopoInfo->AllPoints);
    debug_print_demand(&pTopoInfo->Demand);
    printf("\r\n");
    return;
}

void data_free_for_tpo(void * pEdge)
{
    if (pEdge != NULL)
    {
        free(pEdge);
    }
    return;
}

void data_reset_total_cost(Topo * pTopo)
{
    struct listnode *node = NULL;
    Point *pPoint = NULL;
    
    for (ALL_LIST_ELEMENTS_RO(&pTopo->AllPoints, node, pPoint))
    {
        if(NULL == pPoint)
        {
            continue;
        }
        pPoint->pMiniCostEdge = NULL;
        pPoint->TotalCost = -1;
    }
    return;
}

int data_read_demand_include_reader(char * pSubString, void * pUserData)
{
    Topo * pTopoInfo = (Topo *)pUserData;
    Point * pPoint = NULL;
    int PointID = -1;

    //printf("read include set %s\n", pSubString);

    PointID = atoi(pSubString);
    pPoint = data_get_point_by_id(&pTopoInfo->AllPoints, PointID, 0);
    if (pPoint == NULL)
    {
        printf("<ERROR>include point %d not find in the point set.\n", PointID);
        return -1;
    }
     
    listnode_add(&pTopoInfo->Demand.IncludeSet, pPoint);
    return 0;     
}


int data_read_demand(char Buffer[LINE_BUFFER_SIZE], int LineNum, void * pUserData)
{
    #define VALID_DEMAND_SUB_STR_NUM  3   /* 有效的边信息子字符串个数 */

    Topo * pTopoInfo = (Topo *)pUserData;
    Demand_Path * pDemand = &pTopoInfo->Demand;
    char * pSubString[32] = {0};
    int SubStringCount = 0;
    
    printf("Read demand line:%s", Buffer);

    /* SourceID,DestinationID,IncludingSet */
    SubStringCount = string_split(Buffer, ',', pSubString, 8);
    if (SubStringCount != VALID_DEMAND_SUB_STR_NUM)
    {
        // ERROR
        return 0;
    }

    pDemand->SourceID = atoi(pSubString[0]);
    pDemand->DesID = atoi(pSubString[1]);

    //printf("read include set:%s\n", pSubString[2]);
    string_foreach_split(pSubString[2], '|', data_read_demand_include_reader, pTopoInfo);
    
    return 0;    
    
}

int data_load_topo(Topo * pTopoInfo, const char * pTopoFile, const char * pDemandFile)
{
    int ret = 0;

    /* 只有TOPO的链表上挂释放函数，其它的都只是指针，不释放 */
    pTopoInfo->AllEdges.del = data_free_for_tpo;
    pTopoInfo->AllPoints.del = data_free_for_tpo;
    ret = line_reader_read(pTopoFile, data_read_edge, pTopoInfo, NULL);
    if (ret != 0)
    {
        // log error
    }

    printf("\n");

    ret = line_reader_read(pDemandFile, data_read_demand, pTopoInfo, NULL);
    if (ret != 0)
    {
        // log error
    }
    
    return 0;
}


int main(int argc, char * argv[])
{
    Topo topo = {0};
    struct list OutPath = {0};
    
    data_load_topo(&topo, INPUT_FILE_TOPO, INPUT_FILE_COMMAND);
    arithmetic_dp(&topo, &OutPath);
    debug_print_topo(&topo);
    return 0;
}
