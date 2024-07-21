#include <external.h>
#include <caneka.h>

int TABLE_DIM_LOOKUPS[7] = {0, 15, 255, 4095, 65535, 1048575, 0};
int TABLE_REQUERY_MAX[7] = {0, 2, 4, 8, 16, 32, 0};
#define MAX_POSITIONS ((sizeof(h->id)*2) - dims);

Hashed *Table_GetSetHashed(Span *tbl, Abstract *a, Abstract *value);

static int getReQueryKey(i64 hash, int position, byte dim){
    return (int) ((hash >> (position*4)) & TABLE_DIM_LOOKUPS[dim]);
}

status Table_SetKey(Span *tbl, Abstract *a){
    return NOOP;
}

status Table_SetValue(Span *tbl, Abstract *a){
    return NOOP;
}

status Table_Resize(Span *tbl, word *queries){
    /*
    if(DEBUG_TABLE_EXPAND){
        printf("nvalues:%u vs %u queries:%u vs %u\n",
           tbl->nvalues,
           (TABLE_DIM_LOOKUPS[tbl->dims] / 2), 
           *queries,  TABLE_REQUERY_MAX[tbl->dims]);
    }
    */
    if(tbl->nvalues > ((TABLE_DIM_LOOKUPS[tbl->dims] / 2) + (TABLE_DIM_LOOKUPS[tbl->dims] / 4)) || *queries > TABLE_REQUERY_MAX[tbl->dims]){
        *queries = 0;
        Span *newTbl = Span_Make(tbl->m);
        Span_Set(newTbl, TABLE_DIM_LOOKUPS[tbl->dims]+1, NULL);
        for(int i = 0; i <= tbl->max_idx; i++){
            Hashed *h = (Hashed *)Span_Get(tbl, i);
            if(h != NULL){
                Table_GetSetHashed(newTbl, (Abstract *)h, (Abstract *)h->value);
            }
        }
        memcpy(tbl, newTbl, sizeof(Span));
        return SUCCESS;
    }
    return NOOP;
}

Hashed *Table_GetSetHashed(Span *tbl, Abstract *a, Abstract *value){
    Hashed *h = Hashed_Make(tbl->m, a);
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
                printf("\x1b[%dmhkey of j:%d hkey:%u from %lu\x1b[0m\n", DEBUG_TABLE, j, hkey, h->id);
            }
            _h = (Hashed *)Span_Get(tbl, hkey);
            if(value == NULL){
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
            }else{
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

Abstract *Table_Get(Span *tbl, Abstract *a){
    Hashed *h = Table_GetSetHashed(tbl, a, NULL);
    return h->value;
}

int Table_Set(Span *tbl, Abstract *a, Abstract *value){
    Hashed *h = Table_GetSetHashed(tbl, a, value);
    h->value = value;
    return h->idx;
}

int Table_GetIdx(Span *tbl, Abstract *a){
    Hashed *h = Table_GetSetHashed(tbl, a, NULL);
    if(h->type.state == SUCCESS){
        return h->idx;
    }else{
        return -1;
    }
}
