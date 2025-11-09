#include <external.h>
#include <caneka.h>

status Clone_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    Abstract *a = NULL;
    Abstract *b = NULL;

    MemCh *m = MemCh_Make();
    void **args = Arr_Make(m, 3);
    a = (Abstract *)Str_CstrRef(m, "fancy things");
    b =  Clone(m, a);
    args[0] = a;
    args[1] = b;
    r |= Test(Equals(a, b), "Str clone is equal, expected @ have @", args);

    Span *p = Span_Make(m);
    Span_Add(p, I32_Wrapped(m, 31));
    Span_Add(p, Str_CstrRef(m, "boo bah bing bang!"));
    Span_Add(p, Str_CstrRef(m, "hi"));
    Span_Add(p, Str_CstrRef(m, "there"));
    Span_Add(p, I32_Wrapped(m, 13));
    Span_Add(p, I8_Wrapped(m, 27));
    a = (Abstract *)p;
    b =  Clone(m, a);
    args[0] = a;
    args[1] = b;
    r |= Test(Equals(a, b), "Span clone is equal, expected @ have @", args);

    StrVec *v = StrVec_Make(m);
    StrVec_Add(v, Str_CstrRef(m, "And the end is naer, but we will have a hopping good"
      " time anyway! yay!"));
    a = (Abstract *)v;
    b =  Clone(m, a);
    args[0] = a;
    args[1] = b;
    r |= Test(Equals(a, b), "StrVec clone is equal, expected @ have @", args);


    DebugStack_Pop();
    return r;
}
