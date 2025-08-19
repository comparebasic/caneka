#include <external.h>
#include <caneka.h>

PathTable *PathTable_GetOrMake(PathTable *pt, Abstract *key){
    Hashed *h = PathTable_Get(pt, key);
    if(h == NULL){
        PathTable *new = PathTable_Make(pt->tbl->m);
        PathTable_Set(pt, key, (Abstract *)new);
        return new;
    }else if(h->value->type.of == TYPE_PATHTABLE){
        return (PathTable *)h->value;
    }else{
        Abstract *args[] = {
            (Abstract *)Type_ToStr(ErrStream->m, h->value->type.of),
            (Abstract *)key,
            NULL
        };
        Error(ErrStream->m, (Abstract *)pt, FUNCNAME, FILENAME, LINENUMBER,
            "Trying to make an empty PathTable where a $ value already exists for key @",
            args);
        return NULL;
    }
}

status PathTable_AddByPath(PathTable *pt, StrVec *path, Abstract *value){
    DebugStack_Push(pt, pt->type.state);
    if(pt->type.state & DEBUG){
        Abstract *args[] = {
            (Abstract *)path,
            (Abstract *)pt,
            NULL
        };
        Out("^p.Adding & to &^0.\n", args);
    }
    status r = READY;
    Iter keysIt;
    Iter_Init(&keysIt, path->p);
    Str *key = NULL;
    PathTable *current = pt;
    while((Iter_Next(&keysIt) & END) == 0){
        Str *item = (Str *)Iter_Get(&keysIt);
        if((item->type.state|keysIt.type.state) & LAST){
            if(pt->type.state & DEBUG){
                Abstract *args[] = {
                    (Abstract *)current,
                    (Abstract *)item,
                    (Abstract *)key,
                    (Abstract *)value,
                    NULL
                };
                Out("^p.  last: @ of @/@ -> &^0.\n", args);
            }
            PathTable_Set(current, (Abstract *)key, value);
            r |= SUCCESS;
        }else if((item->type.state & MORE)){
            if(pt->type.state & DEBUG){
                Abstract *args[] = {
                    (Abstract *)current,
                    (Abstract *)key,
                    NULL
                };
                Out("^p.  get-or-make: @ -> @^0.\n", args);
            }
            current = PathTable_GetOrMake(current, (Abstract *)key);
            r |= PROCESSING;
        }else{
            key = item;
            if(pt->type.state & DEBUG){
                Abstract *args[] = {
                    (Abstract *)key,
                    NULL
                };
                Out("^p.  set-key: @^0.\n", args);
            }
        }
    }

    if(r == READY){
        r |= NOOP;
    }

    DebugStack_Pop();
    return r;
}

Hashed *PathTable_Set(PathTable *pt, Abstract *key, Abstract *value){
    pt->type.state &= ~OUTCOME_FLAGS;

    Hashed *h = Table_SetHashed(pt->tbl, key, value);
    pt->type.state |= pt->tbl->type.state & OUTCOME_FLAGS;
    if(h != NULL){
        Span_Add(pt->order, (Abstract *)h);
        if(pt->order->type.state & SUCCESS){
            h->orderIdx = pt->order->max_idx;
        }
        pt->type.state |= pt->order->type.state & OUTCOME_FLAGS; 
    }
    return h;
}

Hashed *PathTable_Get(PathTable *tbl, Abstract *key){
    return Table_GetHashed(tbl->tbl, key);
}

Hashed *PathTable_GetByIdx(PathTable *tbl, i32 idx){
    return (Hashed *)Span_Get(tbl->order, idx);
}

boolean PathTable_IsBlank(PathTable *pt){
    return pt->tbl->nvalues == 0;
}

PathTable *PathTable_Make(MemCh *m){
    PathTable *pt = (PathTable *)MemCh_Alloc(m, sizeof(PathTable));
    pt->type.of = TYPE_PATHTABLE;
    pt->tbl = Table_Make(m);
    pt->order = Span_Make(m);
    return pt;
}
