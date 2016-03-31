#include <base.h>
#include <linklist.h>
#include <memory.h>
#include "memtypes.h"
#include "vector.h"
#include "sdn.h"
#include "file.h"
#include "debug.h"
#include "arithmetic.h"


void cspf_unload_topo(TOPO *topo)
{
    int i = 0;

    if (!topo)
    {
        return 0;
    }

    for (i = 0; i < topo->router_list->active; i++)
    {
        ROUTER *r = zebra_vector_lookup_ensure(topo->router_list, i);
        if (!r)
        {
            continue;
        }
        router_free(topo, r);
    }

    for (i = 0; i < topo->link_record_list->active; i++)
    {
        LINK_RECORD *l = zebra_vector_lookup_ensure(topo->link_record_list, i);
        if (!l)
        {
            continue;
        }
        link_free(topo, l);
    }

    zebra_vector_free(topo->router_list);
    zebra_vector_free(topo->link_record_list);
    XFREE(MTYPE_TOPO, topo);
    return 0;
}


TOPO * cspf_load_topo(vector pAllPoint, vector pAllEdge)
{
    int eindex;
    int vindex;
    TOPO *topo = XCALLOC(MTYPE_TOPO, sizeof(TOPO));
    
    if (!topo)
    {
        return NULL;
    }
    topo->router_list = zebra_vector_init(256);
    topo->link_record_list = zebra_vector_init(256);
    
    for (vindex = 0; vindex < pAllPoint->alloced; vindex++)
    {
        Point *nodeid = zebra_vector_lookup_ensure(pAllPoint, vindex);
        if (nodeid == NULL)
        {
            continue;
        }
        
        ROUTER *r = router_get(topo, nodeid->PointID);
        if (!r)
        {
            goto out;
        }
        r->delay = nodeid->TotalCost;
    }
    
    for (eindex = 0; eindex < pAllEdge->alloced; eindex++)
    {
        ROUTER *from;
        ROUTER *to;
        LINK_RECORD *link;
        Edge *edge = zebra_vector_lookup_ensure(pAllEdge, eindex);
        if (edge == NULL)
        {
            continue;
        }

        from = router_get(topo, edge->SourceID);
        if (!from)
        {
            goto out;
        }
        to = router_get(topo, edge->DesID);
        if (!to)
        {
            goto out;
        }
        link = link_new(topo, edge);
        if (!link)
        {
            goto out;
        }
        link->from = from;
        link->to = to;
        listnode_add(from->links, link);
    }

out:
    return topo;
}

/*
    基于zebra的优先级队列算法
*/
int arithmetic_cspf(Topo * pTopo, struct list * pOutPath)
{
    TOPO * pCspfTopo = cspf_load_topo(pTopo->AllPoints, pTopo->AllEdges);
    cspf_unload_topo(pCspfTopo);
    return 0;
}
