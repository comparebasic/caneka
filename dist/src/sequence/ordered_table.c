#include <external.h>
#include <caneka.h>

OrderedTable *OrderedTable_Make(MemCtx *m){
    OrderedTable *p = MemCtx_Alloc(m, sizeof(OrderedTable));
    p->m = m;
    p->def = SpanDef_FromCls(TYPE_TABLE);
    p->type.of = p->def->typeOf = TYPE_ORDERED_TABLE;
    p->root = Span_valueSlab_Make(m, p->def);
    p->max_idx = -1;
    p->order = Span_Make(m, TYPE_SPAN);
    return p;
}
