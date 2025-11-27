#include <external.h>
#include <caneka.h>

static i64 dim_occupied_max[TABLE_MAX_DIMS] = {8, 128, 2048, 3000};

static Hashed *Table_GetSetHashed(Iter *it, word op, void *_key, void *_value){
    Abstract *key = (Abstract *)_key;
    Abstract *value = (Abstract *)_value;
    Table *tbl = (Table *)it->p;
    tbl->type.state &= ~(SUCCESS|NOOP);
    void *args[5];
    if(key == NULL){
        key = (Abstract *)I32_Wrapped(tbl->m, tbl->nvalues);
    }

    if(tbl->type.state & DEBUG){
        args[0] = I32_Wrapped(OutStream->m, tbl->nvalues);
        args[1] = key;
        args[2] = NULL;
        if(op & SPAN_OP_GET){
            Out("^p.Get \\@@^0\n", args);
        }else{
            Out("^p.Set \\@@^0\n", args);
        }
    }

    if((op & SPAN_OP_SET) && tbl->nvalues > dim_occupied_max[tbl->dims]){
        Iter_ExpandTo(it, dim_max_idx[tbl->dims]+1);
    }

    util parity = Get_Hash(key);

    HKey hk;
    Table_HKeyInit(&hk, tbl->dims, parity);
    while((tbl->type.state & SUCCESS) == 0){
        Table_HKeyVal(&hk);
        Iter_GetByIdx(it, hk.idx);

        if((it->type.state & NOOP) && (op & SPAN_OP_SET)){
            if(hk.idx > dim_max_idx[tbl->dims]){
                Iter_ExpandTo(it, dim_max_idx[tbl->dims]+1);
                Table_HKeyInit(&hk, tbl->dims, parity);
                continue;
            }
        }
        Hashed *record = Iter_Current(it);
        if(record == NULL || record->orderIdx == -1){
            if(op & SPAN_OP_GET){
                if(Table_HKeyMiss(&hk) & END){
                    tbl->type.state |= NOOP;
                    return NULL;
                }else{
                    continue;
                }
            }else if(op & SPAN_OP_SET){
                Hashed *h = record;
                if(h == NULL){
                    h = Hashed_Make(tbl->m, key);
                }
                h->value = value;
                h->id = parity;
                h->idx = hk.idx;
                h->orderIdx = tbl->nvalues;
                Span_Set((Span *)tbl, hk.idx, h);
                tbl->type.state |= SUCCESS;
                return h;
            }
        }else if(record->id == parity &&  Equals(record->key, key)){
            if(op & SPAN_OP_SET){
                record->value = value;
            }
            tbl->type.state |= SUCCESS;
            return record;
        }
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

status Table_HKeyMiss(HKey *hk){
    if(hk->dim > 0){
        hk->dim--;
        hk->idx = 0;
        hk->pos = 0;
        hk->type.state &= ~PROCESSING;
        return SUCCESS;
    }
    return END;
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

status Table_SetKey(Iter *it, void *_a){
    Abstract *a = (Abstract *)_a;

    Hashed *h = Table_GetSetHashed(it, SPAN_OP_SET, a, NULL);
    it->metrics.selected = h->idx;

    if(a->type.of == TYPE_STR || a->type.of == TYPE_STRVEC &&
        Equals(a, Str_FromCstr(ErrStream->m, "config",ZERO))){
    }

    /* set idx here */
    return SUCCESS;
}

i32 Table_SetIdxEntry(Iter *it, void *_a){
    Abstract *a = (Abstract *)_a;

    Hashed *h = Table_GetSetHashed(it, SPAN_OP_SET, a, NULL);
    i32 value = it->metrics.set;
    Single *tag = I32_Wrapped(it->p->m, value);
    h->value = tag;
    return (i32)value;
}

Hashed *Table_SetValue(Iter *it, void *_a){
    Abstract *a = (Abstract *)_a;

    if(it->metrics.selected > -1){
        Hashed *h = Span_Get(it->p, it->metrics.selected);
        if(h != NULL){
            h->value = a;
            it->metrics.selected = -1;
            return h;
        }
    }
    return NULL;
}

Hashed *Table_GetHashed(Table *tbl, void *_a){
    Abstract *a = (Abstract *)_a;

    Iter it;
    Iter_Init(&it, tbl);
    Hashed *h = Table_GetSetHashed(&it, SPAN_OP_GET, a, NULL);
    if(h != NULL){
        return h;
    }
    return NULL;
}

Hashed *Table_SetHashed(Table *tbl, void *_a, void *_value){
    Abstract *a = (Abstract *)_a;
    Abstract *value = (Abstract *)_value;

    tbl->type.state &= ~OUTCOME_FLAGS;
    Iter it;
    Iter_Init(&it, tbl);
    Hashed *h = Table_GetSetHashed(&it, SPAN_OP_SET, a, value);
    tbl->type.state |= it.type.state & OUTCOME_FLAGS;

    if(a->type.of == TYPE_STR || a->type.of == TYPE_STRVEC &&
        Equals(a, Str_FromCstr(ErrStream->m, "config", ZERO))){
    }

    return h;
}

void *Table_GetKey(Table *tbl, i32 idx){
    Hashed *h = Span_Get(tbl, idx);
    return h->key;
}

void *Table_Get(Table *tbl, void *_a){
    Abstract *a = (Abstract *)_a;

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

i32 Table_UnSet(Table *tbl, void *a){
    Hashed *h = Table_GetHashed(tbl, a);
    if(h != NULL){
        i32 orderIdx = h->orderIdx;
        h->value = NULL;
        h->id = 0;
        h->orderIdx = -1;
        tbl->nvalues--;
        return orderIdx;
    }
    return -1;
}

i32 Table_Set(Table *tbl, void *_a, void *_value){
    Abstract *a = (Abstract *)_a;
    Abstract *value = (Abstract *)_value;

    Iter it;
    Iter_Init(&it, tbl);
    Hashed *h = Table_GetSetHashed(&it, SPAN_OP_SET, a, value);

    if(a->type.of == TYPE_STR || a->type.of == TYPE_STRVEC &&
        Equals(a, Str_FromCstr(ErrStream->m, "config",ZERO))){
    }

    return h->idx;
}

i32 Table_SetByIter(Iter *it, void *_a, void *_value){
    Abstract *a = (Abstract *)_a;
    Abstract *value = (Abstract *)_value;

    if(a->type.of == TYPE_STR || a->type.of == TYPE_STRVEC &&
        Equals(a, Str_FromCstr(ErrStream->m, "config",ZERO))){
    }

    Hashed *h = Table_GetSetHashed(it, SPAN_OP_SET, a, value);
    return h->idx;
}

void *Table_FromIdx(Table *tbl, i32 idx){
    Hashed *h = (Hashed *)Span_Get(tbl, idx);
    if(h != NULL){
        return h->value;
    }
    return NULL;
}

i32 Table_GetIdx(Table *tbl, void *a){
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
