#include <external.h>
#include <caneka.h>

i32 Inst_Set(Span *inst, Table *seel, void *key, void *value){
    if((inst->type.state & TABLE_SEALED) == 0){
        Error(seel->m, FUNCNAME, FILENAME, LINENUMBER,
            "Error unable to map values from an unseeled Table", NULL);
        return -1;
    }
    i32 orderIdx = Seel_GetIdx(seel, key);
    if(orderIdx < 0){
        Error(inst->m, FUNCNAME, FILENAME, LINENUMBER,
            "Error unable to map values from an unseeled Table", NULL);
        return -1;
    }
    Span_Set(inst, orderIdx, value);
    return orderIdx;
}

Span *Inst_Make(MemCh *m, cls typeOf){
    Table *seel = Lookup_Get(SeelLookup, typeOf);
    if(seel == NULL){
        void *args[] = {Type_ToStr(m, typeOf), NULL};
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error Seel not found $", args);
        return NULL;

    }
    Span *inst = Span_Make(m);
    Iter it;
    Iter_Init(&it, seel);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = (Hashed *)Iter_Get(&it);
        if(h != NULL){
            Single *sg = (Single *)h->value;
            if(sg->val.w == TYPE_SPAN){
                Span_Set(inst, it.idx, Span_Make(m));
            }else if(sg->val.w == TYPE_TABLE){
                Span_Set(inst, it.idx, Table_Make(m));
            }else if(sg->val.w & TYPE_INSTANCE){
                Span_Set(inst, it.idx, Inst_Make(m, sg->val.w));
            }
        }
    }
    inst->type.of = typeOf;
    return inst;
}
