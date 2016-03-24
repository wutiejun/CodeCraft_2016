/*
    for code craft. basic competion.
    ������������̬�滮��˼�룬ʹ�ü��������Ϳ��Ը㶨��
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linklist.h"
#include "line_reader.h"

#define INPUT_FILE_TOPO         "topo.csv"
#define INPUT_FILE_COMMAND      "demand.csv"

/*
    ����ıߵ���Ϣ
    LinkID,SourceID,DestinationID,Cost
*/
typedef struct Edge_
{
    int LinkID;
    int SourceID;
    int DesID;
    int Cost;
} Edge;

/*
    �����������Ϣ
    SourceID,DestinationID,IncludingSet
*/
typedef struct Demand_Path_
{
    int SourceID;
    int DesID;
    struct list IncludeSet;
} Demand_Path;

/*
    ��ȡ�����ĵ���Ϣ�����ڼ��㴦��
*/
typedef struct Point_
{
    int PointID;
    struct list InEdgeSet;
    struct list OutEdgeSet;
    
    /* �ڵ㶯̬���ݣ������㷨��ʵ�� */
    int TotalCost;

} Point;

/*
    ��ȡ������ͼ��Topo�ṹ
*/
typedef struct Topo_
{
    struct list AllPoints;
    struct list AllEdges;
} Topo;

/*
    ���ַ��а�ָ�����ַ��ָ����Ὣָ�����ַ��滻��\0
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

Point * data_get_pint_by_id(struct list * pAllPoints, int PointID)
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

    pPoint = malloc(sizeof(Point));
    if (pPoint == NULL)
    {
        //error
    }
    memset(pPoint, 0, sizeof(Point));
    pPoint->PointID = PointID;
    pPoint->TotalCost = -1;         

    /* ��ӵ�Topo�����е㼯���� */
    listnode_add(pAllPoints, pPoint);
    
    return pPoint;
}

/* LinkID,SourceID,DestinationID,Cost */
int data_read_edge(char Buffer[LINE_BUFFER_SIZE], int LineNum, void * pUserData)
{
    #define VALID_EDGE_SUB_STR_NUM  4   /* ��Ч�ı���Ϣ���ַ������� */
    Topo * pTopoInfo = (Topo *)pUserData;
    struct list * pAllEdges = &pTopoInfo->AllEdges;
    struct list * pAllPoint = &pTopoInfo->AllPoints;
    char * pSubString[8] = {0};
    int SubStringCount = 0;
    Edge * pOutEdge = NULL;
    Point * pFromPoint = NULL;
    Point * pToPoint = NULL;
    
    SubStringCount = string_split(Buffer, ',', pSubString, 8);
    if (SubStringCount != VALID_EDGE_SUB_STR_NUM)
    {
        // ERROR
        return 0;
    }

    /* ֱ��ת���֣�����д���ģ���ʱ�����ǣ������뱣֤ */
    pOutEdge = malloc(sizeof(Edge));
    if (pOutEdge == NULL)
    {
        return 0;
    }
    pOutEdge->LinkID = atoi(pSubString[0]);
    pOutEdge->SourceID = atoi(pSubString[1]);
    pOutEdge->DesID = atoi(pSubString[2]);
    pOutEdge->Cost = atoi(pSubString[3]);

    /* ������ӵ�TOPO�ı߼��� */
    listnode_add(pAllEdges, pOutEdge);

    pFromPoint = data_get_pint_by_id();

    return 0;
    
}

void debug_print_edge(struct list * pAllEdges)
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
        printf("[%d]%d %d %d %d\n", index, pEdge->LinkID, pEdge->SourceID, 
                pEdge->DesID, pEdge->Cost);
    }
    return;
}

void debug_print_topo(Topo * pTopoInfo)
{
    debug_print_edge(&pTopoInfo->AllEdges);
    return;
}

void data_free_edge_for_tpo(void * pEdge)
{
    if (pEdge != NULL)
    {
        free(pEdge);
    }
    return;
}

int load_edge(Topo * pTopoInfo)
{
    int ret = 0;

    /* ֻ��TOPO�������Ϲ��ͷź����������Ķ�ֻ��ָ�룬���ͷ� */
    pTopoInfo->AllEdges.del = data_free_edge_for_tpo;
    ret = line_reader_read(INPUT_FILE_TOPO, data_read_edge, pTopoInfo, NULL);
    if (ret != 0)
    {
        // log error
    }
    return 0;
}

int main(int argc, char * argv[])
{
    Topo topo = {0};
    load_edge(&topo);
    debug_print_topo(&topo);
    return 0;
}
