#include <external.h>
#include <caneka.h>

void *NodeObj_Att(Inst *nobj, void *key){
    Table *obj = (Table *)Span_Get(nobj, NODEOBJ_PROPIDX_ATTS);
    if(obj != NULL){
        return Table_Get(obj, key);
    }
    return NULL;
}

void *NodeObj_ByPath(Span *inst, StrVec *path, void *value, word op){
    DebugStack_Push(inst, inst->type.state);

    if((path->type.state & STRVEC_PATH) == 0 && path->p->nvalues > 1){
        void *args[] = {inst, NULL};
        Error(inst->m, FUNCNAME, FILENAME, LINENUMBER,
            "Error NodeObj_ByPath requires a strvec path", args);
        return NULL;
    }

    Inst *current = inst;
    Abstract *prev = NULL;

    Iter it;
    Iter_Init(&it, path->p);
    while((Iter_Next(&it) & END) == 0){
        Abstract *token = Iter_Get(&it);
        if(it.type.state & LAST){
            Abstract *key = (token->type.state & MORE) ? prev : token;
            Table *children = Span_Get(current, NODEOBJ_PROPIDX_CHILDREN);
            Abstract *child = Table_Get(children, key);

            void *args[] = {key, child, Type_StateVec(inst->m, TYPE_ITER, op), NULL};
            Out("^p.last key:@ child:@ @^0\n", args);

            if(child == NULL && op == SPAN_OP_SET){
                Table_Set(children, key, value); 
                void *args[] = {key, value, NULL};
                Out("^p.   setting: @/@^0\n", args);
                DebugStack_Pop();
                return value;
            }else{
                DebugStack_Pop();
                return child;
            }
        }else if(token->type.state & MORE){
            if(prev){
                Table *children = Span_Get(current, NODEOBJ_PROPIDX_CHILDREN);
                Abstract *child = Table_Get(children, prev);
                if(child == NULL && op == SPAN_OP_SET){
                    Inst *new = Inst_Make(inst->m, TYPE_NODEOBJ);
                    Table_Set(children, prev, new); 
                    current = new;
                }else if(child->type.of == TYPE_NODEOBJ){
                    DebugStack_Pop();
                    return NULL;
                }
            }
        }
        prev = token;
    }

    DebugStack_Pop();
    return current;
}

status NodeObj_ClsInit(MemCh *m){
    status r = READY;
    Table *tbl = Table_Make(m);

    Table_Set(tbl, S(m, "name"), I16_Wrapped(m, TYPE_STRVEC));
    Table_Set(tbl, S(m, "atts"), I16_Wrapped(m, TYPE_TABLE));
    Hashed *h = Table_SetHashed(tbl, S(m, "children"), I16_Wrapped(m, TYPE_TABLE));

    r |= Seel_Seel(m, tbl, S(m, "NodeObj"), TYPE_NODEOBJ, h->orderIdx);

    return r;
}
