#include <external.h>
#include <caneka.h>

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
