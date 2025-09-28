#include <external.h>
#include <caneka.h>
static i64 dim_nvalue_max[TABLE_MAX_DIMS] = {8, 128, 2048, 3000, 50000};

static Hashed *Table_GetSetHashed(Iter *it, word op, Abstract *key, Abstract *value){
    Table *tbl = (Table *)it->p;
    tbl->type.state &= ~(SUCCESS|NOOP);
    Abstract *args[5];
    if(key == NULL){
        return NULL;
    }

    if(op & SPAN_OP_SET && tbl->nvalues > dim_max_idx[tbl->dims]){
        Iter_ExpandTo(it, tbl->dims);
    }

    Hashed *h = Hashed_Make(tbl->m, key);
    h->value = value;

    HKey hk;
    Table_HKeyInit(&hk, tbl->dims, h->id);
    while((tbl->type.state & SUCCESS) == 0){
        Table_HKeyVal(&hk);
        Iter_GetByIdx(it, hk.idx);
        if(tbl->type.state & DEBUG){
            args[0] = (Abstract*)&hk;
            args[1] = NULL;
            Out("^p.Looking at &^0\n", args);
        }
        if(it->type.state & NOOP){
            break;
        }
        Hashed *record = Iter_Get(it);
        if(record == NULL){
            if(tbl->type.state & DEBUG){
                args[0] = (Abstract*)&hk;
                args[1] = NULL;
                Out("^p.  Looking record is null^0\n", args);
            }
            if(op & SPAN_OP_GET){
                tbl->type.state |= NOOP;
                return NULL;
            }else if(op & SPAN_OP_SET){
                if(tbl->type.state & DEBUG){
                    args[0] = (Abstract*)&hk;
                    args[1] = (Abstract*)h;
                    args[2] = NULL;
                    Out("^p.    Setting & -> &^0\n", args);
                }
                Span_Set((Span *)tbl, hk.idx, (Abstract *)h);
                tbl->type.state |= SUCCESS;
                return h;
            }
        }else if(Hashed_Equals(h, record)){
            if(tbl->type.state & DEBUG){
                args[0] = (Abstract*)&hk;
                args[1] = (Abstract*)record;
                args[2] = NULL;
                Out("^p.  Looking record matches & -> &^0\n", args);
            }
            if(op & SPAN_OP_SET){
                record->value = h->value;
            }
            tbl->type.state |= SUCCESS;
            return record;
        }else{
            if(tbl->type.state & DEBUG){
                args[0] = (Abstract*)&hk;
                args[1] = (Abstract*)record;
                args[2] = NULL;
                Out("^p.  Record but match & -> &^0\n", args);
            }
        }
    }

    if(tbl->type.state & DEBUG){
        args[0] = (Abstract*)&hk;
        args[2] = NULL;
        Out("^p.  No match &^0\n", args);
    }

    return NULL;
}

status Table_HKeyInit(HKey *hk, i8 dims, util id){
    hk->type.of = TYPE_HKEY;
    hk->type.state = 0;
    hk->idx = 0;
    hk->id = id;
    hk->dim = dims;
    hk->pos = 0;
    return SUCCESS;
}

status Table_HKeyVal(HKey *hk){
    if((hk->type.state & PROCESSING) == 0){
        hk->type.state |= PROCESSING;
    }else{
        if(hk->pos == 3 && hk->dim > 0){
            hk->dim--;
            hk->pos = 0;
        }else if(hk->pos < 3 && (hk->type.state & MORE) == 0){
            hk->pos++;
        }else{
            hk->idx++;
            hk->pos = 0;
            hk->type.state |= MORE;
            return hk->type.state;
        }
    }
    hk->idx = (i32) ((hk->id >> (hk->pos*(hk->dim+1)*4)) & _modulos[hk->dim+1]);
    return hk->type.state;
}

status Table_SetKey(Iter *it, Abstract *a){
    Hashed *h = Table_GetSetHashed(it, SPAN_OP_SET, a, NULL);
    /* set idx here */
    return SUCCESS;
}

i32 Table_SetIdxEntry(Iter *it, Abstract *a){
    Hashed *h = Table_GetSetHashed(it, SPAN_OP_SET, a, NULL);
    i32 value = it->metrics.set;
    Single *tag = I32_Wrapped(it->p->m, value);
    h->value = (Abstract *)tag;
    return (i32)value;
}

Hashed *Table_SetValue(Iter *it, Abstract *a){
    Hashed *h = Span_Get(it->p, it->metrics.set);
    if(h != NULL){
        h->value = a;
        it->metrics.set = -1;
        return h;
    }
    return NULL;
}

Hashed *Table_GetHashed(Table *tbl, Abstract *a){
    Iter it;
    Iter_Init(&it, tbl);
    Hashed *h = Table_GetSetHashed(&it, SPAN_OP_GET, a, NULL);
    if(h != NULL && h->value != NULL){
        return h;
    }
    return NULL;
}

Hashed *Table_SetHashed(Table *tbl, Abstract *a, Abstract *value){
    tbl->type.state &= ~OUTCOME_FLAGS;
    Iter it;
    Iter_Init(&it, tbl);
    Hashed *h = Table_GetSetHashed(&it, SPAN_OP_SET, a, value);
    tbl->type.state |= it.type.state & OUTCOME_FLAGS;
    return h;
}

Abstract *Table_GetKey(Table *tbl, i32 idx){
    Hashed *h = Span_Get(tbl, idx);
    return h->key;
}

Abstract *Table_Get(Table *tbl, Abstract *a){
    Iter it;
    Iter_Init(&it, tbl);
    if(tbl != NULL && a != NULL){
        Hashed *h = Table_GetSetHashed(&it, SPAN_OP_GET, a, NULL);
        if(h != NULL){
            return h->value;
        }
    }
    return NULL;
}

i32 Table_Set(Table *tbl, Abstract *a, Abstract *value){
    Iter it;
    Iter_Init(&it, tbl);
    Hashed *h = Table_GetSetHashed(&it, SPAN_OP_SET, a, value);
    return h->idx;
}

Abstract *Table_FromIdx(Table *tbl, i32 idx){
    Hashed *h = (Hashed *)Span_Get(tbl, idx);
    if(h != NULL){
        return h->value;
    }
    return NULL;
}

i32 Table_GetIdx(Table *tbl, Abstract *a){
    Iter it;
    Iter_Init(&it, tbl);
    Hashed *h = Table_GetSetHashed(&it, SPAN_OP_GET, a, NULL);
    if(tbl->type.state & SUCCESS){
        return h->idx;
    }else{
        return -1;
    }
}

Table *Table_Make(MemCh *m){
    Table *tbl =  (Table *)Span_Make(m);
    tbl->type.of = TYPE_TABLE;
    return tbl;
}
