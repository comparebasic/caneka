#include <external.h>
#include <caneka.h>

void *NodeObj_Att(Inst *nobj, void *key){
    Table *obj = (Table *)Span_Get(nobj, NODEOBJ_PROPIDX_ATTS);
    if(obj != NULL){
        return Table_Get(obj, key);
    }
    return NULL;
}

status NodeObj_ClsInit(MemCh *m){
    status r = READY;
    Table *tbl = Table_Make(m);

    Table_Set(tbl, Str_FromCstr(m, "name", STRING_COPY), I16_Wrapped(m, TYPE_STRVEC));
    Table_Set(tbl, Str_FromCstr(m, "atts", STRING_COPY), I16_Wrapped(m, TYPE_TABLE));
    Table_Set(tbl, Str_FromCstr(m, "children", STRING_COPY), I16_Wrapped(m, TYPE_TABLE));

    r |= Seel_Seel(m, tbl, Str_FromCstr(m, "NodeObj", STRING_COPY ), TYPE_NODEOBJ);

    return r;
}

Table *NodeObj_GetOrMake(Table *tbl, void *key, word op){
    Abstract *a = (Abstract *)Table_Get(tbl, key);
    if(a == NULL){
        if((op & (SPAN_OP_SET|SPAN_OP_RESERVE)) == 0){
            return NULL;
        }
        Inst *new = Inst_Make(tbl->m, TYPE_NODEOBJ);
        Table_Set(tbl, key, new);
        return new;
    }else if(a->type.of == TYPE_NODEOBJ){
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

void *NodeObj_ByPath(Span *inst, StrVec *path, void *value, word op){
    DebugStack_Push(inst, inst->type.state);
    if(inst->type.state & DEBUG){
        void *args[] = {
            path,
            NULL
        };
        Out("^p.ByPath @^0\n", args);
    }

    Iter keysIt;
    Iter_Init(&keysIt, path->p);

    Str *key = NULL;
    Inst *current = inst;
    Table *tbl = Span_Get(inst, NODEOBJ_PROPIDX_CHILDREN);
    while((Iter_Next(&keysIt) & END) == 0){
        Str *item = (Str *)Iter_Get(&keysIt);
        if((item->type.state & MORE) && key != NULL){
            current = NodeObj_GetOrMake(Span_Get(current, NODEOBJ_PROPIDX_CHILDREN),
                key, op);
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
                Table_Set(Span_Get(current, NODEOBJ_PROPIDX_CHILDREN), key, value);
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

        current = Table_Get(Span_Get(current, NODEOBJ_PROPIDX_CHILDREN), key);
        if(current == NULL){
            if(SPAN_OP_SET){
                Table_Set(Span_Get(current, NODEOBJ_PROPIDX_CHILDREN), key, value);
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
