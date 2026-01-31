#include <external.h>
#include <caneka.h>

WwwNav *WwwNav_Make(MemCh *m, StrVec *_path, StrVec *name){
    WwwNav *item = Inst_Make(m, TYPE_WWW_NAV);
    StrVec *path = IoPath_FromVec(m, _path);
    Seel_Set(item, K(m, "url"), path);
    Seel_Set(item, K(m, "name"), name);
    return item;
}

status WwwNav_Add(WwwNav *root, WwwNav *item, Table *coordTbl){
    MemCh *m = root->m;
    Span *coords = Span_Make(m);
    StrVec *url = Seel_Get(item, K(m, "url"));
    StrVec *name = Seel_Get(item, K(m, "name"));
    Inst_ByPath(root, url, item, SPAN_OP_SET, coords);
    Table_Set(coordTbl, name, coords);
    return ZERO;
}

status WwwNav_Init(MemCh *m){
    status r = READY;
    Table *tbl = Table_Make(m);
    Table_Set(tbl, S(m, "name"), I16_Wrapped(m, TYPE_STRVEC));
    Table_Set(tbl, S(m, "atts"), I16_Wrapped(m, TYPE_TABLE));
    Table_Set(tbl, S(m, "children"), I16_Wrapped(m, TYPE_TABLE));
    Table_Set(tbl, S(m, "url"), I16_Wrapped(m, TYPE_STRVEC));
    r |= Seel_Seel(m, tbl, S(m, "WwwNav"), TYPE_WWW_NAV);

    IterApi *api = IterApi_Make(m, NestSel_Next, NULL, NestSel_Get);
    Lookup_Add(m, IterApiLookup, TYPE_WWW_NAV, (void *)api);
    return r;
}
