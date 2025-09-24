#include <external.h>
#include <caneka.h>

status Persist_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    Abstract *args[5];
    MemCh *m = MemCh_Make();
    Span *p;
    status r = READY;
    Str *s;

    MemCh *pst = MemCh_Make();
    Str *one = Str_FromCstr(pst, "One", STRING_COPY);
    Str *two = Str_FromCstr(pst, "Two", STRING_COPY);
    Str *three = Str_FromCstr(pst, "Three", STRING_COPY);
    p = Span_Make(pst);
    Span_Add(p, (Abstract *)one);
    Span_Add(p, (Abstract *)two);
    Span_Add(p, (Abstract *)three);

    args[0] = (Abstract *)pst;
    args[1] = NULL;
    Out("^p.Persist MemCh: &\n", args);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
