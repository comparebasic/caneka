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

    StrVec *vec = StrVec_From(pst, Str_FromCstr(pst, "Halo", STRING_COPY));

    p = Span_Make(pst);
    Span_Add(p, (Abstract *)one);
    Span_Add(p, (Abstract *)two);
    Span_Add(p, (Abstract *)three);
    Span_Add(p, (Abstract *)vec);

    args[0] = (Abstract *)pst;
    args[1] = NULL;
    Out("^p.Persist MemCh: &^0\n", args);

    Str *path = IoUtil_GetCwdPath(m, Str_CstrRef(m, "dist/test/persist.mem"));

    i32 fd = open(Str_Cstr(m, path), O_WRONLY|O_CREAT, 00644);
    Stream *sm = Stream_MakeToFd(m, fd, StrVec_Make(m), ZERO);
    status re = Persist_FlushFree(sm, pst);
    close(fd);

    fd = open(Str_Cstr(m, path), O_RDONLY);
    sm = Stream_MakeFromFd(m, fd, ZERO);
    MemCh *loaded = Persist_FromStream(sm);
    close(fd);

    r |= Test(loaded != NULL, 
        "Persist From Stream returns non-null", NULL);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
