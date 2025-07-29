#include <external.h>
#include <caneka.h>

static Table *transpFileAtts = NULL;

status Transp_AttsInit(MemCh *m, Lookup *lk){
    status r = READY;
    if(transpFileAtts == NULL){
        TranspFile tp;
        transpFileAtts = Table_Make(m);
        Table_Set(transpFileAtts, (Abstract *)Str_CstrRef(m, "name"),
            (Abstract *)I16_Wrapped(m, (void *)(&tp.name)-(void *)(&tp)));
        Table_Set(transpFileAtts, (Abstract *)Str_CstrRef(m, "local"),
            (Abstract *)I16_Wrapped(m, (void *)(&tp.local)-(void *)(&tp)));
        Table_Set(transpFileAtts, (Abstract *)Str_CstrRef(m, "src"),
            (Abstract *)I16_Wrapped(m, (void *)(&tp.name)-(void *)(&tp)));
        Table_Set(transpFileAtts, (Abstract *)Str_CstrRef(m, "dest"),
            (Abstract *)I16_Wrapped(m, (void *)(&tp.name)-(void *)(&tp)));
        r |= Lookup_Add(m, lk, TYPE_TRANSP_FILE, (void *)transpFileAtts);
    }
    if(r == READY){
        r |= NOOP;
    }
    return r;
}
