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
        recycled = cp->recycled.p->nvalues;
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

    Abstract *args1[] = {
        (Abstract *)I32_Wrapped(m, m->it.p->nvalues), 
        NULL
    };
    r |= Test(m->it.p->nvalues == 6, "Six slabs registered, have $", args1);

    MemCh_Free(m);
    m->type.range--;
    Abstract *args2[] = {
        (Abstract *)I32_Wrapped(m, m->it.p->nvalues),
        NULL
    };
    r |= Test(m->it.p->nvalues == 3, "Three slabs registered, after temp wipe have, $", args2);
    if(cp != NULL){
        Abstract *args3[] = {
            (Abstract *)I32_Wrapped(m, cp->recycled.p->nvalues),
            NULL
        };
        r |= Test(cp->recycled.p->nvalues == recycled+3,
            "Two additional slabs registered in chapter, have $", args3);
    }
    m->type.range++;

#ifdef INSECURE
    pageIdx = MemBook_GetPageIdx(m);
#endif

    max = 237;
    for(i64 i = 0; i < max; i++){
        i64 *p = MemCh_Alloc(m, sizeof(i64));
        memcpy(p, &i, sizeof(i64));
    }
    Abstract *args4[] = {
        (Abstract *)I64_Wrapped(m, max),
        (Abstract *)I32_Wrapped(m, m->it.p->nvalues),
        NULL
    };
    r |= Test(m->it.p->nvalues == 4,
        "Four slabs registered, after adding $ more i32s, have $", args4);
    if(cp != NULL){
        Abstract *args5[] = {
            (Abstract *)I32_Wrapped(m, cp->recycled.p->nvalues),
            NULL
        };
        r |= Test(cp->recycled.p->nvalues == recycled+2,
            "Two additional slabs registered in chapter, have $", args5);
        i32 currentBookIdx = MemBook_GetPageIdx(m);
        Abstract *args6[] = {
            (Abstract *)I32_Wrapped(m, currentBookIdx),
            NULL
        };
        r |= Test(pageIdx == currentBookIdx,
            "pageIdx has not changed, indicating old memslabs have been reused, have $", args6);
    }

    m->type.range--;
    printf("memRange: %d\n", (i32)m->type.range);
    MemCh_Free(m);
    if(cp != NULL){
        Single sg;
        sg.type.of = TYPE_WRAPPED_I32;
        sg.val.i = cp->recycled.p->nvalues;
        Abstract *args7[] = {
            (Abstract *)&sg,
            NULL
        };
        r |= Test(cp->recycled.p->nvalues == recycled+6,
            "After removal one less page than count, because it belonged to the MemCh, have $.", args7);
    }

    DebugStack_Pop();
    return r;
}
