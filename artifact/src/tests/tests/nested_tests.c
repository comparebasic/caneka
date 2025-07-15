#include <external.h>
#include <caneka.h>

status Nested_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    MemCh *m = MemCh_Make();
    Nested *nd = Nested_Make(m);

    StrVec *v = StrVec_From(m, Str_CstrRef(m, "one.two.three.keyFour"));
    StrVec_Split(v, (Abstract *)I8_Wrapped(m, '.'));
    Span *path = StrVec_ToSpan(m, v);

    Str *value = Str_CstrRef(m, "TheValue");
    Nested_AddByPath(nd, path, (Abstract *)value);

    Abstract *expected = Nested_GetByPath(nd, path);
    Abstract *args[] = {
        (Abstract *)path,
        (Abstract *)nd,
        (Abstract *)expected,
        (Abstract *)value,
        NULL
    };

    Test(Equals(expected, (Abstract *)value), 
        "Test that a variable can be set a retrieved on the path @, from &, expected @, have @", args);

    nd = Nested_Make(m);
    Nested_AddByKey(nd, (Abstract *)Str_CstrRef(m, "base"), 
        (Abstract *)OrdTable_Make(m));
    Nested_Indent(nd, (Abstract *)Str_CstrRef(m, "base"));
    Nested_AddByKey(nd, (Abstract *)Str_CstrRef(m, "one"), 
        (Abstract *)I32_Wrapped(m, 101));
    Nested_AddByKey(nd, (Abstract *)Str_CstrRef(m, "two"), 
        (Abstract *)I32_Wrapped(m, 102));
    Nested_AddByKey(nd, (Abstract *)Str_CstrRef(m, "three"), 
        (Abstract *)I32_Wrapped(m, 103));
    Nested_Outdent(nd);
    Nested_AddByKey(nd, (Abstract *)Str_CstrRef(m, "more"), 
        (Abstract *)OrdTable_Make(m));
    Nested_Indent(nd, (Abstract *)Str_CstrRef(m, "more"));
    Nested_AddByKey(nd, (Abstract *)Str_CstrRef(m, "one"), 
        (Abstract *)I32_Wrapped(m, 301));
    Nested_AddByKey(nd, (Abstract *)Str_CstrRef(m, "two"), 
        (Abstract *)I32_Wrapped(m, 302));

    Abstract *args2[] = {
        (Abstract *)nd,
        (Abstract *)((Frame *)Nested_GetRoot(nd))->value,
        NULL
    };
    Out("^p.Nested: &\n@\n^0.", args2);


    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
