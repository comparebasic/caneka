#include <external.h>
#include <caneka.h>

static i64 dim_nvalue_max[TABLE_MAX_DIMS] = {8, 128, 2048, 3000, 50000};
#define MAX_POSITIONS ((sizeof(h->id)*2) - dims);

static inline Hashed *Table_setHValue(MemCh *m, HKey *hk, Iter *it, Abstract *key, Abstract *value){
    Hashed *h = Hashed_Make(m, key);
    h->idx = hk->idx;
    h->value = value;
    Iter_Set(it, h);
    return h;
}

static inline Hashed *Table_getOrSet(Table *tbl, word op, Iter *it, HKey *hk, Abstract *key, Abstract *value, util hash){
    Hashed *h = NULL;
    if(op & SPAN_OP_GET){
        if(it->value != NULL){
            h = it->value;
            if(h->id == hash && Equals(key, h->key)){
                h = (Hashed *)it->value;
                tbl->type.state |= SUCCESS;
            }
        }
    }else if(op & SPAN_OP_SET){
        if(it->value != NULL){
            h = (Hashed *)it->value;
            if(h->id == hash && Equals(key, h->key)){
                h->idx = hk->idx;
                h->value = value;
                tbl->type.state |= SUCCESS;
            }
        }else{
            h = Table_setHValue(tbl->m, hk, it, key, value);
            tbl->type.state |= SUCCESS;
        }
    }

    if(tbl->nvalues > dim_nvalue_max[tbl->dims]){
        i8 dims = tbl->dims;
        Iter_ExpandTo(it, _capacity[tbl->dims]);
    }

    return h;
}

static Hashed *Table_GetSetHashed(Iter *it, word op, Abstract *key, Abstract *value){
    Table *tbl = (Table *)it->p;
    Abstract *args[5];
    tbl->type.state &= ~SUCCESS;
    if(key == NULL){
        return NULL;
    }

    if(tbl->type.state & DEBUG){
        args[0] = (Abstract *)key;
        args[1] = NULL;
        Out("Key $\n", args);
    }

    Hashed *h = NULL;
    util hash = Get_Hash(key);

    HKey hk;
    Table_HKeyInit(&hk, tbl->dims, hash);
    i32 count = 0;
    while((tbl->type.state & SUCCESS) == 0 &&
            (Table_HKeyVal(&hk) & END) == 0){
        Iter_GetByIdx(it, hk.idx);
        h = Table_getOrSet(tbl, op, it, &hk, key, value, hash);
    }

    if((tbl->type.state & SUCCESS) == 0){
        Table_HKeyInit(&hk, tbl->dims, hash);
        hk.pos = 3;
        Table_HKeyVal(&hk);
        Iter_GetByIdx(it, hk.idx);
        while((tbl->type.state & SUCCESS) == 0 &&
            (Iter_Next(it) & END) == 0){
            hk.idx = it->idx;
            h = Table_getOrSet(tbl, op, it, &hk, key, value, hash);
        }
    }

    if((tbl->type.state & SUCCESS) == 0){
        hk.idx = it->p->max_idx+1;
        Iter_GetByIdx(it, hk.idx);
        h = Table_getOrSet(tbl, op, it, &hk, key, value, hash);
    }

    return h;
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
