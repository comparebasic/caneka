#include <external.h>
#include <caneka.h>

int TABLE_DIM_LOOKUPS[6] = {15, 255, 4095, 65535, 1048575, 0};

static int getReQueryKey(i64 hash, int position, byte dim){
    return (int) ((hash >> (position*4)) & TABLE_DIM_LOOKUPS[dim]);
}

status Table_SetKey(Span *tbl, Abstract *a){
    return NOOP;
}

status Table_SetValue(Span *tbl, Abstract *a){
    return NOOP;
}

Hashed *Table_GetHashed(Span *tbl, Abstract *a){
    Hashed *h = Hashed_Make(tbl->m, a);
    int dims = tbl->dims; 
    Abstract *v = NULL;
    boolean found = FALSE;
    int maxPositions = (sizeof(h->id)*2) - dims;
    for(int i = 0; !found && i < dims && i < TABLE_MAX_DIMS; i++){
        for(int j = 0; !found && j < maxPositions; j++){
            int hkey = getReQueryKey(h->id, j, dims);
            v = Span_Get(tbl, hkey);
            if(v != NULL && v->type.of == TYPE_HASHED){
                Hashed *_h = (Hashed *)v;
                while(_h != NULL){
                    if(tbl->eq(a, _h->item)){
                        return _h;
                    }
                    
                    _h = _h->next;
                }
            }else{
                break;
            }
        }
    }
    return h;
}

Abstract *Table_Get(Span *tbl, Abstract *a){
    Hashed *h = Table_GetHashed(tbl, a);
    if(h->type.state == SUCCESS){
        return h->value;
    }else{
        return NULL;
    }
}

int Table_GetIdx(Span *tbl, Abstract *a){
    Hashed *h = Table_GetHashed(tbl, a);
    if(h->type.state == SUCCESS){
        return h->idx;
    }else{
        return -1;
    }
}
