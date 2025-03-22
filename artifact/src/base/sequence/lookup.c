#include <external.h>
#include <caneka.h>

void *Lookup_Get(Lookup *lk, word type){
    void *result = NULL;
    lk->type.state &= ~(SUCCESS|NOOP|ERROR);
    if(type >= lk->offset && type <= lk->offset+lk->values->max_idx){
        result = (void *)Span_Get(lk->values, (int)(type-lk->offset));
        lk->type.state |= (lk->values->type.state & (SUCCESS|NOOP|ERROR));
        if(result != NULL){
            lk->latest_idx = type-lk->offset;
        }else{
            lk->latest_idx = -1;
        }
    }
    return result;
}

int Lookup_GetKey(Lookup *lk, int idx){
    if(idx <= lk->values->max_idx){
        return lk->offset+idx;
    }
    return -1;
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
        printf("type:%d offset:%d\n", type, lk->offset);
        Fatal("Adding lookup value below zero", TYPE_UNIT);
    }
    if(Span_Set(lk->values, (int)(type-lk->offset), (Abstract *)value) != NULL){
        return SUCCESS;
    }
    return ERROR;
}

status Lookup_Concat(MemCtx *m, Lookup *lk, Lookup *add){
    status r = READY;
    Iter it;
    Iter_Init(&it, add->values);
    while((Iter_Next(&it) & END) == 0){
        Abstract *a = Iter_Get(&it);
        if(a != NULL){
            r |= Lookup_Add(m, lk, it.idx+add->offset, a);
        }
    }

    if(r == READY){
        r |= NOOP;
    }

    return r;
}

Lookup *LookupInt_Make(MemCtx *m, word offset, Abstract *arg){
    Lookup *lk = (Lookup *)MemCtx_Alloc(m, sizeof(Lookup));
    lk->type.of = TYPE_LOOKUP;
    lk->offset = offset;
    lk->values = Span_Make(m);
    lk->arg = arg;

    return lk;
}

Lookup *Lookup_Make(MemCtx *m, word offset, LookupPopulate populate, Abstract *arg){
    Lookup *lk = (Lookup *)MemCtx_Alloc(m, sizeof(Lookup));
    lk->type.of = TYPE_LOOKUP;
    lk->offset = offset;
    lk->values = Span_Make(m);
    lk->arg = arg;
    if(populate != NULL){
        populate(m, lk);
    }

    return lk;
}
