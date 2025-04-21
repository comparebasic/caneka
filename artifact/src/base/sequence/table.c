#include <external.h>
#include <caneka.h>

static i64 dim_nvalue_max[TABLE_MAX_DIMS] = {8, 128, 2048, 3000, 50000};
#define MAX_POSITIONS ((sizeof(h->id)*2) - dims);

static inline status Table_HKeyVal(Table *tbl, HKey *hk){
    i8 dimp = hk->dim+1;
    if((hk->type.state & PROCESSING) == 0){
        hk->type.state |= PROCESSING;
    }else{
        if(hk->pos == 4){
            if(hk->dim == 0){
                hk->type.state |= END;
            }else{
                hk->dim--;
                hk->pos = 0;
            }
        }else{
            hk->pos++;
        }
    }
    hk->idx = (i32) ((hk->id >> (hk->pos*(dimp)*4)) & _modulos[dimp]);
    return hk->type.state;
}

static inline Hashed *Table_setHValue(MemCh *m, HKey *hk, Iter *it, Abstract *key, Abstract *value){
    Hashed *h = Hashed_Make(m, key);
    h->idx = hk->idx;
    h->value = value;
    Iter_Set(it, h);
    return h;
}

static status HKey_Init(HKey *hk, Table *tbl, util id){
    hk->type.of = TYPE_HKEY;
    hk->type.state = 0;
    hk->idx = 0;
    hk->id = id;
    hk->dim = tbl->dims;
    hk->pos = 0;
    return SUCCESS;
}

static Hashed *Table_GetSetHashed(Table *tbl, word op, Abstract *key, Abstract *value){
    tbl->type.state &= ~SUCCESS;
    if(key == NULL){
        return NULL;
    }
    Hashed *h = NULL;
    util hash = Get_Hash(key);

    Iter it;
    Iter_Setup(&it, tbl, SPAN_OP_GET|SPAN_OP_RESERVE, 0);

    it.type.state |= DEBUG;

    HKey hk;
    HKey_Init(&hk, tbl, hash);
    i32 count = 0;
    while((tbl->type.state & SUCCESS) == 0 &&
            (Table_HKeyVal(tbl, &hk) & END) == 0){
        it.idx = hk.idx;
        Iter_Query(&it);
        if(tbl->type.state & DEBUG){
            void *args[] = {&hk, it.value, NULL};
            printf("value:%p %d\n", it.value, (i32)it.type.state);
            Out("^p.Table GetSetHashed _D/_d^0.\n", args);
        }
        if(op == SPAN_OP_GET){
            if(it.value != NULL){
                h = it.value;
                if(h->id == hash && Equals(key, h->item)){
                    h = (Hashed *)it.value;
                    tbl->type.state |= SUCCESS;
                }
            }
        }else if(op == SPAN_OP_SET){
            if(it.value != NULL){
                h = (Hashed *)it.value;
                if(h->id == hash && Equals(key, h->item)){
                    h->idx = hk.idx;
                    h->value = value;
                    tbl->type.state |= SUCCESS;
                }
            }else{
                printf("Setting %d\n", it.idx);
                h = Table_setHValue(tbl->m, &hk, &it, key, value);
                tbl->type.state |= SUCCESS;
            }
        }
        if(tbl->nvalues > dim_nvalue_max[tbl->dims]){
            printf("Resizing\n");
            Iter_Setup(&it, tbl, SPAN_OP_ADD, _capacity[tbl->dims]);
            Iter_Query(&it);
            Iter_Setup(&it, tbl, SPAN_OP_GET|SPAN_OP_RESERVE, 0);
        }
    }

    if(op == SPAN_OP_SET && (tbl->type.state & SUCCESS) == 0){
        printf("BOTTOM STUFF\n");
        /*
        HKey_Init(&hk, tbl, hash);
        Iter_Setup(&it, tbl, SPAN_OP_GET, hk.idx);
        while((tbl->type.state & SUCCESS) == 0 &&
                (Iter_Next(&it) & END) == 0){
            if(it.value == NULL){
                h = Table_setHValue(tbl->m, &hk, &it, key, value);
                tbl->type.state |= SUCCESS;
            }
        }
        if((tbl->type.state & SUCCESS) == 0){
            Iter_Setup(&it, tbl, SPAN_OP_ADD, it.idx);
            h = Table_setHValue(tbl->m, &hk, &it, key, value);
            tbl->type.state |= SUCCESS;
        }
        */
    }

    return h;
}

status Table_SetKey(Iter *it, Abstract *a){
    Hashed *h = Table_GetSetHashed(it->span, SPAN_OP_SET, a, NULL);
    /* set idx here */
    return SUCCESS;
}

i32 Table_SetIdxEntry(Iter *it, Abstract *a){
    Hashed *h = Table_GetSetHashed(it->span, SPAN_OP_SET, a, NULL);
    i32 value = it->metrics.set;
    Single *tag = Int_Wrapped(it->span->m, value);
    h->value = (Abstract *)tag;
    return (i32)value;
}

Hashed *Table_SetValue(Iter *it, Abstract *a){
    Hashed *h = Span_Get(it->span, it->metrics.set);
    if(h != NULL){
        h->value = a;
        it->metrics.set = -1;
        return h;
    }
    return NULL;
}

Hashed *Table_GetHashed(Table *tbl, Abstract *a){
    Hashed *h = Table_GetSetHashed(tbl, SPAN_OP_GET, a, NULL);
    if(h->value != NULL){
        return h;
    }
    return NULL;
}

Hashed *Table_SetHashed(Table *tbl, Abstract *a, Abstract *value){
    Hashed *h = Table_GetSetHashed(tbl, SPAN_OP_SET, a, value);
    return h;
}

Abstract *Table_GetKey(Table *tbl, i32 idx){
    Hashed *h = Span_Get(tbl, idx);
    return h->item;
}

Abstract *Table_Get(Table *tbl, Abstract *a){
    Hashed *h = Table_GetSetHashed(tbl, SPAN_OP_GET, a, NULL);
    return h->value;
}

i32 Table_Set(Table *tbl, Abstract *a, Abstract *value){
    Hashed *h = Table_GetSetHashed(tbl, SPAN_OP_SET, a, value);
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
    Hashed *h = Table_GetSetHashed(tbl, SPAN_OP_GET, a, NULL);
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
