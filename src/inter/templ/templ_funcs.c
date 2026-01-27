#include <external.h>
#include <caneka.h>

Lookup *TemplFuncLookup = NULL;

status Templ_IterNext(Templ *templ){
    return NOOP;
}

status Templ_Indent(Templ *templ){
    return NOOP;
}

Func *Templ_GetFunc(cls typeOf){
    return Lookup_Get(TemplFuncLookup, typeOf);
}

status Templ_FuncInit(MemCh *m){
    status r = READY;
    if(TemplFuncLookup == NULL){
        TemplFuncLookup = Lookup_Make(m, ZERO);
    }
    r |= Lookup_Add(m, TemplFuncLookup, TYPE_ITER, (void *)Templ_IterNext);
    r |= Lookup_Add(m, TemplFuncLookup, FORMAT_TEMPL_INDENT, (void *)Templ_Indent);
    return r;
}
