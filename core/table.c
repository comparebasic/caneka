#include <external.h>
#include <caneka.h>

int TABLE_DIM_LOOKUPS[7] = {0, 15, 255, 4095, 65535, 1048575, 0};
#define TABLE_REQUERY_MAX 4

static int getReQueryKey(i64 hash, int position, byte dim){
    return (int) ((hash >> (position*4)) & TABLE_DIM_LOOKUPS[dim]);
}

status Table_SetKey(Span *tbl, Abstract *a){
    return NOOP;
}

status Table_SetValue(Span *tbl, Abstract *a){
    return NOOP;
}

Hashed *Table_GetSetHashed(Span *tbl, Abstract *a, Abstract *value){
    Hashed *h = Hashed_Make(tbl->m, a);
    h->value = value;
    int dims = tbl->dims; 
    Abstract *v = NULL;
    boolean found = FALSE;
    int requery = TABLE_DIM_LOOKUPS[dims] / TABLE_REQUERY_MAX;
    int queries = 0;
    int maxPositions = (sizeof(h->id)*2) - dims;
    for(int i = 0; !found && i < dims && i < TABLE_MAX_DIMS; i++){
        for(int j = 0; !found && j < maxPositions; j++){
            if(++queries >= requery){
                dims++;
                requery = TABLE_DIM_LOOKUPS[dims] / TABLE_REQUERY_MAX;
                queries = 0;
            }
            int hkey = getReQueryKey(h->id, j, dims);
            if(DEBUG_TABLE){
                printf("\x1b[%dmhkey of j:%d hkey:%u from %lu\x1b[0m\n", DEBUG_TABLE, j, hkey, h->id);
            }
            v = Span_Get(tbl, hkey);
            if(value == NULL){
                if(v != NULL){
                    Hashed *_h = (Hashed *)v;
                    while(_h != NULL){
                        if(tbl->eq(a, _h->item)){
                            h = _h;
                            found = TRUE;
                            break;
                        }
                        _h = _h->next;
                    }
                }
            }else{
                if(v != NULL){
                    Hashed *_h = (Hashed *)v;
                    while(_h != NULL){
                        if(tbl->eq(a, _h->item)){
                            h = _h;
                            h->idx = hkey;
                            Span_Set(tbl, hkey, (Abstract *)h);
                            break;
                        }
                        _h = _h->next;
                    }
                }else{
                    h->idx = hkey;
                    Span_Set(tbl, hkey, (Abstract *)h);
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
