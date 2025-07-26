#include <external.h>
#include <caneka.h>

typedef struct test_exp {
    char *bytes;
    i16 length;
    word flags;
} TestExp;

TestExp expected[] = {
    {"/", 1, MORE},
    {"fancy", 5, ZERO},
    {"/", 1, MORE},
    {"path", 4, ZERO},
    {"/", 1, MORE},
    {"thing", 5, ZERO},
    {"/", 1, MORE},
    {"file", 4, ZERO},
    {".", 1, LAST},
    {"ext", 3, ZERO},
    {NULL, 0, ZERO},
};

status Path_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;

    MemCh *m = MemCh_Make();

    StrVec *v = StrVec_Make(m);
    Str *s = Str_CstrRef(m, "/fancy/path/thing/file.ext");
    StrVec_Add(v, s);

    Span *p = Span_Make(m);
    Span_Add(p, (Abstract *)B_Wrapped(m, (byte)'/', ZERO, MORE));
    Span_Add(p, (Abstract *)B_Wrapped(m, (byte)'.', ZERO, LAST));
    Path_Annotate(m, v, p);

    i32 i;
    TestExp *exp = expected;
    for(i = 0; exp->bytes != NULL; i++, exp++){
        Str *s = (Str *)Span_Get(v->p, i);
        Abstract *args[] = {
            (Abstract *)s,
            NULL,
        };
        r |= Test(exp->length ==  s->length, "Expected length to equal expected for &",
            args); 
        r |= Test(strncmp(exp->bytes, (char *)s->bytes, s->length) == 0,
            "Expected content to equal expected for &", args);
        r |= Test(s->type.state == exp->flags,
            "Expected flags to equal expected for &", args);
    }

    StrVec *fname = IoUtil_Fname(m, v);
    StrVec *bname = IoUtil_BasePath(m, v);

    r |= Test(Equals((Abstract *)bname, (Abstract *)Str_CstrRef(m, "/fancy/path/thing/")), 
        "base path is extracted properly", NULL);
    r |= Test(Equals((Abstract *)fname, (Abstract *)Str_CstrRef(m, "file.ext")), 
        "file name is extracted properly", NULL);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
