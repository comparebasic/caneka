#include <external.h>
#include <caneka.h>

void WwwPage_AddPath(WwwPage *page, StrVec *path){
    return;
}

status WwwPage_Init(MemCh *m){
    status r = READY;
    Table *tbl = Table_Make(m);
    Table_Set(tbl, S(m, "name"), I16_Wrapped(m, TYPE_STRVEC));
    Table_Set(tbl, S(m, "atts"), I16_Wrapped(m, TYPE_TABLE));
    Table_Set(tbl, S(m, "children"), I16_Wrapped(m, TYPE_TABLE));
    Table_Set(tbl, S(m, "nav"), I16_Wrapped(m, TYPE_ITER));
    Table_Set(tbl, S(m, "coords"), I16_Wrapped(m, TYPE_SPAN));
    Table_Set(tbl, S(m, "url"), I16_Wrapped(m, TYPE_STRVEC));
    Table_Set(tbl, S(m, "gens"), I16_Wrapped(m, TYPE_SPAN));
    r |= Seel_Seel(m, tbl, S(m, "WwwPage"), TYPE_WWW_PAGE);
    return r;
}
