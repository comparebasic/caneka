#include <external.h>
#include <caneka.h>

Abstract *Fetch_FromPath(Fetcher *fch, Abstract *data, Abstract *source){
    if(data == NULL || fch->path == NULL || fch->path->total == 0){
        return NULL;
    }

    Iter it;
    Iter_Init(&it, fch->path->p);
    word flags = ZERO;
    while(data != NULL && (Iter_Next(&it) & END) == 0){
        Str *key = (Str *)Iter_Get(&it);
        if(key->type.state & (MORE|LAST)){
            flags = key->type.state;
        }else if(flags & LAST){
            if(fch->key != (Abstract *)key){
                fch->key = (Abstract *)key;
                fch->target.val.idx = -1;
                fch->func = NULL;
            }
            return Fetch(fch, data, source);
        }else{
            data = From_Key(data, (Abstract *)key);
        }
    }
    return data;
}

Abstract *Fetch(Fetcher *fch, Abstract *a, Abstract *source){
    if(fch->type.state & DEBUG){
        Abstract *args[] = {
            (Abstract *)fch,
            (Abstract *)a,
            NULL,
        };
        Out("^y.Fetch & from @^0.\n", args);
    }
    if(fch->target.type.state & FETCHER_ATT){
        if(a->type.of == fch->objType.of && fch->target.val.idx >= 0){
            return (Abstract *)(void **)(((void *)a)+fch->target.val.idx);
        }else if(fch->key != NULL){
            Abstract *ret = NULL;
            AttTable_Att(fch->m, a, fch->key, &ret, &fch->target.val.idx);
            return ret;
        }else{
            Error(ErrStream->m, (Abstract *)fch, FUNCNAME, FILENAME, LINENUMBER,
                "Fetch by att requires a key of offset idx", NULL);
            return NULL;
        }
    }else if(fch->target.type.state & FETCHER_IDX){
        if(a->type.of == TYPE_SPAN){
            return Span_Get((Span *)a, fch->target.val.idx);
        }else if(a->type.of == TYPE_ORDTABLE){
            return Span_Get(((OrdTable *)a)->order, fch->target.val.idx);
        }else if(a->type.of == TYPE_ITER){
            Iter *it = (Iter *)a;
            Iter_GetByIdx(it, fch->target.val.idx);
            if(it->type.state & SUCCESS){
                return Iter_Get((Iter *)a);
            }
        }else{
            Error(ErrStream->m, (Abstract *)fch, FUNCNAME, FILENAME, LINENUMBER,
                "Fetch by idx on this object is not supported", NULL);
            return NULL;
        }
    }else if((fch->target.type.state & (FETCHER_KEY|FETCHER_VALUE)) && fch->key != NULL){
        Hashed *h = NULL;
        if(a->type.of == TYPE_ORDTABLE){
            h = OrdTable_Get((OrdTable *)a, fch->key);
        }else if(a->type.of == TYPE_TABLE){
            h = Table_GetHashed((Table *)a, fch->key);
        }

        if(h != NULL){
            if(fch->target.type.state & FETCHER_KEY){
                return h->key;
            }else if (fch->target.type.state & FETCHER_VALUE){
                return h->value;
            }
        }
    }else if(fch->target.type.state & FETCHER_ITER){
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
    }else if(fch->target.type.state & FETCHER_FUNC){
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

Fetcher *Fetcher_Make(MemCh *m){
    Fetcher *fch = (Fetcher *)MemCh_Alloc(m, sizeof(Fetcher));
    fch->type.of = TYPE_FETCHER;
    fch->targets = Span_Make(m);
    return fch;
}
