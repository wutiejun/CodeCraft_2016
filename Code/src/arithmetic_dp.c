
#include "arithmetic.h"

/* �Ƚ�ÿ���ڵ㵽�յ����С����������� */
void arithmetic_dp_cal_total_cost(Point * pCurrentPoint, vector pAllPoints)
{
    struct listnode *node = NULL;
    Edge *pEdge = NULL;
    Point * pSourcePoint = NULL;
    int TotalCost = 0;
    
    for (ALL_LIST_ELEMENTS_RO(&pCurrentPoint->InEdgeSet, node, pEdge))
    {
        pSourcePoint = data_get_point_by_id_ex(pAllPoints, pEdge->SourceID, FALSE);
        if (pSourcePoint == NULL)
        {
            printf("Error source point not in the global points set.\n");
            continue;
        }

        TotalCost = pCurrentPoint->TotalCost + pEdge->Cost;
        if (pSourcePoint->TotalCost == -1)
        {
            pSourcePoint->TotalCost = TotalCost;
            pSourcePoint->pMiniCostEdge = pEdge;
            arithmetic_dp_cal_total_cost(pSourcePoint, pAllPoints);
        }
        else if (pSourcePoint->TotalCost > TotalCost)
        {
            pSourcePoint->TotalCost = TotalCost;
            pSourcePoint->pMiniCostEdge = pEdge;
        }        
    }

    return;
}


int arithmetic_dp_build_out_path(Point * pStart, Point * pEnd, struct list * pOutPath)
{
    
}

/*
    ��̬�滮����������
*/
int arithmetic_dp(Topo * pTopo, struct list * pOutPath)
{
    Demand_Path * pDemand = &pTopo->Demand;

    Point * pEndPoint = NULL;

    pEndPoint = data_get_point_by_id_ex(pTopo->AllPoints, pDemand->DesID, FALSE);
    if (pEndPoint == NULL)
    {
        printf("Error get end point %d for global points set.\n", pDemand->DesID);
        return -1;
    }

    pEndPoint->TotalCost = 0;
    arithmetic_dp_cal_total_cost(pEndPoint, pTopo->AllPoints);
    
    return 0;
}
