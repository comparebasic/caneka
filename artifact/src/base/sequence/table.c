#include <external.h>
#include <caneka.h>

int TABLE_DIM_BYTESIZES[TABLE_MAX_DIMS] = {1, 1, 2, 2, 4};
int TABLE_REQUERY_MAX[TABLE_MAX_DIMS] = {8, 8, 4, 4, 2};
static i64 dim_lookups[TABLE_MAX_DIMS] = {15, 255, 4095, 65535, 1048575};
static i64 dim_nvalue_max[TABLE_MAX_DIMS] = {12, 192, 3072, 49152, 786432};
#define MAX_POSITIONS ((sizeof(h->id)*2) - dims);

static Hashed *Table_GetSetHashed(Span *tbl, word op, Abstract *a, Abstract *value);

static int getReQueryKey(i64 hash, int position, byte dim){
    return (int) ((hash >> (position*TABLE_DIM_BYTESIZES[dim])) & dim_lookups[dim]);
}

static boolean shouldResize(Span *tbl, word queries){
    if(tbl->nvalues > dim_nvalue_max[tbl->dims] || queries > TABLE_REQUERY_MAX[tbl->dims]){
            return TRUE;
    }
    return FALSE;
}

static status Table_Resize(Span *tbl, word *queries){
    printf("resizing\n");
    *queries = 0;
    Span *newTbl = Span_Make(tbl->m);

    Iter it;
    Iter_Setup(&it, newTbl, SPAN_OP_RESIZE, _capacity[tbl->dims]);
    Iter_Query(&it);

    Iter_Setup(&it, tbl, SPAN_OP_GET, 0);
    newTbl->type.state |= DEBUG;
    while((Iter_Next(&it) & END) == 0){
        printf("iter %d\n", it.idx);
        Hashed *h = (Hashed *)it.value;
        if(h != NULL){
            Out("re-inserting _a _D hKey:_i4 _B\n", h, h->value, h->idx, &h->id, sizeof(util));
            Table_GetSetHashed(newTbl, SPAN_OP_SET, (Abstract *)h, (Abstract *)h->value);
        }
    }
    memcpy(tbl, newTbl, sizeof(Span));
    printf("Resized\n");
    return SUCCESS;
}

static Hashed *Table_GetSetHashed(Span *tbl, word op, Abstract *a, Abstract *value){
    printf("GetSetHased\n");
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
    printf("GetSetHased II\n");
    for(i32 i = 0; !found && i <= tbl->dims && i < TABLE_MAX_DIMS; i++){
        for(i32 j = 0; !found && j < TABLE_REQUERY_MAX[tbl->dims]; j++){
            printf("GetSetHased III i:%d j:%d\n", i, j);
            queries++;
            if(shouldResize(tbl, queries)){
                Table_Resize(tbl, &queries);
            }
            i32 hkey = getReQueryKey(h->id, j, tbl->dims);
            it.idx = hkey;
            printf("GetSetHased III i:%d j:%d about to find %d dims:%d\n", i, j, hkey, it.span->dims);
            Iter_Query(&it);
            printf("GetSetHased III i:%d j:%d about to found\n", i, j);
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
