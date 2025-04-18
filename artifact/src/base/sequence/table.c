#include <external.h>
#include <caneka.h>

const query_max = 4;
static i64 dim_nvalue_max[TABLE_MAX_DIMS] = {12, 192, 3072, 49152, 786432};
#define MAX_POSITIONS ((sizeof(h->id)*2) - dims);

static inline i32 HKey_valFromHKey(HKey *hk, i8 dim){
    /* TODO: make the algorithm pull
        The first upper dim
        then all smaller values inside that section of the hash
        then move on the the higher bits at the higher dim
    */
    if(hk->type.state & NOOP){
        hk->pos++;
        if(hk->pos > 4 && dim == 0){ 
            ;
        }
    }
    if(hk->pos > 4 && dim == 0){ 
        hk->type.state |= END;
    }
    hk->idx = (int) ((hk->id >> (position*dim*4)) & _modulos[dim+1]);
    return hk->type.state;
}

static Hashed *Table_GetSetHashed(Span *tbl, word op, Abstract *a, Abstract *value){
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
    HKey hk = {{TYPE_HKEY,0} tbl->dims, -1, h->id};
    i8 dims = tbl->dims;
    while(HKey_valFromHKey(hk, dims) != END){
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

Hashed *Table_GetHashed(Span *tbl, Abstract *a){
    Hashed *h = Table_GetSetHashed(tbl, SPAN_OP_GET, a, NULL);
    if(h->value != NULL){
        return h;
    }
    return NULL;
}

Hashed *Table_SetHashed(Span *tbl, Abstract *a, Abstract *value){
    Hashed *h = Table_GetSetHashed(tbl, SPAN_OP_SET, a, value);
    return h;
}

Abstract *Table_GetKey(Span *tbl, i32 idx){
    Hashed *h = Span_Get(tbl, idx);
    return h->item;
}

Abstract *Table_Get(Span *tbl, Abstract *a){
    Hashed *h = Table_GetSetHashed(tbl, SPAN_OP_GET, a, NULL);
    return h->value;
}

i32 Table_Set(Span *tbl, Abstract *a, Abstract *value){
    Hashed *h = Table_GetSetHashed(tbl, SPAN_OP_SET, a, value);
    return h->idx;
}

Abstract *Table_FromIdx(Span *tbl, i32 idx){
    Hashed *h = (Hashed *)Span_Get(tbl, idx);
    if(h != NULL){
        return h->value;
    }
    return NULL;
}

int Table_GetIdx(Span *tbl, Abstract *a){
    Hashed *h = Table_GetSetHashed(tbl, SPAN_OP_GET, a, NULL);
    if(tbl->type.state & SUCCESS){
        return h->idx;
    }else{
        return -1;
    }
}
