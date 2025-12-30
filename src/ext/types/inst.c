#include <external.h>
#include <caneka.h>

Inst *asInst(MemCh *m, void *a){
    if(a == NULL){
        return NULL;
    }else if((((Abstract *)a)->type.of & TYPE_INSTANCE) == 0){
        void *args[] = {Type_ToStr(m, ((Abstract *)a)->type.of), NULL};
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Type is not an Inst $", args);
    }
    return (Inst *)a;
}

void *Inst_Att(Inst *inst, void *key){
    if(inst == NULL){
        return NULL;
    }
    Table *obj = (Table *)Span_Get(inst, INST_PROPIDX_ATTS);
    if(obj != NULL){
        return Table_Get(obj, key);
    }
    return NULL;
}

Table *Inst_GetTblOfAtt(Inst *inst, void *key){
    if(inst == NULL){
        return NULL;
    }

    Table *tbl = Inst_Att(inst, key);
    if(tbl == NULL){
        tbl = Table_Make(inst->m);
        Inst_SetAtt(inst, key, tbl);
    }
    return tbl;
}

status Inst_SetAtt(Inst *inst, void *key, void *value){
    if(inst == NULL){
        return ERROR;
    }
    Table *obj = (Table *)Span_Get(inst, INST_PROPIDX_ATTS);
    if(obj != NULL){
        return Table_Set(obj, key, value);
    }
    return ERROR;
}

void *Inst_GetChild(Inst *inst, void *key){
    if(inst == NULL){
        return NULL;
    }
    Table *children = (Table *)Span_Get(inst, INST_PROPIDX_CHILDREN);
    return Table_Get(children, key);
}

status Inst_SetSelected(Span *inst, Span *coords){
    return ZERO;
}

void *Inst_ByPath(Span *inst, StrVec *path, void *value, word op, Span *coords){
    DebugStack_Push(inst, inst->type.state);
    void *args[5];

    cls typeOf = inst->type.of;

    if((path->type.state & STRVEC_PATH) == 0 && path->p->nvalues > 1){
        void *args[] = {inst, NULL};
        Error(inst->m, FUNCNAME, FILENAME, LINENUMBER,
            "Error Inst_ByPath requires a strvec path", args);
        return NULL;
    }

    if(coords != NULL){
        Span_Wipe(coords);
    }
    i32 coordIdx = 0;
    Inst *current = inst;
    Abstract *prev = NULL;

    Iter it;
    Iter_Init(&it, path->p);
    while((Iter_Next(&it) & END) == 0){
        Abstract *token = Iter_Get(&it);
        if(it.type.state & LAST){
            Abstract *key = ((token->type.state & MORE) && it.idx > 0) ? prev : token;
            Table *children = Span_Get(current, INST_PROPIDX_CHILDREN);
            Abstract *child = Table_Get(children, key);

            if(op == SPAN_OP_SET){
                if(coords != NULL){
                    Span_Set(coords, coordIdx, I32_Wrapped(m, children->nvalues));
                    coordIdx++;
                }
                Table_Set(children, key, value); 
                DebugStack_Pop();
                return value;
            }else{
                DebugStack_Pop();
                return child;
            }
        }else if(token->type.state & MORE){
            if(prev){
                Table *children = Span_Get(current, INST_PROPIDX_CHILDREN);
                Abstract *child = Table_Get(children, prev);
                if(op == SPAN_OP_SET){
                    if(child == NULL){
                        Inst *new = Inst_Make(inst->m, typeOf);
                        Table_Set(children, prev, new); 
                        current = new;
                    }else if(child->type.of == typeOf){
                        current = (Inst *)child;
                    }else{
                        args[0] = prev;
                        args[1] = NULL;
                        Error(inst->m, FUNCNAME, FILENAME, LINENUMBER,
                            "The key slot is occupied by a different type,"
                            " conflict, cannot insert into $", args);
                        break;
                    }
                    if(coords != NULL){
                        Span_Set(coords,
                            coordIdx, I32_Wrapped(m, children->nvalues));
                        coordIdx++;
                    }
                }else{
                    if(child == NULL){
                        return child;
                    }else if(child->type.of == typeOf){
                        current = (Inst *)child;
                    }else{
                        break;
                    }
                }
            }
        }
        prev = token;
    }

    DebugStack_Pop();
    return NULL;
}


status Inst_ShowKeys(Buff *bf, Inst *inst, i32 indent){
    Table *tbl = Span_Get(inst, INST_PROPIDX_CHILDREN);
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
                Inst_ShowKeys(bf, h->value, indent+1);
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
            }else if(sg->val.w == TYPE_STRVEC){
                Span_Set(inst, h->orderIdx, StrVec_Make(m));
            }else if(sg->val.w & TYPE_INSTANCE){
                Span_Set(inst, h->orderIdx, Inst_Make(m, sg->val.w));
            }
        }
    }
    inst->type.of = typeOf;
    return inst;
}
