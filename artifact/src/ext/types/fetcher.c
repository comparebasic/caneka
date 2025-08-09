#include <external.h>
#include <caneka.h>

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
    if(1 || fch->type.state & DEBUG){
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
        FetchTarget *tg = (FetchTarget *)Iter_Get(&it);
        if(tg->type.state & FETCH_TARGET_RESOLVED){
            if(tg->type.state & FETCH_TARGET_ATT){
                printf(">>> Assigning by resolved att\n");
                fflush(stdout);
                value = Fetch_FromOffset(m, value, tg->offset, tg->objType.of);
            }else{
                value = tg->func(m, tg, value, source);
            }
        }else if(tg->type.state & FETCH_TARGET_SELF){
            continue;
        }else{
            cls typeOf = value->type.of;
            if(typeOf == TYPE_WRAPPED_PTR){
                typeOf = ((Single *)value)->objType.of;
            }
            ClassDef *cls = Lookup_Get(ClassLookup, typeOf);
            printf(">>> Fetching Cls %p\n", cls);
            fflush(stdout);
            if(cls != NULL){
                if(tg->type.state & FETCH_TARGET_ATT){
                    printf(">>> Assigning by att\n");
                    fflush(stdout);
                    Single *sg = (Single *)Table_Get(cls->atts, 
                        (Abstract *)tg->key);
                    if(sg != NULL){
                        tg->offset = sg->val.w;
                        value = Fetch_FromOffset(m, value, tg->offset, tg->objType.of);
                        tg->type.state |= FETCH_TARGET_RESOLVED;
                        continue;
                    }
                }else if(tg->type.state & FETCH_TARGET_KEY){
                    printf(">>> Assigning by key\n");
                    fflush(stdout);
                    tg->func = cls->byKey;
                }else if(tg->type.state & FETCH_TARGET_IDX){
                    tg->func = cls->byIdx;
                    printf(">>> Assigning by Idx\n");
                    fflush(stdout);
                }else if(tg->type.state & FETCH_TARGET_ITER){
                    tg->func = cls->getIter;
                    printf(">>> Assigning by Iter\n");
                    fflush(stdout);
                }else{
                    value = NULL;
                    break;
                }
                tg->type.state |= FETCH_TARGET_RESOLVED;
                value = tg->func(m, tg, value, source);
            }else{
                Abstract *args[] = {
                    (Abstract *)Type_ToStr(m, value->type.of),
                    NULL
                };
                Error(m, (Abstract *)fch, FUNCNAME, FILENAME, LINENUMBER,
                    "Error ClassDef not found for $\n", args);
            }
        }
    }

    if(it.type.state & END){
        return value;
    }else{
        return NULL;
    }
}

Fetcher *Fetcher_Make(MemCh *m){
    Fetcher *fch = (Fetcher *)MemCh_Alloc(m, sizeof(Fetcher));
    fch->type.of = TYPE_FETCHER;
    fch->val.targets = Span_Make(m);
    return fch;
}
