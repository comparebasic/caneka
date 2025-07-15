#include <external.h> 
#include <caneka.h> 

Abstract *Nested_Get(Nested *nd){
    return Iter_Current(&nd->it);
}

Abstract *Nested_GetRoot(Nested *nd){
    return Span_Get(nd->it.p, 0);
}

status Nested_Outdent(Nested *nd){
    return Iter_PrevRemove(&nd->it);
}

status Nested_Indent(Nested *nd, Abstract *key){
    Abstract *a = Iter_Current(&nd->it);
    if(a != NULL && a->type.of == TYPE_ORDTABLE){
        Hashed *h = OrdTable_Get((OrdTable*)a, key);
        if(h != NULL && h->value != NULL && h->value->type.of == TYPE_ORDTABLE){
            Iter_Add(&nd->it, h->value);
        }
    }
    return NOOP;
}

Abstract *Nested_GetByKey(Nested *nd, Abstract *key){
    Abstract *a = Iter_Current(&nd->it);
    Hashed *h = OrdTable_Get((OrdTable *)a, key);
    if(h != NULL){
        return h->value;
    }else{
        return NULL;
    }
}

status Nested_AddByKey(Nested *nd, Abstract *key, Abstract *value){
    Abstract *store = Iter_Current(&nd->it);
    if(store == NULL){
        store = (Abstract *)OrdTable_Make(nd->it.p->m);
        Iter_Add(&nd->it, store);
        OrdTable_Set((OrdTable *)store, key, value);
        return store->type.state & SUCCESS;
    }else if(store->type.of == TYPE_ORDTABLE){
        Hashed *h = OrdTable_Get((OrdTable *)store, key);
        if(h == NULL){
            OrdTable_Set((OrdTable *)store, key, value);
            return SUCCESS;
        }else if(value != NULL && value->type.of == TYPE_ORDTABLE
                && OrdTable_IsBlank((OrdTable *)store)){
            return NOOP;
        }
    }
    Abstract *args[] = {
        store,
        (Abstract *)key,
        NULL
    };
    Error(ErrStream->m, (Abstract *)nd, FUNCNAME, FILENAME, LINENUMBER, 
        "Nested encountered type that is not a key store @, for key: &", args);
    return ERROR;
}

status Nested_AddByPath(Nested *nd, Span *keys, Abstract *value){
    i32 idx = nd->it.idx;
    Iter_Reset(&nd->it);
    status r = READY;
    Iter keysIt;
    Iter_Init(&keysIt, keys);
    Abstract *store = NULL;
    Abstract *key = NULL;
    while((Iter_Next(&keysIt) & END) == 0){
        key = Iter_Get(&keysIt);
        if(keysIt.type.state & LAST){
            continue;
        }
        if(nd->type.state & DEBUG){
            Abstract *args[] = {
                (Abstract *)key,
                (Abstract *)Iter_Current(&nd->it),
                NULL
            };
            Out("^c.Added key @ current to &\n", args);
        }
        OrdTable *otbl = OrdTable_Make(nd->it.p->m);
        r |= Nested_AddByKey(nd, key, (Abstract *)otbl); 
        Nested_Indent(nd, key);
        if(r & ERROR){
            break;
        }
    }

    store = Iter_Current(&nd->it);
    if((r & ERROR) == 0 && (keysIt.type.state & END) && store != NULL){
        if(store->type.of != TYPE_ORDTABLE){
            r |= ERROR;
        }else{
            OrdTable_Set((OrdTable *)store, key, value);
            r |= store->type.state & SUCCESS;
        }
    }

    Iter_Reset(&nd->it);
    Iter_GetByIdx(&nd->it, idx);
    return r;
}

Nested *Nested_Make(MemCh *m){
    Nested *nd = MemCh_Alloc(m, sizeof(Nested));
    nd->type.of = TYPE_NESTED;
    Iter_Init(&nd->it, Span_Make(m));
    return nd;
}
