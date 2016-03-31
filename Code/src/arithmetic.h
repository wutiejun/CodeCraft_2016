#include <stdlib.h>
#include <string.h>

#include "linklist.h"
#include "data.h"

#ifndef __ARITHMEIC_H__
#define __ARITHMEIC_H__

int arithmetic_dp(Topo * pTopo, struct list * pOutPath);

int arithmetic_cspf(Topo * pTopo, struct list * pOutPath);
void cspf_unload_topo(TOPO *topo);

#endif

