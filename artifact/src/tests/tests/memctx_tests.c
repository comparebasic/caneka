#include <external.h>
#include <caneka.h>

status MemCtx_Tests(MemCtx *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;

    MemCtx *m = MemCtx_Make();
    MemChapter *cp = NULL;
#ifdef INSECURE
    cp = MemChapter_Get(m);
#endif
    i32 count = 0;
    if(cp != NULL){
        count = cp->pages.nvalues;
        r |= Test(count > 0, "Chapter slab count is greater than 0, have %d.", count);
    }

    i64 max = 1024;
    for(i64 i = 0; i < max; i++){
        i64 *p = MemCtx_Alloc(m, sizeof(i64));
        memcpy(p, &i, sizeof(i64));
    }

    m->type.range++;
    for(i64 i = 0; i < max; i++){
        i64 *p = MemCtx_Alloc(m, sizeof(i64));
        memcpy(p, &i, sizeof(i64));
    }

    r |= Test(m->p.nvalues == 6, "Six slabs registered");
    if(cp != NULL){
        r |= Test(cp->pages.nvalues == count+5, "Five additional slabs registered in chapter, have %d", cp->pages.nvalues);
    }

    MemCtx_Free(m);
    r |= Test(m->p.nvalues == 3, "Three slabs registered, after temp wipe");
    if(cp != NULL){
        r |= Test(cp->pages.nvalues == count+2, "Two additional slabs registered in chapter, have", cp->pages.nvalues);
    }

    m->type.range--;
    MemCtx_Free(m);
    if(cp != NULL){
        r |= Test(cp->pages.nvalues == count-1, "After removal one less page than count, because it belonged to the MemCtx, have %d.", cp->pages.nvalues);
    }

    DebugStack_Pop();
    return r;
}
