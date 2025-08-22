#include <external.h>
#include <caneka.h>

Abstract *Fetch_Target(MemCh *m, FetchTarget *tg, Abstract *value, Abstract *source){
    Abstract *args[5];
    ClassDef *cls = NULL;
    word typeOf = ZERO;
    if((tg->type.state & FETCH_TARGET_RESOLVED) && Object_TypeMatch(value, (ObjType *)tg)){
        if(tg->type.state & FETCH_TARGET_ATT){
            return Fetch_FromOffset(m, value, tg->offset, tg->objType.of);
        if(tg->type.state & FETCH_TARGET_PROP){
            Hashed *h = Object_GetPropByIdx(value, tg->idx);
            return h->value;
        }else{
            return tg->func(m, tg, value, source);
        }
    }else if(tg->type.state & FETCH_TARGET_SELF){
        return value;
    }else{
        typeOf = value->type.of;
        if(typeOf == TYPE_WRAPPED_PTR){
            typeOf = ((Single *)value)->objType.of;
        }
        cls = Lookup_Get(ClassLookup, typeOf);
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
            if(tg->type.state & FETCH_TARGET_PROP){
                tg->idx = Object_GetPropIdx(cls, tg->key);
                Hashed *h = Object_GetPropByIdx(value, tg->idx);
                if(h == NULL){
                    goto err;
                }
                tg->type.state |= FETCH_TARGET_RESOLVED;
                return h->value;
            }else if(tg->type.state & FETCH_TARGET_KEY){
                tg->func = cls->api.byKey;
                tg->type.state |= FETCH_TARGET_FUNC;
            }else if(tg->type.state & FETCH_TARGET_IDX){
                tg->func = cls->api.byIdx;
                tg->type.state |= FETCH_TARGET_FUNC;
            }else if(tg->type.state & FETCH_TARGET_ITER){
                tg->func = cls->api.getIter;
                tg->type.state |= FETCH_TARGET_FUNC;
            }else{
                goto err;
            }
            if(tg->func == NULL){
                goto err;
            }

            tg->type.state |= FETCH_TARGET_RESOLVED;
            return tg->func(m, tg, value, source);
        }else{
err:
            args[0] = (Abstract *)(value != NULL ? Type_ToStr(m, value->type.of) : NULL);
            args[1] = (Abstract *)tg;
            args[2] = (Abstract *)Type_ToStr(m, typeOf);
            args[3] = (Abstract *)cls;
            args[4] = NULL;
            Error(m, (Abstract *)tg, FUNCNAME, FILENAME, LINENUMBER,
                "Error ClassDef X or prop not found for $ using @ class $/@\n", args);
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

Abstract *Fetch_Method(MemCh *m, Abstract *a, Str *method, Abstract *source){
    FetchTarget *tg = FetchTarget_MakeMethod(m, method);
    return Fetch_Target(m, tg, a, source);
}

Iter *Fetch_Iter(MemCh *m, Abstract *a, Abstract *source){
    FetchTarget *tg = FetchTarget_MakeIter(m);
    return (Iter *)Fetch_Target(m, tg, a, source);
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

FetchTarget *FetchTarget_MakeMethod(MemCh *m, Str *method){
    FetchTarget *tg = FetchTarget_Make(m);
    tg->type.state = FETCH_TARGET_METHOD;
    tg->key = method;
    return tg;
}
