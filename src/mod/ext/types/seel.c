#include <external.h>
#include <caneka.h>

Lookup *SeelLookup = NULL;
Lookup *SeelOrdLookup = NULL;
Lookup *SeelNameLookup = NULL;
Table *SeelByName = NULL;

i32 Seel_GetIdx(Table *seel, void *key){
    Hashed *h = Table_GetHashed(seel, key);
    if(h != NULL){
        return h->orderIdx;
    }
    return -1;
}

i32 Seel_TypeByName(void *name){
    Single *sg = Table_Get(SeelByName, name);
    if(sg != NULL){
        return sg->val.w;
    }
    return ZERO;
}

status Seel_SetKv(Span *inst, Str *prop, void *key, void *value){
    Table *tbl = Seel_Get(inst, prop);
    if(tbl != NULL && tbl->type.of == TYPE_TABLE){
        return Table_Set(tbl, key, value);
    }
    return NOOP;
}

status Seel_Set(Span *inst, void *key, void *value){
    Table *seel = Lookup_Get(SeelLookup, inst->type.of);
    void *args[3];
    if(seel == NULL){
        args[0] = Type_ToStr(inst->m, inst->type.of);
        args[1] = NULL;
        Error(inst->m, FUNCNAME, FILENAME, LINENUMBER,
            "Seel not found for type $", args);
        return ERROR;
    }
    Hashed *h = Table_GetHashed(seel, key);
    if(h == NULL){
        args[0] = key;
        args[1] = seel;
        args[2] = NULL;
        Error(inst->m, FUNCNAME, FILENAME, LINENUMBER,
            "Seel prop not found key: @ for @", args);
        return ERROR;
    }

    return Span_Set(inst, h->orderIdx, value);
}

void *Seel_Get(Span *inst, void *key){
    if(inst == NULL){
        return NULL;
    }
    Table *seel = Lookup_Get(SeelLookup, inst->type.of);
    void *args[2];
    if(seel == NULL){
        args[0] = Type_ToStr(inst->m, inst->type.of);
        args[1] = NULL;
        Error(inst->m, FUNCNAME, FILENAME, LINENUMBER,
            "Seel not found for type $", args);
        return NULL;
    }
    Hashed *h = Table_GetHashed(seel, key);
    if(h == NULL){
        args[0] = key;
        args[1] = NULL;
        Error(inst->m, FUNCNAME, FILENAME, LINENUMBER,
            "Seel prop not found key: @", args);
        return NULL;
    }
    Single *sg = (Single *)h->value;
    if(sg->val.w == TYPE_ABSTRACT){
        return Span_Get(inst, h->orderIdx);
    }else{
        Abstract *value = Span_Get(inst, h->orderIdx);
        if(value != NULL){
            Ifc(inst->m, value, sg->val.w);
        }
        return value;
    }
}

status Seel_Seel(MemCh *m, Table *seel, Str *name, cls typeOf){
    seel->type.state |= TABLE_SEALED;
    Lookup_Add(m, SeelLookup, typeOf, seel);
    Lookup_Add(m, SeelOrdLookup, typeOf, Table_Ordered(m, seel));
    Lookup_Add(m, SeelNameLookup, typeOf, name);
    Table_Set(SeelByName, name, I16_Wrapped(m, typeOf));
    Lookup_Add(m, ToStreamLookup, typeOf, Inst_Print);
    return seel->type.state;
}

Table *Seel_GetSeel(MemCh *m, cls instOf){
    Table *seel = Lookup_Get(SeelLookup, instOf);
    if(seel == NULL){
        void *args[] = {
            Type_ToStr(m, instOf),
            NULL
        };
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Seel not found for type $", args);
        return NULL;
    }
    return seel;
}

Span *Seel_OrdSeel(MemCh *m, seelType instOf){
    Span *ord = Lookup_Get(SeelOrdLookup, instOf);
    if(ord == NULL){
        void *args[] = {
            Type_ToStr(m, instOf),
            NULL
        };
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Seel not found for type $", args);
        return NULL;
    }
    return ord;
}

status Seel_Init(MemCh *m){
    status r = READY;
    if(SeelLookup == NULL){
        SeelLookup = Lookup_Make(m, TYPE_INSTANCE);
        SeelNameLookup = Lookup_Make(m, TYPE_INSTANCE);
        SeelOrdLookup = Lookup_Make(m, TYPE_INSTANCE);
        SeelByName = Table_Make(m);
        r |= SUCCESS;
    }
    return r;
}
