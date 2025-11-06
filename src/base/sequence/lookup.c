#include <external.h>
#include <caneka.h>

i64 Lookup_GetRaw(Lookup *lk, word type){
    void *ptr = Lookup_Get(lk, type);
    if(ptr == NULL){
        return -1;
    }else{
        return (i64)ptr;
    }
}

status Lookup_AddRaw(MemCh *m, Lookup *lk, word type, i64 n){
    return Lookup_Add(m, lk, type, (void *)n);
}

void *Lookup_Get(Lookup *lk, word type){
    void *result = NULL;
    lk->type.state &= ~(SUCCESS|NOOP|ERROR);
    if(type >= lk->offset && type <= lk->offset+lk->values->max_idx){
        result = (void *)Span_Get(lk->values, (i32)(type-lk->offset));
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

Lookup *Lookup_FromConfig(MemCh *m, LookupConfig *config){
    LookupConfig *cnf = config;
    Lookup *lk = Lookup_Make(m, cnf->key);
    while(cnf->key != 0){
        Lookup_Add(m, lk, cnf->key, cnf->a); 
        cnf++;
    };
    return lk;
}

status Lookup_Add(MemCh *m, Lookup *lk, word type, void *value){
    if(type < lk->offset){
        Abstract *args[] = {
            (Abstract *)I16_Wrapped(m, lk->offset),
            (Abstract *)I16_Wrapped(m, type),
            NULL,
        };
        Error(m, FUNCNAME, FILENAME, LINENUMBER, "Adding lookup value below offset of $, have $ ", args);
        return ERROR;
    }else if(Span_Set(lk->values, (int)(type-lk->offset), (Abstract *)value) & SUCCESS){
        return SUCCESS;
    }

    return ERROR;
}

status Lookup_Concat(MemCh *m, Lookup *lk, Lookup *add){
    status r = READY;
    Iter it;
    Iter_Init(&it, add->values);
    while((Iter_Next(&it) & END) == 0){
        Abstract *a = (Abstract *)it.value;
        if(a != NULL){
            r |= Lookup_Add(m, lk, it.idx+add->offset, a);
        }
    }

    if(r == READY){
        r |= NOOP;
    }

    return r;
}

Lookup *LookupInt_Make(MemCh *m, word offset){
    Lookup *lk = (Lookup *)MemCh_Alloc(m, sizeof(Lookup));
    lk->type.of = TYPE_LOOKUP;
    lk->offset = offset;
    lk->values = Span_Make(m);

    return lk;
}

Lookup *Lookup_Make(MemCh *m, word offset){
    Lookup *lk = (Lookup *)MemCh_Alloc(m, sizeof(Lookup));
    lk->type.of = TYPE_LOOKUP;
    lk->offset = offset;
    lk->values = Span_Make(m);

    return lk;
}
