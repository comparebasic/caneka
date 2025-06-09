#include <external.h>
#include <caneka.h>

OrdTable *OrdTable_Make(MemCtx *m){
    OrdTable *p = MemCtx_Alloc(m, sizeof(OrdTable));
    p->m = m;
    p->type.of = TYPE_ORDERED_TABLE;
    p->root = Span_Slab_Make(m);
    p->max_idx = -1;
    p->order = Span_Make(m);
    return p;
}
