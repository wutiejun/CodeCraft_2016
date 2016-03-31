/* 调试工具文件 */

#include "stdio.h"

#include "debug.h"


/* 调试打印函数 */
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

void debug_print_edge_ex(vector pAllEdges, int ident)
{
    Edge *pEdge = NULL;
    int index = 0;
    
    for (index = 0; index < pAllEdges->alloced; index ++)
    {
        pEdge = zebra_vector_lookup(pAllEdges, index);
        if (pEdge == NULL)
        {
            continue;
        }  
        if (ident) printf("    ");
        printf("[E:%d]ID:%d Source:%d Des:%d Cost:%d\n", index, pEdge->LinkID, pEdge->SourceID, 
                pEdge->DesID, pEdge->Cost);        
    }
    return;
}

#if 0
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
#endif

void debug_print_point_ex(vector pAllPoint)
{
    Point *pPoint = NULL;
    int index = 0;
    
    for (index = 0; index < pAllPoint->alloced; index ++)
    {
        pPoint = (Point *)zebra_vector_lookup(pAllPoint, index);
        if (pPoint == NULL)
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
    debug_print_edge_ex(pTopoInfo->AllEdges, 0);
    debug_print_point_ex(pTopoInfo->AllPoints);
    debug_print_demand(&pTopoInfo->Demand);
    printf("\r\n");
    return;
}



