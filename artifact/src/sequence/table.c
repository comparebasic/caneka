#include <external.h>
#include <caneka.h>

int TABLE_DIM_BYTESIZES[7] = {1, 1, 2, 2, 4, 0, 0};
int TABLE_REQUERY_MAX[7] = {4, 4,  8, 16, 32, 0, 0};
#define MAX_POSITIONS ((sizeof(h->id)*2) - dims);

static void setValuePtr(Span *tbl, Hashed *h, Abstract *value){
    LocalPtr lp;
    h->value = value;
}

static Hashed *Table_GetSetHashed(Span *tbl, byte op, Abstract *a, Abstract *value);

static int getReQueryKey(SpanDef *def, i64 hash, int position, byte dim){
    return (int) ((hash >> (position*4)) & def->dim_lookups[dim]);
}

static boolean shouldResize(Span *tbl, word *queries){
    SpanDef *def = tbl->def;
    if(def->dim_lookups[tbl->dims] == 0){
        Fatal("Dim lookup value not set for SpanDef\n", tbl->type.of);
        return FALSE;
    }
    if(tbl->nvalues > ((def->dim_lookups[tbl->dims] / 2) + (def->dim_lookups[tbl->dims] / 4)) 
            || *queries > TABLE_REQUERY_MAX[tbl->dims]){
            *queries = 0;
            return TRUE;
    }
    return FALSE;
}

static status Table_Resize(Span *tbl, word *queries){
    SpanDef *def = tbl->def;
    if(shouldResize(tbl, queries)){
        *queries = 0;
        Span *newTbl = Span_Make(tbl->m, TYPE_TABLE);

        SpanQuery sr;
        SpanQuery_Setup(&sr, newTbl, SPAN_OP_RESIZE, 0);
        sr.dimsNeeded = tbl->dims+1;
        Span_GrowToNeeded(&sr);

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

static Hashed *Table_GetSetHashed(Span *tbl, byte op, Abstract *a, Abstract *value){
    if(a == NULL){
        return NULL;
    }
    Hashed *h = Hashed_Make(tbl->m, a);
    tbl->type.state &= ~SUCCESS;
    if(op != SPAN_OP_GET && op != SPAN_OP_SET){
        return h;
    }
    Hashed *_h = NULL;
    if(op != SPAN_OP_GET && value != NULL){
        setValuePtr(tbl, h, value);
    }
    Abstract *v = NULL;
    boolean found = FALSE;
    word queries = 0;
    for(int i = 0; !found && i <= tbl->dims && i < TABLE_MAX_DIMS; i++){
        for(int j = 0; !found && j < TABLE_REQUERY_MAX[tbl->dims]; j++){
            queries++;
            Table_Resize(tbl, &queries);
            int hkey = getReQueryKey(tbl->def, h->id, j, tbl->dims);
            _h = (Hashed *)Span_Get(tbl, hkey);
            if(DEBUG_TABLE){
                Debug_Print((void *)h->item, 0, "At ", DEBUG_TABLE, FALSE);
                printf("\x1b[%dm dimsize=%hu key/%lu=", DEBUG_TABLE, tbl->dims, tbl->def->dim_lookups[tbl->dims]);
                Bits_Print((byte *)&hkey, TABLE_DIM_BYTESIZES[tbl->dims], "", DEBUG_TABLE, TRUE);
                printf("\x1b[%dm%d half-byte of ", DEBUG_TABLE, j+1);
                Bits_Print((byte *)&(h->id), sizeof(int), "", DEBUG_TABLE, FALSE);
                printf("\x1b[%dm query=%d>\x1b[0m\n", DEBUG_TABLE, queries);
            }
            if(op == SPAN_OP_GET){
                if(_h != NULL && *((util *)_h) != 0){
                    if(Hashed_ExternalEquals(tbl->m, h, _h)){
                        h = _h;
                        found = TRUE;
                    }
                }
            }else if(op == SPAN_OP_SET){
                if(_h != NULL && *((util *)_h) != 0){
                    if(Hashed_LocalEquals(tbl->m, h, _h)){
                        h = _h;
                        h->idx = hkey;
                        h->value = value;
                        found = TRUE;
                        break;
                    }
                }else{
                    h->idx = hkey;
                    h = (Hashed *)Span_Set(tbl, hkey, (Abstract *)h);
                    if(tbl->type.of == TYPE_ORDERED_TABLE){
                        OrdTable *otbl = (OrdTable *)as(tbl, TYPE_ORDERED_TABLE);
                        Span_Add(otbl->order, (Abstract *)h);
                    }
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

status Table_SetKey(Span *tbl, Abstract *a){
    Hashed *h = Table_GetSetHashed(tbl, SPAN_OP_SET, a, NULL);
    return SUCCESS;
}

Hashed *Table_SetValue(Span *tbl, Abstract *a){
    Hashed *h = Span_Get(tbl, tbl->metrics.set);
    if(h != NULL){
        h->value = a;
        tbl->metrics.set = -1;
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

Abstract *Table_Get(Span *tbl, Abstract *a){
    Hashed *h = Table_GetSetHashed(tbl, SPAN_OP_GET, a, NULL);
    return h->value;
}

int Table_Set(Span *tbl, Abstract *a, Abstract *value){
    Hashed *h = Table_GetSetHashed(tbl, SPAN_OP_SET, a, value);
    return h->idx;
}

/* untested */
status Table_Merge(Span *tbl, Span *oldTbl){
    Iter it;
    Iter_Init(&it, oldTbl);
    status r = READY;
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = (Hashed *)Iter_Get(&it);
        if(h != NULL){
            h = Hashed_Clone(tbl->m, h);
            if(Table_GetSetHashed(tbl, SPAN_OP_SET, h->item, h->value) != NULL){
                r |= SUCCESS;
            }else{
                r |= ERROR;
                break;
            }
        }
    }
    if(r == READY){
        r |= NOOP;
    }
    return r;
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
