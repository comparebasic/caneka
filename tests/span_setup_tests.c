#include <external.h>
#include <caneka.h>
#include <testsuite.h>

status SpanSetup_Tests(MemCtx *gm){
    status r = READY;
    MemCtx *m = MemCtx_Make();

    Span *p;
    p = Span_Make(m, TYPE_SPAN);
    printf("span %d\n", p->def->stride);

    p = Span_Make(m, TYPE_QUEUE_SPAN);
    printf("queue %d\n", p->def->stride);

    p = Span_Make(m, TYPE_MINI_SPAN);
    printf("mini %d\n", p->def->stride);

    p = Span_Make(m, TYPE_STRING_SPAN);
    printf("string %d\n", p->def->stride);

    p = Span_Make(m, TYPE_SLAB_SPAN);
    printf("slab %d\n", p->def->stride);

    return r;
}
