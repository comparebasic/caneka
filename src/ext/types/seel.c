#include <external.h>
#include <caneka.h>

Lookup *SeelLookup = NULL;
Lookup *SeelNameLookup = NULL;

i32 Seel_GetIdx(Table *seel, void *key){
    Hashed *h = Table_GetHashed(seel, key);
    if(h != NULL){
        return h->orderIdx;
    }
    return -1;
}

void *Seel_Get(Span *inst, void *key){
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
        return as(Span_Get(inst, h->orderIdx), sg->val.w);
    }
}

status Seel_Seel(MemCh *m, Table *seel, Str *name, cls typeOf){
    seel->type.state |= TABLE_SEALED;
    Lookup_Add(m, SeelLookup, typeOf, seel);
    Lookup_Add(m, SeelNameLookup, typeOf, name);
    Lookup_Add(m, ToStreamLookup, typeOf, Inst_Print);
    return seel->type.state;
}

status Seel_Init(MemCh *m){
    status r = READY;
    if(SeelLookup == NULL){
        SeelLookup = Lookup_Make(m, TYPE_INSTANCE);
        SeelNameLookup = Lookup_Make(m, TYPE_INSTANCE);
        r |= SUCCESS;
    }
    return r;
}
