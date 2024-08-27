#include <external.h>
#include <caneka.h>

void *Lookup_Get(Lookup *lk, word type){
    void *result = NULL;
    if(type >= lk->offset && type <= lk->offset+lk->values->max_idx){
        result = (void *)Span_Get(lk->values, (int)(type-lk->offset));
        if(result != NULL){
            lk->latest_idx = type-lk->offset;
        }else{
            lk->latest_idx = -1;
        }
    }
    return result;
}

word Lookup_AbsFromIdx(Lookup *lk, word idx){
    return idx+lk->offset;
}

Lookup *Lookup_FromConfig(MemCtx *m, LookupConfig *config, Abstract *arg){
    LookupConfig *cnf = config;
    Lookup *lk = Lookup_Make(m, cnf->key, NULL, arg);
    while(cnf->key != 0){
        Lookup_Add(m, lk, cnf->key, cnf->a); 
        cnf++;
    };
    return lk;
}

status Lookup_Add(MemCtx *m, Lookup *lk, word type, void *value){
    if(type < lk->offset){
        Fatal("Adding lookup value below zero", TYPE_UNIT);
    }
    if(Span_Set(lk->values, (int)(type-lk->offset), (Abstract *)value) != NULL){
        return SUCCESS;
    }
    return ERROR;
}

Lookup *LookupInt_Make(MemCtx *m, word offset, Abstract *arg){
    Lookup *lk = (Lookup *)MemCtx_Alloc(m, sizeof(Lookup));
    lk->offset = offset;
    lk->values = Span_MakeInline(m, TYPE_SPAN, sizeof(int));
    lk->values->type.state |= RAW;
    lk->values->def->flags |= RAW;
    lk->arg = arg;

    return lk;
}

Lookup *Lookup_Make(MemCtx *m, word offset, LookupPopulate populate, Abstract *arg){
    Lookup *lk = (Lookup *)MemCtx_Alloc(m, sizeof(Lookup));
    lk->offset = offset;
    lk->values = Span_Make(m, TYPE_SPAN);
    lk->arg = arg;
    if(populate != NULL){
        populate(m, lk);
    }

    return lk;
}
