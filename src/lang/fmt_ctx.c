#include <external.h>
#include <caneka.h>

status Fmt_Add(MemCtx *m, FmtCtx *o, Lookup *fmtDefs){
    Iter *it = Iter_Make(m, fmtDefs->values);
    Lookup *byId = Lookup_Make(m, fmtDefs->offset, NULL, NULL);
    Span *byName = Span_Make(m, TYPE_TABLE);
    Span *byAlias = Span_Make(m, TYPE_TABLE);
    while((Iter_Next(it) & END) == 0){
        FmtDef *def = (FmtDef *)Iter_Get(it);
        if(def != NULL){
            Table_Set(byName, (Abstract *)def->name, (Abstract *)def);
            if(def->alias != NULL){
                Table_Set(byAlias, (Abstract *)def->alias, (Abstract *)def);
            }
            Lookup_Add(m, byId, def->id, (Abstract *)def);
        }
    }
    if(o->byId == NULL){
        o->byId = Chain_Make(m, byId);
    }else{
        Chain_Extend(m, o->byId, byId);
    }
    if(o->byName == NULL){
        o->byName = TableChain_Make(m, byName);
    }else{
        TableChain_Extend(m, o->byName,  byName);
    }
    if(o->byAlias == NULL){
        o->byAlias = TableChain_Make(m, byAlias);
    }else{
        TableChain_Extend(m, o->byAlias,  byAlias);
    }
    return SUCCESS;
}

FmtCtx *FmtCtx_Make(MemCtx *m){
    FmtCtx *fmt = (FmtCtx *)MemCtx_Alloc(m, sizeof(FmtCtx));
    fmt->type.of = TYPE_FMT_CTX;
    fmt->m = m;
    return fmt;
}
