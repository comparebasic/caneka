#include <external.h>
#include <caneka.h>

status MemCtx_Tests(MemCtx *gm){
    status r = READY;
    MemCtx *m = MemCtx_Make();

    String *s = NULL;

    int stackOffset = m->type.range;
    printf("making string 1\n");
    s = String_Init(m, STRING_EXTEND);

    m->type.range++;
    s = String_Init(m, STRING_EXTEND);
    m->type.range--;

    i64 allCount = MemCtx_MemCount(m, 0);
    i64 stackedCount = MemCtx_MemCount(m, stackOffset+1);
    r |= Test(allCount != stackedCount, "All count and Stack count differ");
    r |= Test(allCount - stackedCount == sizeof(String), "Stack count has one String size of difference from all count, have %ld", allCount - stackedCount);
    i64 total = MemCtx_Total(m, 0);
    r |= Test(total == MEM_SLAB_SIZE*2, "Total is the size of two mem slabs, have %ld", total);

    MemCtx_FreeTemp(m, stackOffset+1);
    total = MemCtx_Total(m, 0);
    r |= Test(total == MEM_SLAB_SIZE*1, "Total is the size of one mem slabs after free temp, have %ld", total);

    MemCtx_Free(m);
    return r;
}
