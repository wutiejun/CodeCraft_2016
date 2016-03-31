/*
    for code craft. basic competion.
    ������������̬�滮��˼�룬ʹ�ü��������Ϳ��Ը㶨��
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data.h"
#include "line_reader.h"
#include "arithmetic.h"


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

typedef int (* split_callback)(char * pSubString, void * pUserData);

/* ��һ���ַ�����ָ���ַ��ָ���ÿ���ָ��ص�һ�»ص����� */
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

    /* ��������һ�����ݣ�Ҳ�ص�һ�� */
    if (pSubString != &pString[StrIndex])
    {
        call(pSubString, pUserData);
    }
    
    return 0;
}

/* ͨ�����ID����ȡ������ݣ����AutoAddΪTrueʱ����ʾ�Զ����һ���㵽ȫ�ֵĵ��� */
Point * data_get_point_by_id_ex(vector pAllPoints, int PointID, int AutoAdd)
{
    Point* pPoint = (Point *)zebra_vector_lookup_ensure(pAllPoints, PointID);
    if ((pPoint == NULL) && (AutoAdd == TRUE))
    {
        pPoint = malloc(sizeof(Point));
        if (pPoint == NULL)
        {
            //error
            return NULL;
        }
        memset(pPoint, 0, sizeof(Point));
        pPoint->PointID = PointID;
        pPoint->TotalCost = -1;
        zebra_vector_set_index(pAllPoints, PointID, pPoint);
    }
    return pPoint;
}

#if 0
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

    /* ԭ���ļ�����û���ҵ��㣬�ֲ��Զ���ӣ��ͷ���NULL */
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

    /* ��ӵ�Topo�����е㼯���� */
    listnode_add(pAllPoints, pPoint);
    
    return pPoint;
}
#endif

/* 
    LinkID,SourceID,DestinationID,Cost 
    ������ĸ�ʽ��ȡ�ߵ���Ϣ
*/
int data_read_edge(char Buffer[LINE_BUFFER_SIZE], int LineNum, void * pUserData)
{
    #define VALID_EDGE_SUB_STR_NUM  4   /* ��Ч�ı���Ϣ���ַ������� */

    Topo * pTopoInfo = (Topo *)pUserData;
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

    /* ֱ��ת���֣�����д���ģ���ʱ�����ǣ������뱣֤ */
    pEdge = malloc(sizeof(Edge));
    if (pEdge == NULL)
    {
        // ERROR
        return 0;
    }
    
    pEdge->LinkID = atoi(pSubString[0]);
    pEdge->SourceID = atoi(pSubString[1]);
    pEdge->DesID = atoi(pSubString[2]);
    pEdge->Cost = atoi(pSubString[3]);

    /* ������ӵ�TOPO�ı߼��� */
    zebra_vector_set_index(pTopoInfo->AllEdges, pEdge->LinkID, pEdge);

    /* �ȏ����е��c��ȡһ�c */
    pFromPoint = data_get_point_by_id_ex(pTopoInfo->AllPoints, pEdge->SourceID, TRUE);
    listnode_add(&pFromPoint->OutEdgeSet, pEdge);

    pToPoint = data_get_point_by_id_ex(pTopoInfo->AllPoints, pEdge->DesID, TRUE);
    listnode_add(&pToPoint->InEdgeSet, pEdge);

    return 0;
    
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
    //struct listnode *node = NULL;
    Point *pPoint = NULL;
    int index;
    //for (ALL_LIST_ELEMENTS_RO(&pTopo->AllPoints, node, pPoint))

    for (index = 0; index < pTopo->AllPoints->alloced; index ++)
    {
        pPoint = zebra_vector_lookup(pTopo->AllPoints ,index);
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
    pPoint = data_get_point_by_id_ex(pTopoInfo->AllPoints, PointID, FALSE);
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
    #define VALID_DEMAND_SUB_STR_NUM  3   /* ��Ч�ı���Ϣ���ַ������� */

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

    pTopoInfo->AllEdges = zebra_vector_init(256);
    pTopoInfo->AllPoints = zebra_vector_init(256);
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

typedef void(* vector_free_callback)(void * pData);

void data_free_vector(vector v, vector_free_callback free_callback)
{
    int path_i = 0;
    for (path_i = 0; path_i < v->active; path_i++)
    {
        void *path = zebra_vector_lookup(v, path_i);
        if (path)
        {
            free_callback(path);
            zebra_vector_unset(v, path_i);
        }
    }
    zebra_vector_free(v);
    return;
}

inline void data_free_point(void * pData)
{
    Point * pPoint = (Point *)pData;
    if (pPoint->InEdgeSet.count > 0)
    {
        list_delete_all_node(&pPoint->InEdgeSet);
    }
    if (pPoint->OutEdgeSet.count > 0)
    {
        list_delete_all_node(&pPoint->OutEdgeSet); 
    }
    return;
}

inline void data_free_edge(void * pData)
{
    free(pData);
    return;
}

void data_free_topo(Topo * pTopoInfo)
{
    data_free_vector(pTopoInfo->AllEdges, data_free_edge);
    data_free_vector(pTopoInfo->AllPoints, data_free_point);
    list_delete_all_node(&pTopoInfo->Demand.IncludeSet);
    return;
}


