#include <external.h>
#include <caneka.h>

status Fmt_Add(MemCtx *m, FmtCtx *o, Lookup *fmtDefs){
    Iter *it = Iter_Make(m, fmtDefs->values);
    Lookup *byId = Lookup_Make(m, fmtDefs->offset, NULL, NULL);
    Span *byName = Span_Make(m);
    Span *byAlias = Span_Make(m);
    while((Iter_Next(it) & END) == 0){
        FmtDef *def = (FmtDef *)Iter_Get(it);
        if(def != NULL){
            if(def->name != NULL){
                Table_Set(byName, (Abstract *)def->name, (Abstract *)def);
            }
            if(def->alias != NULL){
                Table_Set(byAlias, (Abstract *)def->alias, (Abstract *)def);
            }
            Lookup_Add(m, byId, def->id, (Abstract *)def);
        }
    }
    Chain_Extend(m, o->resolver->byId, byId);
    TableChain_Extend(m, o->resolver->byName,  byName);
    TableChain_Extend(m, o->resolver->byAlias,  byAlias);
    return SUCCESS;
}

FmtCtx *FmtCtx_Make(MemCtx *m, OutFunc out){
    FmtCtx *fmt = (FmtCtx *)MemCtx_Alloc(m, sizeof(FmtCtx));
    fmt->type.of = TYPE_FMT_CTX;
    fmt->m = m;
    fmt->out = out;
    fmt->resolver = FmtResolver_Make(m);
    return fmt;
}
