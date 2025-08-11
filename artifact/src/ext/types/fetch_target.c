#include <external.h>
#include <caneka.h>

Abstract *Fetch_Target(MemCh *m, FetchTarget *tg, Abstract *value, Abstract *source){
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

Abstract *Fetch_ByKey(MemCh *m, Abstract *a, Str *key, Abstract *source){
    FetchTarget *tg = FetchTarget_MakeKey(m, key);
    return Fetch_Target(m, tg, a, source);
}

Abstract *Fetch_ByAtt(MemCh *m, Abstract *a, Str *att, Abstract *source){
    FetchTarget *tg = FetchTarget_MakeAtt(m, att);
    return Fetch_Target(m, tg, a, source);
}

Abstract *Fetch_Iter(MemCh *m, Abstract *a, Abstract *source){
    FetchTarget *tg = FetchTarget_MakeIter(m);
    return Fetch_Target(m, tg, a, source);
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
