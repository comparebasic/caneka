#include <external.h>
#include <caneka.h>

static void *Fetch_byIdx(MemCh *m, FetchTarget *tg, void *data){
    return Span_Get((Span *)data, tg->idx);
}

static void *Fetch_byKey(MemCh *m, FetchTarget *tg, void *data){
    return Table_Get((Table *)data, tg->key);
}

static void *Fetch_getIter(MemCh *m, FetchTarget *tg, void *data){
    return Iter_Make(m, data);
}

status FetchTarget_Resolve(MemCh *m, FetchTarget *tg, cls typeOf){
    void *args[4];
    Str s = {.type = {TYPE_STR, STRING_CONST}, .length = 0, .alloc = 0, .bytes = NULL};
    args[0] = &s;
    if(typeOf & TYPE_INSTANCE){
        Table *seel = Lookup_Get(SeelLookup, typeOf);
        if(seel == NULL){
            char *cstr = "seel is NULL";
            i16 len = strlen(cstr);
            Str_Init(&s, (byte *)cstr, len, len+1); 
            goto err;
        }

        if((tg->type.state & FETCH_TARGET_PROP) &&
            ((tg->idx = Seel_GetIdx(seel, tg->key)) != -1)
        ){
            /* all set */
        }else if(tg->type.state & FETCH_TARGET_ITER){
            tg->func = Fetch_getIter;
        }else{
            char *cstr = "seel prop not found";
            i16 len = strlen(cstr);
            Str_Init(&s, (byte *)cstr, len, len+1); 
            goto err;
        }

        tg->type.state |= FETCH_TARGET_RESOLVED;
        tg->objType.of = typeOf;
        return SUCCESS;
    } else {
        if(typeOf == TYPE_TABLE && (tg->type.state & FETCH_TARGET_PROP)){
            tg->id = Hash_Bytes(tg->key->bytes, tg->key->length);
            tg->func = Fetch_byKey;
        }else if(typeOf == TYPE_TABLE && (tg->type.state & FETCH_TARGET_KEY)){
            tg->id = Hash_Bytes(tg->key->bytes, tg->key->length);;
            tg->func = Fetch_byKey;
        }else if(tg->type.state & FETCH_TARGET_ATT){
           Map *map = Map_Get(typeOf);
           if(map == NULL){
                char *cstr = "Map is NULL";
                i16 len = strlen(cstr);
                Str_Init(&s, (byte *)cstr, len, len+1); 
                goto err;
           }else{
                RangeType *att = Table_Get(map->tbl, tg->key); 
                if(att == NULL){
                    char *cstr = "att is NULL";
                    i16 len = strlen(cstr);
                    Str_Init(&s, (byte *)cstr, len, len+1); 
                    goto err;
                }
                tg->offsetType = att;
           }
        }else if(tg->type.state & FETCH_TARGET_ITER && 
            (typeOf == TYPE_TABLE || typeOf == TYPE_SPAN)
        ){
            tg->func = Fetch_getIter;
        }else{
            char *cstr = "non seel is NULL";
            i16 len = strlen(cstr);
            Str_Init(&s, (byte *)cstr, len, len+1); 
            goto err;
        }

        tg->type.state |= FETCH_TARGET_RESOLVED;
        tg->objType.of = typeOf;
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
    args[4] = NULL;

    Table *seel = NULL;
    word typeOf = value->type.of;

    if(value->type.of != tg->objType.of || (tg->type.state & FETCH_TARGET_RESOLVED) == 0){
        if(FetchTarget_Resolve(m, tg, typeOf) & SUCCESS){
            return Fetch_Target(m, tg, value, source);
        }else{
            args[4] = tg;
            goto err;
        }
    }else{
        if(typeOf == TYPE_TABLE && (tg->type.state & FETCH_TARGET_PROP)){
            return tg->func(m, tg, value);
        }else if(tg->type.state & FETCH_TARGET_PROP){
            Inst *obj = (Inst *)value;
            if(obj == NULL || (obj->type.of & TYPE_INSTANCE) == 0){
                args[4] = value;
                goto err;
            }
            void *a = Span_Get(obj, tg->idx);
            if(a == NULL){
                args[4] = value;
                goto err;
            }
            return a;
        }else if(tg->type.state & FETCH_TARGET_ATT){
            return Map_FromOffset(m, value, tg->offsetType->range, tg->offsetType->of);
        }else{
            return tg->func(m, tg, value);
        }
    }
err:
    if((tg->type.state & PROCESSING) == 0){
        seel = Lookup_Get(SeelLookup, value->type.of);
        args[0] = tg;

        args[1] = (value != NULL ? Type_ToStr(m, value->type.of) : NULL);
        args[2] = Type_ToStr(m, typeOf);
        args[3] = seel;
        args[5] = NULL;
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error for @/$ not found, using @ seel $/@\n", args);
    }
    return NULL;
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
