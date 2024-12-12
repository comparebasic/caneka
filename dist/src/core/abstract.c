#include <external.h>
#include <caneka.h>

status MHAbstract_Init(MHAbstract *ma, MemCtx *m, Abstract *a){
    memset(ma, 0, sizeof(MHAbstract));
    ma->type.of = TYPE_MHABSTRACT;
    ma->m = m;
    ma->a = a;

    return SUCCESS;
}
