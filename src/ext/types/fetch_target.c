#include <external.h>
#include <caneka.h>

status FetchTarget_Resolve(MemCh *m, FetchTarget *tg, cls typeOf){
    ClassDef *cls = NULL;
    void *args[4];
    Str s = {.type = {TYPE_STR, STRING_CONST}, .length = 0, .alloc = 0, .bytes = NULL};
    args[0] = &s;
    if(typeOf & TYPE_OBJECT){
        Lookup_Get(ClassLookup, typeOf);
        if(cls == NULL){
            char *cstr = "cls is NULL";
            i16 len = strlen(cstr);
            Str_Init(&s, (byte *)cstr, len, len+1); 
            goto err;
        }
        if(tg->type.state & FETCH_TARGET_ATT){
            Single *sg = (Single *)Table_Get(cls->atts, tg->key);
            if(sg == NULL){
                char *cstr = "Single for cls->atts is NULL";
                i16 len = strlen(cstr);
                Str_Init(&s, (byte *)cstr, len, len+1); 
                goto err;
            }else{
                tg->offset = sg->val.w;
                tg->type.state |= FETCH_TARGET_RESOLVED;
                tg->type.of = typeOf;
                return SUCCESS;
            }
        }else if(tg->type.state & FETCH_TARGET_PROP){
            tg->idx = Class_GetPropIdx(cls, tg->key);
            if(tg->idx == -1){
                tg->type.state |= FETCH_TARGET_HASH;
                tg->id = Hash_Bytes(tg->key->bytes, tg->key->length);;
            }
            tg->type.state |= FETCH_TARGET_RESOLVED;
            tg->type.of = typeOf;
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
        }

        if(tg->func == NULL){
            char *cstr = "func is NULL";
            i16 len = strlen(cstr);
            Str_Init(&s, (byte *)cstr, len, len+1); 
            goto err;
        }

        tg->type.state |= FETCH_TARGET_RESOLVED;
        tg->type.of = typeOf;
        return SUCCESS;
    } else {
        if(typeOf == TYPE_TABLE && (tg->type.state & FETCH_TARGET_PROP)){
            tg->type.state |= FETCH_TARGET_HASH;
            tg->id = Hash_Bytes(tg->key->bytes, tg->key->length);;
        }
        if(tg->type.state & FETCH_TARGET_ATT){
           Map *map = Map_Get(typeOf);
           if(map == NULL){
                goto err;
           }else{
                RangeType *att = Table_Get(map->tbl, tg->key); 
                tg->offset = att->range;
                return SUCCESS;
           }
        }else{
            goto err;
        }

        tg->type.state |= FETCH_TARGET_RESOLVED;
        tg->type.of = typeOf;
        return SUCCESS;
    }
err:
    args[1] = tg;
    args[2] = Type_ToStr(m, typeOf);
    args[3] = I16_Wrapped(m, typeOf);
    Error(m, FUNCNAME, FILENAME, LINENUMBER,
        "Error @ resolving $ against type $/$\n", args);
    return ERROR;
}

void *Fetch_Target(MemCh *m, FetchTarget *tg, void *_value, void *source){
    Abstract *value = (Abstract *)_value;
    void *args[6];
    /*
    args[0] = Type_StateVec(m, tg->type.of, tg->type.state);
    args[1] = NULL;
    Out("^c.Fetch_Target @^0\n", args);
    */

    args[3] = NULL;
    ClassDef *cls = NULL;
    word typeOf = value->type.of;
    if(typeOf & TYPE_OBJECT){
        typeOf = ((Object *)value)->type.of;
    }
    if((tg->type.state & FETCH_TARGET_RESOLVED) &&
            Object_TypeMatch(value, tg->type.of)){
        if(tg->type.state & FETCH_TARGET_ATT){
            return Fetch_FromOffset(m, value, tg->offset, tg->type.of);
        }else if(tg->type.state & FETCH_TARGET_HASH){
            void *a = NULL; 
            if(value->type.of & TYPE_OBJECT){
                Object *obj = (Object *)as(value, TYPE_OBJECT);
                a = Object_Get(obj, tg->key);
            }else if(value->type.of == TYPE_TABLE){
                a = Table_Get((Table *)value, tg->key);
            }
            if(a == NULL){
                args[1] = value;
                goto err;
            }
            return a;
        }else if(tg->type.state & FETCH_TARGET_PROP){
            Object *obj = (Object *)as(value, TYPE_OBJECT);
            Abstract *a = Object_GetPropByIdx(obj, tg->idx);
            if(a == NULL){
                args[1] = value;
                goto err;
            }
            return a;
        }else{
            return tg->func(m, tg, value, source);
        }
    }else{
        if(FetchTarget_Resolve(m, tg, typeOf) & SUCCESS){
            return Fetch_Target(m, tg, value, source);
        }else{
            args[1] = tg;
            goto err;
        }
    }
err:
    if((tg->type.state & PROCESSING) == 0){
        cls = Lookup_Get(ClassLookup, value->type.of);
        args[0] = tg;

        args[1] = (value != NULL ? Type_ToStr(m, value->type.of) : NULL);
        args[3] = Type_ToStr(m, typeOf);
        args[4] = cls;
        args[5] = NULL;
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error for @/$ ClassDef X or prop not found, using @ class $/@\n", args);
    }
    return NULL;
}

void *Fetch_ByKey(MemCh *m, void *a, Str *key, void *source){
    FetchTarget *tg = FetchTarget_MakeKey(m, key);
    return Fetch_Target(m, tg, a, source);
}

void *Fetch_ByAtt(MemCh *m, void *a, Str *att, void *source){
    FetchTarget *tg = FetchTarget_MakeAtt(m, att);
    return Fetch_Target(m, tg, a, source);
}

void *Fetch_Prop(MemCh *m, void *a, Str *prop, void *source){
    FetchTarget *tg = FetchTarget_MakeProp(m, prop);
    return Fetch_Target(m, tg, a, source);
}

Iter *Fetch_Iter(MemCh *m, void *a, void *source){
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
