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

    m->level++;
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
    m->level--;
    Abstract *args2[] = {
        (Abstract *)I32_Wrapped(m, m->it.p->nvalues),
        NULL
    };
    r |= Test(m->it.p->nvalues == 3, "Three slabs registered, after temp wipe have, $", args2);
    if(cp != NULL){
        Abstract *args3[] = {
            (Abstract *)I32_Wrapped(m, recycled),
            (Abstract *)I32_Wrapped(m, cp->recycled.p->nvalues),
            NULL
        };
        r |= Test(cp->recycled.p->nvalues == recycled+3,
            "Two additional slabs registered in chapter $recycled, have $", args3);
    }
    m->level++;

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

    m->level--;
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

status MemChLevel_Tests(MemCh *gm){
    status r = READY;
    MemCh *m = MemCh_Make();
    Abstract *args[5];

    MemBook *cp = NULL;
    i32 recycleCount = 0;
    i32 count = 0;
#ifdef INSECURE
    cp = MemBook_Get(m);
#endif

    MemCh *x = MemCh_Make();
    
    if(cp != NULL){
        args[0] = (Abstract *)&cp->recycled;
        args[1] = NULL;
        Out("^p.Recycled @^0\n", args);
    }

#ifdef INSECURE
    cp->type.state |= DEBUG;
#endif

    i32 slabs = m->it.p->nvalues;
    if(cp != NULL){
        recycleCount = cp->recycled.p->nvalues;
    }
    m->level++;
    Str_Make(x, STR_MAX);
    Str_Make(x, STR_MAX);
    Str_Make(x, STR_MAX);
    Str_Make(x, STR_MAX);
    Str_Make(x, STR_MAX);
    Str_Make(x, STR_MAX);
    Str_Make(x, STR_MAX);
    /*
    if(cp != NULL){
        count = cp->recycled.p->nvalues;
        args[0] = (Abstract *)I32_Wrapped(m, recycleCount);
        args[1] = (Abstract *)I32_Wrapped(m, count);
        args[2] = (Abstract *)I32_Wrapped(m, x->it.p->nvalues);
        args[3] = NULL;
        Out("^p.RecycledStart @, recycleCurrent @ -> MemCh.pages @^0\n", args);
    }
    */
    MemCh_Free(x);
#ifdef INSECURE
    cp->type.state &= ~DEBUG;
#endif
    /*
    if(cp != NULL){
        count = cp->recycled.p->nvalues;
        args[0] = (Abstract *)I32_Wrapped(m, recycleCount);
        args[1] = (Abstract *)I32_Wrapped(m, count);
        args[2] = (Abstract *)I32_Wrapped(m, x->it.p->nvalues);
        args[3] = NULL;
        Out("^p.RecycledStart @, recycleCurrent @ -> MemCh.pages @^0\n", args);
    }
    */
    m->level--;

    r |= ERROR;
    x->level = 0;
    MemCh_Free(x);
    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
