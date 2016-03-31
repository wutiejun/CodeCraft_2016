#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data.h"
#include "line_reader.h"
#include "arithmetic.h"
#include "debug.h"


#define INPUT_FILE_TOPO         "../test-case/topo.csv"
#define INPUT_FILE_COMMAND      "../test-case/demand.csv"
#define OUTPUT_FILE_RESULT      "../test-case/result.csv"

int main(int argc, char * argv[])
{
    Topo topo = {0};
    struct list OutPath = {0};
    
    data_load_topo(&topo, INPUT_FILE_TOPO, INPUT_FILE_COMMAND);
    arithmetic_dp(&topo, &OutPath);
    debug_print_topo(&topo);

    arithmetic_cspf(&topo, &OutPath);
    
    data_free_topo(&topo);
    return 0;
}

