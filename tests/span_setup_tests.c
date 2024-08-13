#include <external.h>
#include <caneka.h>
#include <testsuite.h>

static void _Debug(Span *p, char *msg){
    printf("%sP<%dx(idx:%d)>\n", msg, p->def->stride, p->def->idxStride);
}

status SpanSetup_Tests(MemCtx *gm){
    status r = READY;
    MemCtx *m = MemCtx_Make();

    Span *p;
    i64 used = 0;
    i64 delta = 0;
    p = Span_Make(m, TYPE_SPAN);
    r |= Test(p->def->stride == 16, "Span stride is 16 %d", p->def->stride);
    r |= Test(p->def->idxStride == 16, "Span idxStride is 16 %d", p->def->idxStride);
    r |= Test(p->def->valueSlab_Make == Span_valueSlab_Make, "Span has default valueSlab_Make");
    r |= Test(p->def->nextByIdx == Span_nextByIdx, "Span has default nextByIdx");
    used = MemCtx_Used(m);
    p->def->valueSlab_Make(m, p->def);
    delta = MemCtx_Used(m) - used;
    r |= Test(delta == 128, "Span slab has allocated 128 bytes, have %ld", delta);

    p = Span_Make(m, TYPE_QUEUE_SPAN);
    r |= Test(p->def->stride == 16, "Queue stride is 16 %d", p->def->stride);
    r |= Test(p->def->idxStride == 32, "Queue idxStride is 32 %d", p->def->idxStride);
    r |= Test(p->def->valueSlab_Make == Span_valueSlab_Make, "Queue has default valueSlab_Make");
    used = MemCtx_Used(m);
    p->def->valueSlab_Make(m, p->def);
    delta = MemCtx_Used(m) - used; 
    r |= Test(delta == 128, "Queue slab has allocated 128 bytes, have %ld", delta);

    p = Span_Make(m, TYPE_MINI_SPAN);
    r |= Test(p->def->stride == 4, "Mini stride is 4 %d", p->def->stride);
    r |= Test(p->def->idxStride == 16, "Mini idxStride is 16 %d", p->def->idxStride);
    r |= Test(p->def->valueSlab_Make == Span_valueSlab_Make, "Mini has default valueSlab_Make");
    used = MemCtx_Used(m);
    p->def->valueSlab_Make(m, p->def);
    delta = MemCtx_Used(m) - used;
    r |= Test(delta == 32, "Mini slab has allocated 32 bytes, have %ld", delta);

    p = Span_Make(m, TYPE_STRING_SPAN);
    r |= Test(p->def->stride == 30, "StringSpan stride is 30 %d", p->def->stride);
    r |= Test(p->def->idxStride == 16, "StringSpan idxStride is 16 %d", p->def->idxStride);
    r |= Test(p->def->valueSlab_Make != Span_valueSlab_Make, "StringSpan has custom valueSlab_Make");
    used = MemCtx_Used(m);
    p->def->valueSlab_Make(m, p->def);
    delta = MemCtx_Used(m) - used; 
    r |= Test(delta == 256, "String span has allocated 256 bytes, have %ld", delta);

    p = Span_Make(m, TYPE_SLAB_SPAN);
    r |= Test(p->def->stride == 512, "SlabSpan stride is 512 %d", p->def->stride);
    r |= Test(p->def->idxStride == 32, "SlabSpan idxStride is 16 %d", p->def->idxStride);
    r |= Test(p->def->valueSlab_Make == Span_valueSlab_Make, "SlabSpan has default valueSlab_Make");
    used = MemCtx_Used(m);
    p->def->valueSlab_Make(m, p->def);
    delta = MemCtx_Used(m) - used;
    r |= Test(delta == 4096, "SlabSpan span has allocated 4096 bytes, have %ld", delta);

    return r;
}
