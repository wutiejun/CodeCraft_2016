
#ifndef __CODE_CRAFT_DATA__
#define __CODE_CRAFT_DATA__

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

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
    Edge * pMiniCostEdge;
} Point;

/*
    ��ȡ������ͼ��Topo�ṹ
*/
typedef struct Topo_
{
    struct list AllPoints;
    struct list AllEdges;
    Demand_Path Demand;
} Topo;


/* ��������Topo���ݣ����¼���·�� */
void data_reset_total_cost(Topo * pTopo);

#endif

