#include <external.h>
#include <caneka.h>

status MemCh_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;

    MemCh *m = MemCh_Make();
    MemBook *cp = NULL;
    i32 pageIdx = 0;
#ifdef INSECURE
    cp = MemBook_Get(m);
#endif
    i32 recycled = 0;
    r |= Test(cp != NULL, "Book is not NULL", NULL);
    if(cp != NULL){
        recycled = cp->recycled.span->nvalues;
    }

    i64 max = 1024;
    for(i64 i = 0; i < max; i++){
        i64 *p = MemCh_Alloc(m, sizeof(i64));
        memcpy(p, &i, sizeof(i64));
    }

    m->type.range++;
    for(i64 i = 0; i < max; i++){
        i64 *p = MemCh_Alloc(m, sizeof(i64));
        memcpy(p, &i, sizeof(i64));
    }

    void *args1[] = {&m->it.span->nvalues, NULL};
    r |= Test(m->it.span->nvalues == 6, "Six slabs registered, have _i4", args1);

    MemCh_Free(m);
    void *args2[] = {&m->it.span->nvalues, NULL};
    r |= Test(m->it.span->nvalues == 3, "Three slabs registered, after temp wipe have, _i4", args2);
    if(cp != NULL){
        void *args3[] = {&cp->recycled.span->nvalues, NULL};
        r |= Test(cp->recycled.span->nvalues == recycled+3,
            "Two additional slabs registered in chapter, have _i4", args3);
    }

#ifdef INSECURE
    pageIdx = MemBook_GetPageIdx(m);
#endif

    max = 237;
    for(i64 i = 0; i < max; i++){
        i64 *p = MemCh_Alloc(m, sizeof(i64));
        memcpy(p, &i, sizeof(i64));
    }
    void *args4[] = { &max, &m->it.span->nvalues, NULL};
    r |= Test(m->it.span->nvalues == 4,
        "Four slabs registered, after adding _i3 more i32s, have _i4", args4);
    if(cp != NULL){
        void *args5[] = {&cp->recycled.span->nvalues, NULL};
        r |= Test(cp->recycled.span->nvalues == recycled+2,
            "Two additional slabs registered in chapter, have _i4", args5);
        i32 currentBookIdx = MemBook_GetPageIdx(m);
        void *args6[] = {&currentBookIdx, NULL};
        r |= Test(pageIdx == currentBookIdx,
            "pageIdx has not changed, indicating old memslabs have been reused, have _i4", args6);
    }

    m->type.range--;
    MemCh_Free(m);
    if(cp != NULL){
        void *args7[] = {&cp->recycled.span->nvalues, NULL};
        r |= Test(cp->recycled.span->nvalues == recycled+6,
            "After removal one less page than count, because it belonged to the MemCh, have _i4.", args7);
    }

    DebugStack_Pop();
    return r;
}
