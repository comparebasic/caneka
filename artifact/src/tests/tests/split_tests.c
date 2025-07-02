#include <external.h>
#include <caneka.h>

status StrVecSplit_Tests(MemCh *gm){
    status r = READY;
    MemCh *m = MemCh_Make();

    Span *p = NULL;
    Abstract *split = NULL;
    Span *expected = Span_Make(m);
    Span_Add(expected, (Abstract *)Str_CstrRef(m, "this"));
    Span_Add(expected, (Abstract *)Str_CstrRef(m, "that"));
    Span_Add(expected, (Abstract *)Str_CstrRef(m, "thing"));
    Span_Add(expected, (Abstract *)Str_CstrRef(m, "x"));

    StrVec *orig = StrVec_Make(m);
    StrVec_Add(orig, Str_CstrRef(m, "this.that.thingy.x"));
    StrVec *v = (StrVec *)StrVec_Clone(m, (Abstract *)orig);
    split = (Abstract *)Str_CstrRef(m ,".");
    StrVec_Split(v, split); 
    p = StrVec_ToSpan(m, v);

    Abstract *args[] = {
        (Abstract *)v,
        split,
        (Abstract *)p,
        NULL
    };
    r |= Test(Equals((Abstract *)expected, (Abstract *)p),
        "Split by & @ becomes @.\n", args);

    v = (StrVec *)StrVec_Clone(m, (Abstract *)orig);
    Match *mt = Match_Make(m, PatChar_FromStr(m, Str_CstrRef(m, ".")), Span_Make(m));
    mt->type.state |= MATCH_SEARCH;
    split = (Abstract *)mt;
    StrVec_Split(v, split);
    p = StrVec_ToSpan(m, v);

    Abstract *args2[] = {
        (Abstract *)v,
        split,
        (Abstract *)p,
        NULL
    };
    r |= Test(Equals((Abstract *)expected, (Abstract *)p),
        "Split by & @ becomes @.\n", args2);

    v = (StrVec *)StrVec_Clone(m, (Abstract *)orig);
    split = (Abstract *)I8_Wrapped(m, '.');
    StrVec_Split(v, split); 
    p = StrVec_ToSpan(m, v);

    Abstract *args3[] = {
        (Abstract *)v,
        split,
        (Abstract *)p,
        NULL
    };
    r |= Test(Equals((Abstract *)expected, (Abstract *)p),
        "Split by & @ becomes @.\n", args3);


    MemCh_Free(m);

    return r;
    
}
