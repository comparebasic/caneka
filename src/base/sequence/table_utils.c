#include <external.h>
#include "base_module.h"

i32 Table_SetByCstr(Table *tbl, char *cstr, void *value){
    return Table_Set(tbl, Str_FromCstr(tbl->m, cstr, STRING_COPY), value);
}

Span *Table_Ordered(MemCh *m, Table *tbl){
    Span *p = Span_Make(m);
    Iter it;
    Iter_Init(&it, tbl);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = (Hashed *)Iter_Get(&it);
        if(h != NULL && h->orderIdx >= 0){
            Span_Set(p, h->orderIdx, h);
        }
    }
    return p;
}

Span *Table_OrdValues(MemCh *m, Table *tbl){
    Span *p = Span_Make(m);
    Iter it;
    Iter_Init(&it, tbl);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = (Hashed *)Iter_Get(&it);
        if(h != NULL){
            Span_Set(p, h->orderIdx, h->value);
        }
    }
    return p;
}

status Table_Underlay(Table *a, Table *b){
    status r = READY;
    Iter it;
    Iter_Init(&it, (Span *)b);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);
        if(h != NULL){
            if(Table_GetHashed(a, h->key) == NULL){
                Table_Set(a, h->key, h->value);
                r |= SUCCESS;
            }
        }
    }
    if(r == READY){
        r |= NOOP;
    }
    return r;
}

status Table_Overlay(Table *a, Table *b){
    status r = READY;
    Iter it;
    Iter_Init(&it, (Span *)b);
    while((Iter_Next(&it) & END) == 0){
        Hashed *_h = Iter_Get(&it);
        if(_h != NULL){
            Hashed *h = Table_GetHashed(a, _h->key);
            if(h != NULL){
                h->value = _h->value;
                r |= SUCCESS;
            }else{
                Table_Set(a, _h->key, _h->value);
            }
        }
    }
    if(r == READY){
        r |= NOOP;
    }
    return r;
}

status Table_Lay(Span *dest, Span *src, boolean overlay){
    status r = READY;
    if(src == NULL){
        return r;
    }
    MemCh *m = dest->m;

    Iter it;
    Iter_Init(&it, src);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = (Hashed *)Iter_Get(&it);
        if(h != NULL){
            Hashed *hb = Table_GetHashed(src, h->key);
            if(hb != NULL && hb->type.of == TYPE_TABLE){
                if(!overlay && Table_Get(hb->value, h->key)){
                   continue; 
                }
                r |= Table_Set(hb->value, h->key, h->value);
            }else{
                if(overlay){
                    r |= Table_Set(dest, h->key, h->value);
                }
            }

        }
    }

    return r;
}

status Table_Merge(Table *dest, Table *src){
    return Table_Lay(dest, src, TRUE);
}

Table *Table_GetOrMake(Table *tbl, void *key, word op){
    Abstract *a = (Abstract *)Table_Get(tbl, key);
    if(a == NULL){
        if((op & (SPAN_OP_SET|SPAN_OP_RESERVE)) == 0){
            return NULL;
        }
        Table *new = Table_Make(tbl->m);
        Table_Set(tbl, key, new);
        return new;
    }else if(a->type.of & TYPE_TABLE){
        return (Table *)a;
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

void *Table_ByPath(Table *tbl, StrVec *path, void *value, word op){
    DebugStack_Push(tbl, tbl->type.state);

    Iter keysIt;
    Iter_Init(&keysIt, path->p);

    Str *key = NULL;
    Table *current = tbl;
    while((Iter_Next(&keysIt) & END) == 0){
        Str *item = (Str *)Iter_Get(&keysIt);
        if((item->type.state & MORE) && key != NULL){
            current = Table_GetOrMake(current, key, op);
            if(current == NULL){
                DebugStack_Pop();
                return NULL;
            }
            key = NULL;
        }else{
            key = item;
            if((keysIt.type.state & LAST) && (op & SPAN_OP_SET)){
                Table_Set(current, key, value);
            }
        }
    }

    if(key != NULL && path->p->nvalues > 1 && (key->type.state & (LAST|MORE)) == 0){
        current = Table_GetOrMake(current, key, op);
        if(current == NULL){
            DebugStack_Pop();
            return NULL;
        }
    }

    DebugStack_Pop();
    return current;
}
