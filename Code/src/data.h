
#include "linklist.h"
#include "vector.h"

#ifndef __CODE_CRAFT_DATA__
#define __CODE_CRAFT_DATA__

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/*
    输入的边的信息
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
    输入的命令信息
    SourceID,DestinationID,IncludingSet
*/
typedef struct Demand_Path_
{
    int SourceID;
    int DesID;
    struct list IncludeSet;
} Demand_Path;

/*
    提取出来的点信息，用于计算处理
*/
typedef struct Point_
{
    int PointID;
    struct list InEdgeSet;
    struct list OutEdgeSet;
    
    /* 节点动态数据，方便算法的实现 */
    int TotalCost;
    Edge * pMiniCostEdge;
} Point;

/*
    提取出来的图的Topo结构，将list修改成vector
*/
typedef struct Topo_
{
    
    //struct list AllPoints;
    //struct list AllEdges;
    vector AllPoints;
    vector AllEdges;
    Demand_Path Demand;
} Topo;


/* 重新设置Topo数据，重新计算路径 */
void data_reset_total_cost(Topo * pTopo);

int data_load_topo(Topo * pTopoInfo, const char * pTopoFile, const char * pDemandFile);

void data_free_topo(Topo * pTopoInfo);


#endif

