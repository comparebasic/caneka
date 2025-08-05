#include <external.h>
#include <caneka.h>

Abstract *Fetch(Fetcher *fch, Abstract *a, Abstract *source){
    if(fch->type.state & FETCHER_ATT){
        if(a->type.of == fch->objType.of && fch->idx >= 0){
            return (Abstract *)(void **)(((void *)a)+fch->idx);
        }else if(fch->key != NULL){
            Abstract *ret = NULL;
            AttTable_Att(fch->m, a, fch->key, &ret, &fch->idx);
            return ret;
        }else{
            Error(ErrStream->m, (Abstract *)fch, FUNCNAME, FILENAME, LINENUMBER,
                "Fetch by att requires a key of offset idx", NULL);
            return NULL;
        }
    }else if(fch->type.state & FETCHER_IDX){
        if(a->type.of == TYPE_SPAN){
            return Span_Get((Span *)a, fch->idx);
        }else if(a->type.of == TYPE_ORDTABLE){
            return Span_Get(((OrdTable *)a)->order, fch->idx);
        }else if(a->type.of == TYPE_ITER){
            Iter *it = (Iter *)a;
            Iter_GetByIdx(it, fch->idx);
            if(it->type.state & SUCCESS){
                return Iter_Get((Iter *)a);
            }
        }else{
            Error(ErrStream->m, (Abstract *)fch, FUNCNAME, FILENAME, LINENUMBER,
                "Fetch by idx on this object is not supported", NULL);
            return NULL;
        }
    }else if((fch->type.state & (FETCHER_KEY|FETCHER_VALUE)) && fch->key != NULL){
        Hashed *h = NULL;
        if(a->type.of == TYPE_ORDTABLE){
            h = OrdTable_Get((OrdTable *)a, fch->key);
        }else if(a->type.of == TYPE_TABLE){
            h = Table_GetHashed((Table *)a, fch->key);
        }

        if(h != NULL){
            if(fch->type.state & FETCHER_KEY){
                return h->key;
            }else if (fch->type.state & FETCHER_VALUE){
                return h->value;
            }
        }
    }else if(fch->type.state & FETCHER_ITER){
        Span *p = NULL;
        if(a->type.of == TYPE_ORDTABLE){
            p = ((OrdTable *)a)->order;
        }else if(a->type.of == TYPE_SPAN){
            p = (Span *)a;
        }
        if(p == NULL){
            Error(ErrStream->m, (Abstract *)fch, FUNCNAME, FILENAME, LINENUMBER,
                "Iterator for this object not supported", NULL);
            return NULL;
        }
        return (Abstract *)Iter_Make(fch->m, p);
    }else if(fch->type.state & FETCHER_FUNC){
        if(a->type.of == fch->objType.of && fch->func != NULL){
            return fch->func(fch, a, source);
        }else{
            FetchFunc func = Lookup_Get(fch->lk, a->type.of);
            if(func != NULL){
                fch->objType.of = a->type.of;
                fch->func = func;
                return fch->func(fch, a, source);
            }else{
                Error(ErrStream->m, (Abstract *)fch, FUNCNAME, FILENAME, LINENUMBER,
                    "Lookup func for typeOf not found", NULL);
                return NULL;
            }
        } 
    }
    return NULL;
}

status Fetcher_SetOp(Fetcher *fch, word op){
    fch->objType.state = op;
    return SUCCESS;
}

Fetcher *Fetcher_Make(MemCh *m, StrVec *path, i32 idx, Abstract *key, word flags, word op){
    Fetcher *fch = (Fetcher *)MemCh_Alloc(m, sizeof(Fetcher));
    fch->type.of = TYPE_FETCHER;
    fch->type.state = flags;
    fch->objType.state = op;
    fch->path = path;
    fch->idx = idx;
    fch->key = key;
    return fch;
}
