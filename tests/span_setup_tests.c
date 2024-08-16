#include <external.h>
#include <caneka.h>
#include <testsuite.h>

static void _Debug(Span *p, char *msg){
    printf("%sP<%dx(idx:%d)>\n", msg, p->def->stride, p->def->idxStride);
}

static status testSpan(Span *p, char *name, int dimFor100, int dimFor2000, int stride, int idxStride, int slabSz, int idxSlabSz){
    MemCtx *m = p->m;
    status r = READY;
    i64 used = 0;
    i64 delta = 0;
    int idx = 0;
    SlabResult sr;
    Single *w;

    r |= Test(p->def->stride == stride, "%s stride is 16, have %d", name, p->def->stride);
    r |= Test(p->def->idxStride == idxStride, "%s idxStride is 16, have %d", name, p->def->idxStride);
    used = MemCtx_Used(m);
    Span_valueSlab_Make(m, p->def);
    delta = MemCtx_Used(m) - used;
    r |= Test(delta == slabSz, "%s slab has allocated %d bytes, have %ld", name, slabSz, delta);
    used += delta;
    Span_idxSlab_Make(m, p->def);
    delta = MemCtx_Used(m) - used;
    r |= Test(delta == idxSlabSz, "%s idxSlab has allocated %d bytes, have %ld", name, idxSlabSz, delta);

    idx = 100;

    SlabResult_Setup(&sr, p, SPAN_OP_SET, idx);
    r |= Test(sr.dimsNeeded == dimFor100, "%s needs %d dims to index into %d, have %hu",
        name, dimFor100, idx, sr.dimsNeeded);

    w = Int_Wrapped(m, idx);
    Span_Set(p, idx, (Abstract *)w);

    idx = 2000;
    SlabResult_Setup(&sr, p, SPAN_OP_SET, idx);
    r |= Test(sr.dimsNeeded == dimFor2000, "%s only needs %d dim to index into %d, have %hu",
        name, dimFor2000, idx, sr.dimsNeeded);
    Span_Set(p, idx, (Abstract *)w);

    w = Int_Wrapped(m, idx);

    if(DEBUG_SPAN){
        Span_Print((Abstract *)p, p->type.of, "Tested Span ", DEBUG_SPAN, TRUE);
        printf("\n");
    }

    return r;
}

status SpanSetup_Tests(MemCtx *gm){
    status r = READY;
    MemCtx *m = MemCtx_Make();

    Span *p;
    i64 used = 0;
    i64 delta = 0;
    int idx = 0;
    int dims = 0;
    SlabResult sr;

    /* span */
    p = Span_Make(m, TYPE_SPAN);
    testSpan(p, "Span", 1, 2, 16, 16, 128, 128);

    /* queue */
    p = Span_Make(m, TYPE_QUEUE_SPAN);
    testSpan(p, "Queue", 1, 2, 16, 32, 128, 512);

    /* mini */
    p = Span_Make(m, TYPE_MINI_SPAN);
    testSpan(p, "Mini", 2, 3, 4, 16, 32, 128);

    /* string span */
    p = Span_Make(m, TYPE_STRING_SPAN);
    testSpan(p, "StringSpan", 1, 2, STRING_CHUNK_SIZE / sizeof(void *), 16, 240, 128);

    /* slab span */
    p = Span_Make(m, TYPE_SLAB_SPAN);
    testSpan(p, "SlabSpan", 0, 1, 512, 32, 4096, 512);

    return r;
}
