
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
    Demand_Path Demand;
} Topo;


