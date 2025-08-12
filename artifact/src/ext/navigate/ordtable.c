#include <external.h>
#include <caneka.h>

OrdTable *OrdTable_GetOrMake(OrdTable *otbl, Abstract *key){
    Hashed *h = OrdTable_Get(otbl, key);
    if(h == NULL){
        OrdTable *new = OrdTable_Make(otbl->tbl->m);
        OrdTable_Set(otbl, key, (Abstract *)new);
        return new;
    }else if(h->value->type.of == TYPE_ORDTABLE){
        return (OrdTable *)h->value;
    }else{
        Abstract *args[] = {
            (Abstract *)Type_ToStr(ErrStream->m, h->value->type.of),
            (Abstract *)key,
            NULL
        };
        Error(ErrStream->m, (Abstract *)otbl, FUNCNAME, FILENAME, LINENUMBER,
            "Trying to make an empty OrdTable where a $ value already exists for key @",
            args);
        return NULL;
    }
}

status OrdTable_AddByPath(OrdTable *otbl, StrVec *path, Str *name, Abstract *value){
    DebugStack_Push(otbl, otbl->type.state);
    status r = READY;
    Iter keysIt;
    Iter_Init(&keysIt, path->p);
    Str *key = NULL;
    OrdTable *current = otbl;
    while((Iter_Next(&keysIt) & END) == 0){
        Str *item = (Str *)Iter_Get(&keysIt);
        if((item->type.state|keysIt.type.state) & LAST){
            OrdTable_Set(current, (Abstract *)key, value);
            r |= SUCCESS;
        }else if((item->type.state & MORE)){
            current = OrdTable_GetOrMake(current, (Abstract *)key);
            r |= PROCESSING;
        }else{
            key = item;
        }
    }

    if(r == READY){
        r |= NOOP;
    }

    DebugStack_Pop();
    return r;
}

Hashed *OrdTable_Set(OrdTable *otbl, Abstract *key, Abstract *value){
    otbl->type.state &= ~OUTCOME_FLAGS;

    Hashed *h = Table_SetHashed(otbl->tbl, key, value);
    otbl->type.state |= otbl->tbl->type.state & OUTCOME_FLAGS;
    if(h != NULL){
        Span_Add(otbl->order, (Abstract *)h);
        if(otbl->order->type.state & SUCCESS){
            h->orderIdx = otbl->order->max_idx;
        }
        otbl->type.state |= otbl->order->type.state & OUTCOME_FLAGS; 
    }
    return h;
}

Hashed *OrdTable_Get(OrdTable *tbl, Abstract *key){
    return Table_GetHashed(tbl->tbl, key);
}

Hashed *OrdTable_GetByIdx(OrdTable *tbl, i32 idx){
    return (Hashed *)Span_Get(tbl->order, idx);
}

boolean OrdTable_IsBlank(OrdTable *otbl){
    return otbl->tbl->nvalues == 0;
}

OrdTable *OrdTable_Make(MemCh *m){
    OrdTable *otbl = (OrdTable *)MemCh_Alloc(m, sizeof(OrdTable));
    otbl->type.of = TYPE_ORDTABLE;
    otbl->tbl = Table_Make(m);
    otbl->order = Span_Make(m);
    return otbl;
}
