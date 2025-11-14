#include <external.h>
#include <caneka.h>

status Inst_ShowKeys(Buff *bf, Inst *inst, i32 childrenIdx, i32 indent){
    Table *tbl = Span_Get(inst, childrenIdx);
    Iter it;
    void *args[3];
    status r = READY;
    Iter_Init(&it, tbl);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = (Hashed *)Iter_Get(&it); 
        if(h != NULL){
            r |= SUCCESS;
            i32 id = indent;
            while(id--){
                Buff_AddBytes(bf, (byte *)"  ", 2);
            }
            args[0] = h->key;
            args[1] = (h->value == NULL ?
                NULL : Type_ToStr(bf->m,((Abstract *)h->value)->type.of));
            args[2] = NULL;
            Fmt(bf, " -> @=$\n", args);
            if(h->value != NULL && ((Abstract *)h->value)->type.of == inst->type.of){
                Inst_ShowKeys(bf, h->value, childrenIdx, indent+1);
            }
        }
    }
    return r;
}

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
                Span_Set(inst, h->orderIdx, Span_Make(m));
            }else if(sg->val.w == TYPE_TABLE){
                Span_Set(inst, h->orderIdx, Table_Make(m));
            }else if(sg->val.w & TYPE_INSTANCE){
                Span_Set(inst, h->orderIdx, Inst_Make(m, sg->val.w));
            }
        }
    }
    inst->type.of = typeOf;
    return inst;
}
