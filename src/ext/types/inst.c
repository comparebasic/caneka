#include <external.h>
#include <caneka.h>

i32 Inst_Set(Span *inst, Table *seal, void *key, void *value){
    if((inst->type.state & TABLE_SEALED) == 0){
        Error(seal->m, FUNCNAME, FILENAME, LINENUMBER,
            "Error unable to map values from an unsealed Table", NULL);
        return -1;
    }
    i32 orderIdx = Seal_GetIdx(seal, key);
    if(orderIdx < 0){
        Error(inst->m, FUNCNAME, FILENAME, LINENUMBER,
            "Error unable to map values from an unsealed Table", NULL);
        return -1;
    }
    Span_Set(inst, orderIdx, value);
    return orderIdx;
}

Span *Inst_Make(MemCh *m, cls typeOf){
    Table *seal = Lookup_Get(SealLookup, typeOf);
    if(seal == NULL){
        void *args[] = {Type_ToStr(m, typeOf), NULL};
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error Seal not found $", args);
        return NULL;

    }
    Span *inst = Span_Make(m);
    Iter it;
    Iter_Init(&it, seal);
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
