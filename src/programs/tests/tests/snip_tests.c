#include <external.h>
#include <caneka.h>
#include <tests.h>

status Snip_Tests(MemCh *gm){
    status r = READY;
    MemCh *m = MemCh_Make();
    Str *s;
    StrVec *v;
    Cursor *curs;
    Span *sns;
    Snip *sn;

    sns = Span_Make(m);
    sn = Snip_Make(m);
    v = StrVec_Make(m);
    curs = Cursor_Make(m, StrVec_Make(m));
    Cursor_Add(curs, Str_CstrRef(m , "Rain "));
    Cursor_Add(curs, Str_CstrRef(m , "and sunshine, and clouds and sky."));
    
    sn->type.state = SNIP_GAP;
    sn->length = 9;
    SnipSpan_Add(sns, sn);
    sn->type.state = SNIP_CONTENT;
    sn->length = 8;
    SnipSpan_Add(sns, sn);

    Cursor_Incr(curs, (9 + 8)-1);
    void *args2[] = {
        curs,
        NULL
    };

    v = StrVec_Snip(m, sns, curs);

    s = Str_CstrRef(m, "sunshine");
    void *args1[] = {
        s,
        v,
        Ptr_Wrapped(m, sns, TYPE_SNIPSPAN),
        curs->v,
        NULL
    };
    r |= Test(Equals(v, s),
        "Found expected keyword @, have @ from @ of @", args1);

    MemCh_Free(m);
    return r;
}

status SnipModify_Tests(MemCh *gm){
    status r = READY;
    MemCh *m = MemCh_Make();
    Str *s;
    StrVec *v;
    Cursor *curs;
    Span *sns;
    Snip *sn;
    Snip _sn;
    memset(&_sn, 0, sizeof(Snip));
    _sn.type.of = TYPE_SNIP;

    sns = Span_Make(m);
    sn = Snip_Make(m);
    v = StrVec_Make(m);
    curs = Cursor_Make(m, StrVec_Make(m));
    Cursor_Add(curs, Str_CstrRef(m , "Start with some text, "));
    Cursor_Add(curs, Str_CstrRef(m , "that has a _link=article line@http://example.com inside it."));

    _sn.type.state = SNIP_CONTENT;
    _sn.length = 22;
    SnipSpan_Add(sns, &_sn);

    _sn.type.state = SNIP_STR_BOUNDRY;
    _sn.length = 0;
    SnipSpan_Add(sns, &_sn);

    _sn.type.state = SNIP_CONTENT;
    _sn.length = 16;
    SnipSpan_Add(sns, &_sn);

    Cursor_Incr(curs, 38);
    Cursor_Decr(curs, 1);

    SnipSpan_Set(m, sns, 5, SNIP_GAP);

    s = Str_CstrRef(m, "Start with some text, that has a ");
    v = StrVec_Snip(m, sns, curs);
    void *args1[] = {
        v,
        s,
        NULL
    };
    r |= Test(Equals(v, s), 
        "Start of line matches, expected &, have &", args1);
    
    SnipSpan_SetAll(sns, SNIP_GAP);
    sn = (Snip *)Span_Get(sns, 0);
    void *args2[] = {
        Ptr_Wrapped(OutStream->m, sns, TYPE_SNIPSPAN),
        NULL
    };
    r |= Test((sn->length == 38 && sns->nvalues == 1), 
        "SnipSpan now has single value which is a gap that spans the total length, have &\n", args2);

    MemCh_Free(m);
    return r;
}
