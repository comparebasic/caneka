#include <external.h>
#include <caneka.h>

Cash *Cash_Make(MemCtx *m, Getter get, Abstract *source){
    Cash *o = (Cash*)MemCtx_Alloc(m, sizeof(Cash));
    o->type.of = TYPE_CASH;
    o->m = m;
    o->rbl = Cash_RblMake_rbl(m, (Abstract *)o, Cash_Capture);
    o->get = get;
    o->source = source;

    return o;
}
