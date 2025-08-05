#include <external.h>
#include <caneka.h>

static ClassDef *transpCls = NULL;

status Transp_ClassesInit(MemCh *m, Lookup *lk){
    status r = READY;
    if(transpCls == NULL){
        transpCls = ClassDef_Make(m);
        Table_Set(transpCls->attsTbl, (Abstract *)Str_CstrRef(m, "name"),
            (Abstract *)I16_Wrapped(m, (void *)(&tp.name)-(void *)(&tp)));
        Table_Set(transpCls->attsTbl, (Abstract *)Str_CstrRef(m, "local"),
            (Abstract *)I16_Wrapped(m, (void *)(&tp.local)-(void *)(&tp)));
        Table_Set(transpCls->attsTbl, (Abstract *)Str_CstrRef(m, "src"),
            (Abstract *)I16_Wrapped(m, (void *)(&tp.name)-(void *)(&tp)));
        Table_Set(transpCls->attsTbl, (Abstract *)Str_CstrRef(m, "dest"),
            (Abstract *)I16_Wrapped(m, (void *)(&tp.name)-(void *)(&tp)));
        r |= Lookup_Add(m, lk, TYPE_TRANSP_FILE, (void *)transpCls);
    }
    if(r == READY){
        r |= NOOP;
    }
    return r;
}
