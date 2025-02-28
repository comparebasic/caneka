#include <external.h>
#include <caneka.h>

OrdTable *OrdTable_Make(MemCtx *m){
    OrdTable *p = MemCtx_Alloc(m, sizeof(OrdTable));
    p->m = m;
    p->def = SpanDef_FromCls(TYPE_TABLE);
    p->type.of = p->def->typeOf = TYPE_ORDERED_TABLE;
    p->root = Span_valueSlab_Make(m, p->def);
    p->max_idx = -1;
    p->order = Span_Make(m, TYPE_SPAN);
    return p;
}
