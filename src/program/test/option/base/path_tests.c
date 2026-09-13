#include <external.h>
#include <caneka.h>
#include <test_module.h>

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

status Path_Tests(MemCh *m){
    Debug_Push(m, NULL);
    status r = READY;
    void *args[3];

    StrVec *v = StrVec_Make(m);
    Str *s = Str_CstrRef(m, "/fancy/path/thing/file.ext");
    StrVec_Add(v, s);

    Span *p = Span_Make(m);
    Span_Add(p, B_Wrapped(m, (byte)'/', ZERO, MORE));
    Span_Add(p, B_Wrapped(m, (byte)'.', ZERO, LAST));
    StrVec *annotated = Path_Annotate(m, v, p);

    i32 i;
    TestExp *exp = expected;
    for(i = 0; exp->bytes != NULL; i++, exp++){
        Str *s = (Str *)Span_Get(annotated->p, i);
        void *args[] = {
            I16_Wrapped(m, exp->length),
            s,
            NULL,
        };
        r |= Test(exp->length ==  s->length, "Expected length to equal expected $, have &",
            args); 
        r |= Test(strncmp(exp->bytes, (char *)s->bytes, s->length) == 0,
            "Expected content to equal expected $, have &", args);
        r |= Test(s->type.state == exp->flags,
            "Expected flags to equal expected $, have &", args);
    }

    Str *fname = IoUtil_FnameStr(m, annotated);
    StrVec *bname = IoUtil_BasePath(m, annotated);

    Str *expectedBase = S(m, "/fancy/path/thing/");
    Str *expectedFname = S(m, "file.ext");

    args[0] = expectedBase;
    args[1] = bname;
    args[2] = NULL;
    r |= Test(Equals(bname, expectedBase), 
        "base path is extracted properly expected @, have @", args);
    args[0] = expectedFname;
    args[1] = fname;
    args[2] = NULL;
    r |= Test(Equals(fname, expectedFname), 
        "file name is extracted properly expected @, have @", args);

    Return(m, r);
}
