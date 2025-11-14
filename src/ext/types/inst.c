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

Table *Inst_GetOrMake(Table *tbl, void *key, word op, i32 childrenIdx, cls typeOf){
    Abstract *a = (Abstract *)Table_Get(tbl, key);
    if(a == NULL){
        if((op & (SPAN_OP_SET|SPAN_OP_RESERVE)) == 0){
            return NULL;
        }
        Inst *new = Inst_Make(tbl->m, typeOf);
        Table_Set(tbl, key, new);
        return new;
    }else if(a->type.of == typeOf){
        return (Inst *)a;
    }else{
        void *args[] = {
            Type_ToStr(tbl->m, a->type.of),
            key,
            a,
            NULL
        };
        Error(tbl->m, FUNCNAME, FILENAME, LINENUMBER,
            "Trying to make an empty Table where a $ value already exists for"
            " key @, or is not an object @", args);
        return NULL;
    }
}

void *Inst_ByPath(Span *inst, i32 childrenIdx, StrVec *path, void *value, word op){
    DebugStack_Push(inst, inst->type.state);

    cls typeOf = inst->type.of;

    if(inst->type.state & DEBUG){
        void *args[] = {
            path,
            NULL
        };
        Out("^p.ByPath @^0\n", args);
        Inst_ShowKeys(OutStream, inst, childrenIdx, 0);
    }

    Iter keysIt;
    Iter_Init(&keysIt, path->p);

    Str *key = NULL;
    Inst *current = inst;
    Table *tbl = Span_Get(inst, childrenIdx);
    while((Iter_Next(&keysIt) & END) == 0){
        Str *item = (Str *)Iter_Get(&keysIt);
        if((item->type.state & MORE) && key != NULL){
            current = Inst_GetOrMake(Span_Get(current, childrenIdx),
                key, op, childrenIdx, typeOf);
            if(current == NULL){
                DebugStack_Pop();
                return NULL;
            }
            if(inst->type.state & DEBUG){
                void *args[] = {
                    current,
                    key,
                    NULL
                };
                Out("^p.    While @[@]^0\n", args);
            }
            key = NULL;
        }else{
            key = item;
            if((keysIt.type.state & LAST) && (op & SPAN_OP_SET)){
                Table_Set(Span_Get(current, childrenIdx), key, value);
            }
        }
    }

    if(key != NULL && (key->type.state & (LAST|MORE)) == 0){
        if(inst->type.state & DEBUG){
            void *args[] = {
                current,
                key,
                NULL
            };
            Out("^p.    BelowLast @[@]^0\n", args);
        }

        current = Table_Get(Span_Get(current, childrenIdx), key);
        if(current == NULL){
            if(SPAN_OP_SET){
                Table_Set(Span_Get(current, childrenIdx), key, value);
            }else{
                DebugStack_Pop();
                return NULL;
            }
        }
    }

    if(inst->type.state & DEBUG){
        void *args[] = {
            current,
            NULL
        };
        Out("^c.   Returning @^0\n", args);
    }

    DebugStack_Pop();
    return current;
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
