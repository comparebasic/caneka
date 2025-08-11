#include <external.h>
#include <caneka.h>

static inline Abstract *Fetch_target(MemCh *m, FetchTarget *tg, Abstract *value, Abstract *source){
    if(tg->type.state & FETCH_TARGET_RESOLVED){
        if(tg->type.state & FETCH_TARGET_ATT){
            value = Fetch_FromOffset(m, value, tg->offset, tg->objType.of);
        }else{
            value = tg->func(m, tg, value, source);
        }
    }else if(tg->type.state & FETCH_TARGET_SELF){
        return value;
    }else{
        cls typeOf = value->type.of;
        if(typeOf == TYPE_WRAPPED_PTR){
            typeOf = ((Single *)value)->objType.of;
        }
        ClassDef *cls = Lookup_Get(ClassLookup, typeOf);
        if(cls != NULL){
            if(tg->type.state & FETCH_TARGET_ATT){
                Single *sg = (Single *)Table_Get(cls->atts, 
                    (Abstract *)tg->key);
                if(sg != NULL){
                    tg->offset = sg->val.w;
                    value = Fetch_FromOffset(m, value, tg->offset, tg->objType.of);
                    tg->type.state |= FETCH_TARGET_RESOLVED;
                    return value;
                }
            }else if(tg->type.state & FETCH_TARGET_KEY){
                tg->func = cls->byKey;
                tg->type.state |= FETCH_TARGET_FUNC;
            }else if(tg->type.state & FETCH_TARGET_IDX){
                tg->func = cls->byIdx;
                tg->type.state |= FETCH_TARGET_FUNC;
            }else if(tg->type.state & FETCH_TARGET_ITER){
                tg->func = cls->getIter;
                tg->type.state |= FETCH_TARGET_FUNC;
            }else{
                goto err;
            }
            tg->type.state |= FETCH_TARGET_RESOLVED;
            value = tg->func(m, tg, value, source);
        }else{
            Abstract *args[] = {
                (Abstract *)Type_ToStr(m, value->type.of),
                NULL
            };
err:
            Error(m, (Abstract *)tg, FUNCNAME, FILENAME, LINENUMBER,
                "Error ClassDef not found for $\n", args);
            return NULL;
        }
    }
    return NULL;
}

Abstract *Fetch_FromOffset(MemCh *m, Abstract *a, i16 offset, cls typeOf){
    Abstract *value = NULL;
    if(typeOf == ZERO || typeOf > _TYPE_RAW_END){
        void **ptr = (void **)(((void *)a)+offset);
        value = *ptr; 
    }else{
        if(typeOf == TYPE_UTIL){
            util *ptr = ((void *)a)+offset;
            value = (Abstract *)Util_Wrapped(m, *ptr); 
        }else if(typeOf == TYPE_I32){
            i32 *ptr = ((void *)a)+offset;
            value = (Abstract *)I32_Wrapped(m, *ptr); 
        }else if(typeOf == TYPE_I64){
            i64 *ptr = ((void *)a)+offset;
            value = (Abstract *)I64_Wrapped(m, *ptr); 
        }else if(typeOf == TYPE_I16){
            i16 *ptr = ((void *)a)+offset;
            value = (Abstract *)I16_Wrapped(m, *ptr);
        }else if(typeOf == TYPE_I8){
            i8 *ptr = ((void *)a)+offset;
            value = (Abstract *)I16_Wrapped(m, *ptr);
        }else if(typeOf == TYPE_BYTE){
            byte *ptr = ((void *)a)+offset;
            value = (Abstract *)B_Wrapped(m, *ptr, ZERO, ZERO); 
        }
    }
    return value;
}

Abstract *Fetch(MemCh *m, Fetcher *fch, Abstract *value, Abstract *source){
    if(fch->type.state & DEBUG){
        Abstract *args[] = {
            (Abstract *)fch,
            (Abstract *)value,
            NULL,
        };
        Out("^c.Fetch & from @^0.\n", args);
    }
    Iter it;
    Iter_Init(&it, fch->val.targets);
    while(value != NULL && (Iter_Next(&it) & END) == 0){
        value = Fetch_target(m, (FetchTarget *)Iter_Get(&it), value, source);
    }

    if(it.type.state & END){
        return value;
    }else{
        return NULL;
    }
}

Abstract *Fetch_ByKey(MemCh *m, Abstract *a, Str *key, Abstract *source){
    FetchTarget *tg = FetchTarget_MakeKey(m, key);
    return Fetch_target(m, tg, a, source);
}

Abstract *Fetch_ByAtt(MemCh *m, Abstract *a, Str *att, Abstract *source){
    FetchTarget *tg = FetchTarget_MakeAtt(m, att);
    return Fetch_target(m, tg, a, source);
}

Abstract *Fetch_Iter(MemCh *m, Abstract *a, Abstract *source){
    FetchTarget *tg = FetchTarget_MakeIter(m);
    return Fetch_target(m, tg, a, source);
}

Fetcher *Fetcher_Make(MemCh *m){
    Fetcher *fch = (Fetcher *)MemCh_Alloc(m, sizeof(Fetcher));
    fch->type.of = TYPE_FETCHER;
    fch->val.targets = Span_Make(m);
    return fch;
}

FetchTarget *FetchTarget_Make(MemCh *m){
    FetchTarget *tg = (FetchTarget *)MemCh_Alloc(m, sizeof(FetchTarget));
    tg->type.of = TYPE_FETCH_TARGET;
    return tg;
}

FetchTarget *FetchTarget_MakeKey(MemCh *m, Str *key){
    FetchTarget *tg = FetchTarget_Make(m);
    tg->type.state = FETCH_TARGET_KEY;
    tg->key = key;
    return tg;
}

FetchTarget *FetchTarget_MakeAtt(MemCh *m, Str *att){
    FetchTarget *tg = FetchTarget_Make(m);
    tg->type.state = FETCH_TARGET_ATT;
    tg->key = att;
    return tg;
}

FetchTarget *FetchTarget_MakeFunc(MemCh *m, Str *key){
    FetchTarget *tg = FetchTarget_Make(m);
    tg->type.state = FETCH_TARGET_FUNC;
    tg->key = key;
    return tg;
}

FetchTarget *FetchTarget_MakeIter(MemCh *m){
    FetchTarget *tg = FetchTarget_Make(m);
    tg->type.state = FETCH_TARGET_ITER;
    return tg;
}

FetchTarget *FetchTarget_MakeIdx(MemCh *m, i32 idx){
    FetchTarget *tg = FetchTarget_Make(m);
    tg->type.state = FETCH_TARGET_IDX;
    tg->idx = idx;
    return tg;
}

