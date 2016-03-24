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

Point * data_get_point_by_id(struct list * pAllPoints, int PointID)
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
    Edge * pEdge = NULL;
    Point * pFromPoint = NULL;
    Point * pToPoint = NULL;

    printf("Read line:%s", Buffer);
    
    SubStringCount = string_split(Buffer, ',', pSubString, 8);
    if (SubStringCount != VALID_EDGE_SUB_STR_NUM)
    {
        // ERROR
        return 0;
    }

    /* ֱ��ת���֣�����д���ģ���ʱ�����ǣ������뱣֤ */
    pEdge = malloc(sizeof(Edge));
    if (pEdge == NULL)
    {
        return 0;
    }
    pEdge->LinkID = atoi(pSubString[0]);
    pEdge->SourceID = atoi(pSubString[1]);
    pEdge->DesID = atoi(pSubString[2]);
    pEdge->Cost = atoi(pSubString[3]);

    /* ������ӵ�TOPO�ı߼��� */
    listnode_add(pAllEdges, pEdge);

    /* �ȏ����е��c��ȡһ�c */
    pFromPoint = data_get_point_by_id(pAllPoint, pEdge->SourceID);
    listnode_add(&pFromPoint->OutEdgeSet, pEdge);

    pToPoint = data_get_point_by_id(pAllPoint, pEdge->DesID);
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
        printf("[E:%d]ID:%d S:%d D:%d C:%d\n", index, pEdge->LinkID, pEdge->SourceID, 
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
        printf("[P:%d]ID:%d C:%d\n", pPoint, pPoint->PointID, pPoint->TotalCost);
        printf("    In edges:\n");
        debug_print_edge(&pPoint->InEdgeSet, 1);
        printf("    Out edges:\n");        
        debug_print_edge(&pPoint->OutEdgeSet, 1);
    }
    return;
}

void debug_print_topo(Topo * pTopoInfo)
{
    print("\r\n");
    debug_print_edge(&pTopoInfo->AllEdges, 0);
    debug_print_point(&pTopoInfo->AllPoints);
    print("\r\n");
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
