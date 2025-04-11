#include <external.h>
#include <caneka.h>

int TABLE_DIM_BYTESIZES[TABLE_MAX_DIMS] = {1, 1, 2, 2, 4};
int TABLE_REQUERY_MAX[TABLE_MAX_DIMS] = {8, 8, 4, 4, 2};
static i64 dim_lookups[TABLE_MAX_DIMS] = {15, 255, 4095, 65535, 1048575};
#define MAX_POSITIONS ((sizeof(h->id)*2) - dims);

static Hashed *Table_GetSetHashed(Span *tbl, word op, Abstract *a, Abstract *value);

static int getReQueryKey(i64 hash, int position, byte dim){
    return (int) ((hash >> (position*TABLE_DIM_BYTESIZES[dim])) & dim_lookups[dim]);
}

static boolean shouldResize(Span *tbl, word *queries){
    if(dim_lookups[tbl->dims] == 0){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Dim lookup value not set dim");
        return FALSE;
    }
    if(tbl->nvalues > ((dim_lookups[tbl->dims] / 2) + (dim_lookups[tbl->dims] / 4)) 
            || *queries > TABLE_REQUERY_MAX[tbl->dims]){
            *queries = 0;
            return TRUE;
    }
    return FALSE;
}

static status Table_Resize(Span *tbl, word *queries){
    if(shouldResize(tbl, queries)){
        *queries = 0;
        Span *newTbl = Span_Make(tbl->m);

        Iter it;
        Iter_Setup(&it, newTbl, SPAN_OP_RESIZE, 0);
        Span_Set(tbl, _increments[tbl->dims], NULL);

        for(int i = 0; i <= tbl->max_idx; i++){
            Hashed *h = (Hashed *)Span_Get(tbl, i);
            if(h != NULL){
                if(h != NULL){
                    Table_GetSetHashed(newTbl, SPAN_OP_SET, (Abstract *)h, (Abstract *)h->value);
                }
            }
        }
        memcpy(tbl, newTbl, sizeof(Span));
        return SUCCESS;
    }
    return NOOP;
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
    for(int i = 0; !found && i <= tbl->dims && i < TABLE_MAX_DIMS; i++){
        for(int j = 0; !found && j < TABLE_REQUERY_MAX[tbl->dims]; j++){
            queries++;
            Table_Resize(tbl, &queries);
            int hkey = getReQueryKey(h->id, j, tbl->dims);
            _h = (Hashed *)Span_Get(tbl, hkey);
            if(op == SPAN_OP_GET){
                if(_h != NULL){
                    if(Hashed_Equals(h, _h)){
                        h = _h;
                        found = TRUE;
                    }
                }
            }else if(op == SPAN_OP_SET){
                if(_h != NULL){
                    if(Hashed_Equals(h, _h)){
                        h = _h;
                        h->idx = hkey;
                        h->value = value;
                        found = TRUE;
                        break;
                    }
                }else{
                    h->idx = hkey;
                    h->value = value;
                    Span_Set(tbl, hkey, (Abstract *)h);
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

int Table_Set(Span *tbl, Abstract *a, Abstract *value){
    Hashed *h = Table_GetSetHashed(tbl, SPAN_OP_SET, a, value);
    return h->idx;
}

Abstract *Table_FromIdx(Span *tbl, int idx){
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
