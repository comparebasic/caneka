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
    for(i32 i = 0; i < (9 + 8); i ++){
        Cursor_NextByte(curs);
    }

    v = StrVec_Snip(m, sns, curs);

    s = Str_CstrRef(m, "sunshine");
    Abstract *args1[] = {
        (Abstract *)s,
        (Abstract *)v,
        (Abstract *)Ptr_Wrapped(m, sns, TYPE_SNIPSPAN),
        (Abstract *)curs->v,
        NULL
    };
    r |= Test(Equals((Abstract *)v, (Abstract *)s),
        "Found expected keyword @, have @ from @ of @", args1);

    Abstract *args2[] = {
        (Abstract *)curs,
        NULL
    };
    Debug("^p.Cursor: @^0.\n", args2);
    MemCh_Free(m);
    return r;
}
