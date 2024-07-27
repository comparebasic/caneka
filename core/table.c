#include <external.h>
#include <caneka.h>

int TABLE_DIM_LOOKUPS[7] = {0, 15, 255, 4095, 65535, 1048575, 0};
int TABLE_DIM_BYTESIZES[7] = {0, 1, 1, 2, 2, 4, 0};
int TABLE_REQUERY_MAX[7] = {0, 2, 4, 8, 16, 32, 0};
#define MAX_POSITIONS ((sizeof(h->id)*2) - dims);

static Hashed *Table_GetSetHashed(Span *tbl, byte op, Abstract *a, Abstract *value);

static int getReQueryKey(i64 hash, int position, byte dim){
    return (int) ((hash >> (position*4)) & TABLE_DIM_LOOKUPS[dim]);
}

static status Table_Resize(Span *tbl, word *queries){
    if(tbl->nvalues > ((TABLE_DIM_LOOKUPS[tbl->dims] / 2) + (TABLE_DIM_LOOKUPS[tbl->dims] / 4)) 
            || *queries > TABLE_REQUERY_MAX[tbl->dims]){
        *queries = 0;
        Span *newTbl = Span_Make(tbl->m);
        Span_Set(newTbl, TABLE_DIM_LOOKUPS[tbl->dims]+1, NULL);
        for(int i = 0; i <= tbl->max_idx; i++){
            Hashed *h = (Hashed *)Span_Get(tbl, i);
            if(h != NULL){
                Table_GetSetHashed(newTbl, SPAN_OP_SET, (Abstract *)h, (Abstract *)h->value);
            }
        }
        memcpy(tbl, newTbl, sizeof(Span));
        return SUCCESS;
    }
    return NOOP;
}

static Hashed *Table_GetSetHashed(Span *tbl, byte op, Abstract *a, Abstract *value){
    Hashed *h = Hashed_Make(tbl->m, a);
    if(op != SPAN_OP_GET && op != SPAN_OP_SET){
        return h;
    }
    Hashed *_h = NULL;
    h->value = value;
    Abstract *v = NULL;
    boolean found = FALSE;
    word queries = 0;
    for(int i = 0; !found && i < tbl->dims && i < TABLE_MAX_DIMS; i++){
        for(int j = 0; !found && j < TABLE_REQUERY_MAX[tbl->dims]; j++){
            queries++;
            Table_Resize(tbl, &queries);
            int hkey = getReQueryKey(h->id, j, tbl->dims);
            if(DEBUG_TABLE){
                Debug_Print((void *)h->item, 0, "Placing ", DEBUG_TABLE, FALSE);
                printf("\x1b[%dm dim %hu offset %d key:", DEBUG_TABLE, tbl->dims, j);
                Bits_Print((byte *)&hkey, TABLE_DIM_BYTESIZES[tbl->dims], "", DEBUG_TABLE, TRUE);
                Bits_Print((byte *)&(h->id), sizeof(h->id), " of ", DEBUG_TABLE, FALSE);
                printf("\x1b[%dm query=%d>\x1b[0m\n", DEBUG_TABLE, queries+1);
            }
            _h = (Hashed *)Span_Get(tbl, hkey);
            if(op == SPAN_OP_GET){
                if(_h != NULL){
                    while(_h != NULL){
                        if(Hashed_Equals(h, _h)){
                            h = _h;
                            found = TRUE;
                            break;
                        }
                        _h = _h->next;
                    }
                }else{
                    found = TRUE;
                    break;
                }
            }else if(op == SPAN_OP_SET){
                if(_h != NULL){
                    while(_h != NULL){
                        if(Hashed_Equals(h, _h)){
                            h = _h;
                            h->idx = hkey;
                            h->value = value;
                            found = TRUE;
                            break;
                        }
                        _h = _h->next;
                    }
                }else{
                    h->idx = hkey;
                    Span_Set(tbl, hkey, (Abstract *)h);
                    found = TRUE;
                    break;
                }
            }
        }
    }
    return h;
}

status Table_SetKey(Span *tbl, Abstract *a){
    Hashed *h = Table_GetSetHashed(tbl, SPAN_OP_SET, a, NULL);
    return SUCCESS;
}

status Table_SetValue(Span *tbl, Abstract *a){
    Hashed *h = Span_Get(tbl, tbl->metrics.set);
    if(h != NULL){
        h->value = a;
        return SUCCESS;
    }
    return ERROR;
}

Abstract *Table_Get(Span *tbl, Abstract *a){
    Hashed *h = Table_GetSetHashed(tbl, SPAN_OP_GET, a, NULL);
    return h->value;
}

int Table_Set(Span *tbl, Abstract *a, Abstract *value){
    Hashed *h = Table_GetSetHashed(tbl, SPAN_OP_SET, a, value);
    h->value = value;
    return h->idx;
}

int Table_GetIdx(Span *tbl, Abstract *a){
    Hashed *h = Table_GetSetHashed(tbl, SPAN_OP_GET, a, NULL);
    if(h->type.state == SUCCESS){
        return h->idx;
    }else{
        return -1;
    }
}
