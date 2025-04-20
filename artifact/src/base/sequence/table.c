#include <external.h>
#include <caneka.h>

const query_max = 4;
static i64 dim_nvalue_max[TABLE_MAX_DIMS] = {12, 192, 3072, 49152, 786432};
#define MAX_POSITIONS ((sizeof(h->id)*2) - dims);

inline status Table_HKeyVal(Table *tbl, HKey *hk){
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
    hk->idx = (i32) ((hk->id >> (position*hk->dim*4)) & _modulos[hk->dim+1]);
    return hk->type.state;
}

status HKey_Init(HKey *hk, Table *tbl, util id){
    hk->type.state = TYPE_HKEY;
    hk->idx = 0;
    hk->id = id;
    hk->dim = tbl->dims;
    hk->pos = 0;
    return SUCCESS;
}

static Hashed *Table_GetSetHashed(Table *tbl, word op, Abstract *a, Abstract *value){
    Hashed *h = Hashed_Make(tbl->m, a);
    if(a == NULL){
        return NULL;
    }
    tbl->type.state &= ~SUCCESS;
    if(op != SPAN_OP_GET && op != SPAN_OP_SET){
        return h;
    }
    Hashed *_h = NULL;
    Abstract *v = NULL;
    boolean found = FALSE;
    word queries = 0;
    Iter it;
    Iter_Setup(&it, tbl, SPAN_OP_GET, 0);
    HKey hk;
    HKey_Init(&hk, Table *tbl, h->id);
    i8 dims = tbl->dims;
    while(Table_HKeyVal(tbl, &hk) != END){
        it.idx = hk->idx;
        Iter_Query(&it);
        if(op == SPAN_OP_GET){
            if(it.value != NULL){
                if(Hashed_Equals(h, (Hashed *)it.value)){
                    h = (Hashed *)it.value;
                    found = TRUE;
                }
            }
        }else if(op == SPAN_OP_SET){
            if(it.value != NULL){
                if(Hashed_Equals(h, (Hashed *)it.value)){
                    h = (Hashed *)it.value;
                    h->idx = hkey;
                    h->value = value;
                    found = TRUE;
                    break;
                }
            }else{
                h->idx = hkey;
                h->value = value;
                Iter_Set(&it, h);
                found = TRUE;
                break;
            }
        }
    }

    if(found){
        tbl->type.state |= SUCCESS;
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

int Table_GetIdx(Table *tbl, Abstract *a){

Table *Table_Make(MemCh *m){
    return (Table *)Span_Make(m);
}
    Hashed *h = Table_GetSetHashed(tbl, SPAN_OP_GET, a, NULL);
    if(tbl->type.state & SUCCESS){
        return h->idx;
    }else{
        return -1;
    }
}
