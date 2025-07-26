#include <external.h>
#include <caneka.h>

static Table *transpFileAtts = NULL;

status Transp_AttsInit(MemCh *m, Lookup *lk){
    status r = READY;
    if(transpFileAtts == NULL){
        TranspFile tp;
        Table *tbl = Table_Make(m);
        Table_Set(tbl, (Abstract *)Str_CstrRef(m, "name"),
            I16_Wrapped(m, (void *)(&tp.name)-(void *)(&tp)));
        Table_Set(tbl, (Abstract *)Str_CstrRef(m, "fullName"),
            I16_Wrapped(m, (void *)(&tp.fullName)-(void *)(&tp)));
        Table_Set(tbl, (Abstract *)Str_CstrRef(m, "src"),
            I16_Wrapped(m, (void *)(&tp.name)-(void *)(&tp)));
        Table_Set(tbl, (Abstract *)Str_CstrRef(m, "dest"),
            I16_Wrapped(m, (void *)(&tp.name)-(void *)(&tp)));
        r |= Lookup_Add(m, lk, TYPE_TRANSP_FILE, (void *)tbl);
    }
    if(r == READY){
        r |= NOOP;
    }
    return r;
}
