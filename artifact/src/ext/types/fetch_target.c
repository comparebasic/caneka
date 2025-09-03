#include <external.h>
#include <caneka.h>

status FetchTarget_Resolve(MemCh *m, FetchTarget *tg, cls typeOf){
    ClassDef *cls = Lookup_Get(ClassLookup, typeOf);
    Abstract *args[4];
    if(cls != NULL){
        if(tg->type.state & FETCH_TARGET_ATT){
            Single *sg = (Single *)Table_Get(cls->atts, 
                (Abstract *)tg->key);
            if(sg == NULL){
                goto err;
            }else{
                tg->offset = sg->val.w;
                tg->type.state |= FETCH_TARGET_RESOLVED;
                tg->objType.of = typeOf;
                return SUCCESS;
            }
        }else if(tg->type.state & FETCH_TARGET_PROP){
            tg->idx = Class_GetPropIdx(cls, tg->key);
            if(tg->idx == -1){
                goto err;
            }
            tg->type.state |= FETCH_TARGET_RESOLVED;
            tg->objType.of = typeOf;
            return SUCCESS;
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
        tg->objType.of = typeOf;
        return SUCCESS;
    }
err:
    args[0] = (Abstract *)Type_ToStr(m, typeOf);
    args[1] = (Abstract *)tg;
    args[2] = (Abstract *)cls;
    args[3] = NULL;
    Error(m, (Abstract *)tg, FUNCNAME, FILENAME, LINENUMBER,
        "Error resolving ClassDef $ for prop or att $ using class @\n", args);
    return ERROR;
}

Abstract *Fetch_Target(MemCh *m, FetchTarget *tg, Abstract *value, Abstract *source){
    Abstract *args[5];
    ClassDef *cls = NULL;
    word typeOf = ZERO;
    if((tg->type.state & FETCH_TARGET_RESOLVED) &&
            Object_TypeMatch(value, tg->objType.of)){
        if(tg->type.state & FETCH_TARGET_ATT){
            return Fetch_FromOffset(m, value, tg->offset, tg->objType.of);
        }else if(tg->type.state & FETCH_TARGET_PROP){
            Object *obj = (Object *)as(value, TYPE_OBJECT);
            Abstract *a = Object_GetPropByIdx(obj, tg->idx);
            if(a == NULL){
                goto err;
            }
            return a;
        }else{
            return tg->func(m, tg, value, source);
        }
    }else{
        if(FetchTarget_Resolve(m, tg, value->type.of) & SUCCESS){
            return Fetch_Target(m, tg, value, source);
        }else{
            goto err;
        }
    }
err:
    cls = Lookup_Get(ClassLookup, value->type.of);
    args[0] = (Abstract *)(value != NULL ? Type_ToStr(m, value->type.of) : NULL);
    args[1] = (Abstract *)tg;
    args[2] = (Abstract *)Type_ToStr(m, typeOf);
    args[3] = (Abstract *)cls;
    args[4] = NULL;
    Error(m, (Abstract *)tg, FUNCNAME, FILENAME, LINENUMBER,
        "Error ClassDef X or prop not found for $ using @ class $/@\n", args);
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

Abstract *Fetch_Prop(MemCh *m, Abstract *a, Str *prop, Abstract *source){
    FetchTarget *tg = FetchTarget_MakeProp(m, prop);
    return Fetch_Target(m, tg, a, source);
}

Iter *Fetch_Iter(MemCh *m, Abstract *a, Abstract *source){
    FetchTarget *tg = FetchTarget_MakeIter(m);
    return (Iter *)Fetch_Target(m, tg, a, source);
}

FetchTarget *FetchTarget_Make(MemCh *m){
    FetchTarget *tg = (FetchTarget *)MemCh_Alloc(m, sizeof(FetchTarget));
    tg->type.of = TYPE_FETCH_TARGET;
    tg->idx = -1;
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

FetchTarget *FetchTarget_MakeProp(MemCh *m, Str *prop){
    FetchTarget *tg = FetchTarget_Make(m);
    tg->type.state = FETCH_TARGET_PROP;
    tg->key = prop;
    return tg;
}
