#include <external.h>
#include <caneka.h>
#include "../module.h"

status ProcDets_Init(ProcDets *pd){
    memset(pd, 0, sizeof(ProcDets));
    pd->type.of = TYPE_PROCDETS;
    pd->code = -1;
    return SUCCESS;
}
