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

status Seel_Seel(MemCh *m, Table *seel, Str *name, cls typeOf){
    seel->type.state |= TABLE_SEALED;
    Lookup_Add(m, SeelLookup, typeOf, seel);
    Lookup_Add(m, SeelNameLookup, typeOf, name);
    Lookup_Add(m, ToStreamLookup, typeOf, Inst_Print);
    return seel->type.state;
}

Table *Seel_GetOrMake(Table *tbl, void *key, word op, i32 childrenIdx, cls typeOf){
    Abstract *a = (Abstract *)Table_Get(tbl, key);
    if(a == NULL){
        if((op & (SPAN_OP_SET|SPAN_OP_RESERVE)) == 0){
            return NULL;
        }
        Table *new = Table_Make(tbl->m);
        Table_Set(tbl, key, new);
        return new;
    }else if(a->type.of == typeOf){
        return (Table *)Span_Get((Span *)a, childrenIdx);
    }else{
        void *args[] = {
            Type_ToStr(tbl->m, a->type.of),
            key,
            a,
            NULL
        };
        Error(tbl->m, FUNCNAME, FILENAME, LINENUMBER,
            "Trying to make an empty Table where a $ value already exists for key @, or is not an object @",
            args);
        return NULL;
    }
}

void *Seel_ByPath(Span *inst, i32 childrenIdx, StrVec *path, void *value, word op){
    DebugStack_Push(inst, inst->type.state);

    cls typeOf = inst->type.of;

    if(inst->type.state & DEBUG){
        void *args[] = {
            inst,
            path,
            NULL
        };
        Out("^p.ByPath @[@]^0\n", args);
    }

    Iter keysIt;
    Iter_Init(&keysIt, path->p);

    Str *key = NULL;
    Inst *current = inst;
    while((Iter_Next(&keysIt) & END) == 0){
        Str *item = (Str *)Iter_Get(&keysIt);
        if((item->type.state & MORE) && key != NULL){
            current = Seel_GetOrMake(Span_Get(current, childrenIdx),
                key, op, childrenIdx, typeOf);
            if(current == NULL){
                DebugStack_Pop();
                return NULL;
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

        current = Seel_GetOrMake(Span_Get(current, childrenIdx), key, op, childrenIdx, typeOf);
        if(current == NULL){
            DebugStack_Pop();
            return NULL;
        }
    }

    DebugStack_Pop();
    return current;
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
