#include <external.h>
#include <caneka.h>
#include <test_module.h>

status MemCh_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    status r = READY;

    MemBook *cp = NULL;
#ifdef INSECURE
    cp = MemBook_Get(m);
    cp->type.state |= DEBUG;
#endif

    MemBookStats st;
    MemBook_GetStats(m, &st);
    i32 start = st.total;

    MemCh *x = MemCh_Make();
    Span *p = Span_Make(x);
    for(i32 i = 0; i < 1000; i++){
        Span_Add(p, I32_Wrapped(x, i));
    }
    MemCh_Free(x);

    x = MemCh_Make();
    p = Span_Make(x);
    for(i32 i = 0; i < 500; i++){
        Span_Add(p, I32_Wrapped(x, i));
    }
    MemCh_Free(x);

    x = MemCh_Make();
    p = Span_Make(x);
    for(i32 i = 0; i < 2000; i++){
        Span_Add(p, I32_Wrapped(x, i));
    }
    MemCh_Free(x);

#ifdef INSECURE
    cp = MemBook_Get(m);
    cp->type.state &= ~DEBUG;
#endif

    MemBook_GetStats(m, &st);
    r |= Test(st.total == start, "MemBook chapters match the start level", NULL);

    DebugStack_Pop();
    return r;
}

status MemChLevel_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    status r = READY;

    MemBook *cp = NULL;
#ifdef INSECURE
    cp = MemBook_Get(m);
    cp->type.state |= DEBUG;
#endif

    MemBookStats st;
    MemBook_GetStats(m, &st);
    i32 start = st.total;

    MemCh *x = MemCh_Make();
    x->level++;
    Span *p = Span_Make(x);
    for(i32 i = 0; i < 700; i++){
        Span_Add(p, I32_Wrapped(x, i));
    }

    x->level++;
    p = Span_Make(x);
    for(i32 i = 0; i < 1000; i++){
        Span_Add(p, I32_Wrapped(x, i));
    }
    x->level--;
    MemCh_FreeTemp(x);

    p = Span_Make(x);
    for(i32 i = 0; i < 2000; i++){
        Span_Add(p, I32_Wrapped(x, i));
    }
    x->level--;
    MemBook_GetStats(m, &st);
    MemCh_FreeTemp(x);
    MemCh_Free(x);

#ifdef INSECURE
    cp = MemBook_Get(m);
    cp->type.state &= ~DEBUG;
#endif

    MemBook_GetStats(m, &st);
    r |= Test(st.total == start, "MemBook chapters match the start level", NULL);

    DebugStack_Pop();
    return r;
}
