#include <external.h>
#include <caneka.h>
#include <test_module.h>

status StrVecSplit_Tests(MemCh *m){
    status r = READY;

    Span *p = NULL;
    void *split = NULL;
    Span *expected = Span_Make(m);
    Span_Add(expected, Str_CstrRef(m, "this"));
    Span_Add(expected, Str_CstrRef(m, "that"));
    Span_Add(expected, Str_CstrRef(m, "thing"));
    Span_Add(expected, Str_CstrRef(m, "x"));

    StrVec *orig = StrVec_Make(m);
    StrVec_Add(orig, Str_CstrRef(m, "this.that.thingy.x"));
    StrVec *v = (StrVec *)StrVec_Clone(m, orig);
    split = Str_CstrRef(m ,".");
    StrVec_Split(v, split); 
    p = StrVec_ToSpan(m, v);

    void *args[] = {
        v,
        split,
        p,
        NULL
    };
    r |= Test(Equals(expected, p), "Split by & @ becomes @.\n", args);

    v = (StrVec *)StrVec_Clone(m, orig);
    Match *mt = Match_Make(m, PatChar_FromStr(m, Str_CstrRef(m, ".")), Span_Make(m));
    mt->type.state |= MATCH_SEARCH;
    split = mt;
    StrVec_Split(v, split);
    p = StrVec_ToSpan(m, v);

    void *args2[] = {
        v,
        split,
        p,
        NULL
    };
    r |= Test(Equals(expected, p),
        "Split by & @ becomes @.", args2);

    v = (StrVec *)StrVec_Clone(m, orig);
    split = I8_Wrapped(m, '.');
    StrVec_Split(v, split); 
    p = StrVec_ToSpan(m, v);

    void *args3[] = {
        v,
        split,
        p,
        NULL
    };
    r |= Test(Equals(expected, p),
        "Split by & @ becomes @.", args3);

    return r;
}
