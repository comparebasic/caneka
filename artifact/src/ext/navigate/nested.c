#include <external.h> 
#include <caneka.h> 

Abstract *Nested_Get(Nested *nd){
    if(nd->type.state & NESTED_ORDERED){
        return Iter_Current(&nd->itemIt);
    }else{
        return Iter_Current(&nd->it);
    }
}

Abstract *Nested_GetRoot(Nested *nd){
    return Span_Get(nd->it.p, 0);
}

Abstract *Nested_GetByKey(Nested *nd, Abstract *key){
    if(nd->type.state & NESTED_KEY_STORE){
        Abstract *a = Iter_Current(&nd->it);
        Hashed *h = OrdTable_Get((OrdTable *)a, key);
        if(h != NULL && h->value != NULL){
            if(h->value->type.of == TYPE_SPAN){
                Iter_Init(&nd->itemIt, (Span *)h->value);
            }else if(h->value->type.of == TYPE_ORDTABLE){
                Iter_Init(&nd->itemIt, ((OrdTable *)h->value)->order);
            }
        }
        return h->value;
    }

    Abstract *args[] = {
        key,
        NULL
    };
    Error(ErrStream->m, (Abstract *)nd, FUNCNAME, FILENAME, LINENUMBER, 
        "Nested not on a key value object to perform key based item query, key: &",
        args);
    return NULL;
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
        Iter_Add(&nd->it, otbl);
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

status Nested_Add(Nested *nd, Abstract *value){
    if(nd->itemIt.p != NULL){
        return Iter_Add(&nd->itemIt, value);
    }else{
        Abstract *args[] = {
            (Abstract *)value,
            NULL
        };
        Error(ErrStream->m, (Abstract *)nd, FUNCNAME, FILENAME, LINENUMBER, 
            "Nested item iterator is not set, value : &", 
            args);
        return ERROR;
    }
}

status Nested_Next(Nested *nd){
    if((nd->itemIt.type.state & END) == 0){
        Iter_Next(&nd->itemIt);
    }
    nd->type.state |= (nd->itemIt.type.state & NORMAL_FLAGS);
    return nd->type.state;
}

Nested *Nested_Make(MemCh *m){
    Nested *nd = MemCh_Alloc(m, sizeof(Nested));
    nd->type.of = TYPE_NESTED;
    Iter_Init(&nd->it, Span_Make(m));
    Iter_Init(&nd->itemIt, Span_Make(m));
    return nd;
}
