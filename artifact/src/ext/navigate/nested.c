#include <external.h> 
#include <caneka.h> 

status Nested_Next(Nested *nd){
    if((nd->type.state & PROCESSING) == 0){
        Nested_SetRoot(nd, Nested_GetRoot(nd));
        nd->type.state |= PROCESSING;
    }
    i16 guard;
    while(TRUE){
        Guard_Incr(&guard, GUARD_MAX, FUNCNAME, FILENAME, LINENUMBER);
        Frame *fm = Iter_Current(&nd->it);
        while((Iter_Next(&fm->it) & END) == 0){
            Abstract *item = Iter_Get(&fm->it);
            if(item->type.of == TYPE_HASHED){
                Hashed *h = (Hashed *)item;
                if(h->value->type.of == TYPE_ORDTABLE || h->value->type.of == TYPE_SPAN){
                    Nested_IndentByIdx(nd, fm->it.idx);
                    break;
                }else{
                    nd->h = h;
                    goto end;
                }
            }
        }
        if(fm->it.type.state & END){
            if(nd->it.idx == 0){
                nd->type.state |= END;
                nd->h = NULL;
                goto end;
            }else{
                Iter_Pop(&nd->it); 
            }
        }
    }
end:
    return nd->type.state;
}

Abstract *Nested_Get(Nested *nd){
    return Iter_Current(&nd->it);
}

Abstract *Nested_GetRoot(Nested *nd){
    return Span_Get(nd->it.p, 0);
}

status Nested_Outdent(Nested *nd){
    status r = Iter_Pop(&nd->it);
    return r;
}

status Nested_Indent(Nested *nd, Abstract *key){
    Frame *fm = (Frame *)Iter_Current(&nd->it);
    if(fm != NULL && fm->value->type.of == TYPE_ORDTABLE){
        Hashed *h = OrdTable_Get((OrdTable*)fm->value, key);
        if(h != NULL && h->value != NULL && h->value->type.of == TYPE_ORDTABLE){
            Iter_Add(&nd->it, 
                Frame_Make(nd->m, -1, key, h->value, ((OrdTable*)h->value)->order));
            return nd->it.type.state & SUCCESS;
        }
    }
    Abstract *args[] = {
        (Abstract *)nd,
        (Abstract *)key,
        NULL
    };
    Error(ErrStream->m, (Abstract *)nd, FUNCNAME, FILENAME, LINENUMBER, 
        "Nested unable to indent by key, nested: @, key: &", args);
    return ERROR;
}

status Nested_IndentByIdx(Nested *nd, i32 idx){
    Frame *fm = (Frame *)Iter_Current(&nd->it);
    Span *p = NULL;
    if(fm != NULL){
        if(fm->value->type.of == TYPE_SPAN){
            p = (Span *)fm->value;
        }else if(fm->value->type.of == TYPE_ORDTABLE){
            p = ((OrdTable *)fm->value)->order;
        }
    }
    if(p != NULL){
        Abstract *a = Span_Get(p, idx);
        if(a != NULL && a->type.of == TYPE_SPAN){
            Iter_Add(&nd->it, 
                Frame_Make(nd->m, idx, NULL, a, (Span *)a));
            return nd->it.type.state & SUCCESS;
        }else if(a->type.of == TYPE_HASHED && ((Hashed *)a)->value->type.of == TYPE_ORDTABLE){
            Hashed *h = (Hashed *)a;
            Iter_Add(&nd->it, 
                Frame_Make(nd->m, idx, h->item, h->value, ((OrdTable *)h->value)->order));
            return nd->it.type.state & SUCCESS;
        }
    }
    Abstract *args[] = {
        (Abstract *)nd,
        (Abstract *)I32_Wrapped(nd->it.p->m, idx),
        NULL
    };
    Error(ErrStream->m, (Abstract *)nd, FUNCNAME, FILENAME, LINENUMBER, 
        "Nested unable to nest by idx,  nested: @, idx: @", args);
    return ERROR;
}

Abstract *Nested_GetByKey(Nested *nd, Abstract *key){
    Frame *fm = Iter_Current(&nd->it);
    if(fm->value->type.of == TYPE_ORDTABLE){
        Hashed *h = OrdTable_Get((OrdTable *)fm->value, key);
        if(h != NULL){
            return h->value;
        }else{
            return NULL;
        }
    }
    return NULL;
}

status Nested_AddByKey(Nested *nd, Abstract *key, Abstract *value){
    if(nd->type.state & DEBUG){
        Abstract *args[] = {
            (Abstract *)key,
            (Abstract *)value,
            (Abstract *)nd,
            NULL
        };
        Out("^p.Add by key @ -> @ to @ ^0.\n", args);
    }
    Frame *fm = Iter_Current(&nd->it);
    if(fm == NULL || fm->value == NULL){
        OrdTable *store = OrdTable_Make(nd->m);
        OrdTable_Set(store, key, value);
        Iter_Add(&nd->it, 
            Frame_Make(nd->m, -1, key, (Abstract *)store, store->order));
        return store->type.state & SUCCESS;
    }else if(fm->value->type.of == TYPE_ORDTABLE){
        Hashed *h = OrdTable_Get((OrdTable *)fm->value, key);
        if(h == NULL){
            OrdTable_Set((OrdTable *)fm->value, key, value);
            return SUCCESS;
        }else if(value != NULL && value->type.of == TYPE_ORDTABLE
                && OrdTable_IsBlank((OrdTable *)value)){
            return NOOP;
        }
    }
    Abstract *args[] = {
        (Abstract *)Type_ToStr(nd->m, fm->value->type.of),
        (Abstract *)key,
        NULL
    };
    Error(ErrStream->m, (Abstract *)nd, FUNCNAME, FILENAME, LINENUMBER, 
        "Nested encountered type that is not a key store @, for key: &", args);
    return ERROR;
}

Abstract *Nested_GetByPath(Nested *_nd, Span *keys){
    Iter it;
    Iter_Init(&it, keys);
    _nd->m->type.range++;
    Nested *nd = (Nested *)Nested_Make(_nd->m);
    _nd->m->type.range--;
    Nested_SetRoot(nd, Nested_GetRoot(_nd));
    Abstract *value = NULL;
    while((Iter_Next(&it) & END) == 0){
        Abstract *key = Iter_Get(&it);
        if(it.type.state & LAST){
            value = Nested_GetByKey(nd, key);
        }else{
            Nested_Indent(nd, key);
        }
    }
    _nd->m->type.range++;
    MemCh_Free(nd->m);
    _nd->m->type.range--;
    return value; 
}

status Nested_AddByPath(Nested *nd, Span *keys, Abstract *value){
    DebugStack_Push(nd, nd->type.state);
    if(nd->type.state & DEBUG){
        Abstract *args[] = {
            (Abstract *)keys,
            (Abstract *)value,
            (Abstract *)nd,
            NULL
        };
        Out("^p.Adding by path @ -> @ to @ ^0.\n", args);
    }
    i32 idx = nd->it.idx;
    Iter_Reset(&nd->it);
    status r = READY;
    Iter keysIt;
    Iter_Init(&keysIt, keys);
    Abstract *key = NULL;
    if(keys->nvalues == 1){
        r |= Nested_AddByKey(nd, key, value); 
    }else{
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

        Frame *fm = Iter_Current(&nd->it);
        if((r & ERROR) == 0 && (keysIt.type.state & END) && fm->value != NULL){
            if(fm->value->type.of != TYPE_ORDTABLE){
                r |= ERROR;
            }else{
                OrdTable_Set((OrdTable *)fm->value, key, value);
                r |= fm->value->type.state & SUCCESS;
            }
        }
    }

    Iter_Reset(&nd->it);
    Iter_GetByIdx(&nd->it, idx);
    DebugStack_Pop();
    return r;
}

status Nested_SetRoot(Nested *nd, Abstract *root){
    Frame *fm = NULL;
    if(root == NULL){
        return NOOP;
    }else if(root->type.of == TYPE_FRAME){
        fm = (Frame *)root;
    }else if(root->type.of == TYPE_ORDTABLE){
        fm = Frame_Make(nd->m, -1, NULL, root, ((OrdTable *)root)->order); 
    }else if(root->type.of == TYPE_SPAN){
        fm = Frame_Make(nd->m, -1, NULL, root, (Span *)root); 
    }

    if(fm != NULL){
        Span_Wipe(nd->it.p);
        Iter_Add(&nd->it, fm);
        return SUCCESS;
    }

    Abstract *args[] = {
        (Abstract *)nd,
        (Abstract *)fm,
        NULL
    };
    Error(ErrStream->m, (Abstract *)nd, FUNCNAME, FILENAME, LINENUMBER, 
        "Nested unable to start with an element that is not a Span or OrdTable, @, root: &", args);
    return ERROR;
}

Nested *Nested_Make(MemCh *m){
    Nested *nd = MemCh_Alloc(m, sizeof(Nested));
    nd->type.of = TYPE_NESTED;
    nd->m = m;
    Iter_Init(&nd->it, Span_Make(m));
    return nd;
}
