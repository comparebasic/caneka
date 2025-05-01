#include <external.h>
#include <caneka.h>

status MemCh_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;

    MemCh *m = MemCh_Make();
    MemBook *cp = NULL;
#ifdef INSECURE
    cp = MemBook_Get(m);
#endif
    i32 count = 0;
    if(cp != NULL){
        count = cp->it.span->nvalues;
        r |= Test(count > 0, "Book slab count is greater than 0, have %d.", count);
    }

    i64 max = 1024;
    for(i64 i = 0; i < max; i++){
        i64 *p = MemCh_Alloc(m, sizeof(i64));
        memcpy(p, &i, sizeof(i64));
    }

    m->type.range++;
    m->it.span->type.state |= DEBUG;
    for(i64 i = 0; i < max; i++){
        i64 *p = MemCh_Alloc(m, sizeof(i64));
        memcpy(p, &i, sizeof(i64));
    }

    r |= Test(m->it.span->nvalues == 6, "Six slabs registered, have _i4", m->it.span->nvalues);
    if(cp != NULL){
        r |= Test(cp->it.span->nvalues == count+5, "Five additional slabs registered in chapter, have %d", cp->it.span->nvalues);
    }

    MemCh_Free(m);
    r |= Test(m->it.span->nvalues == 3, "Three slabs registered, after temp wipe have, _i4", m->it.span->nvalues);
    if(cp != NULL){
        r |= Test(cp->it.span->nvalues == count+2, "Two additional slabs registered in chapter, have", cp->it.span->nvalues);
    }

    m->type.range--;
    MemCh_Free(m);
    if(cp != NULL){
        r |= Test(cp->it.span->nvalues == count-1, "After removal one less page than count, because it belonged to the MemCh, have %d.", cp->it.span->nvalues);
    }

    DebugStack_Pop();
    return r;
}
