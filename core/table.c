#include <external.h>
#include <caneka.h>

int TABLE_DIM_LOOKUPS[6] = {15, 255, 4095, 65535, 1048575, 0};

static int getReQueryKey(i64 hash, int position, byte dim){
    return (int) ((hash >> (position*4)) & TABLE_DIM_LOOKUPS[dim]);
}

status Table_SetKey(Table *tbl, Abstract *a){
    return NOOP;
}

Hashed *Table_GetHashed(Table *tbl, Abstract *a){
    Hashed *h = Hashed_Make(tbl->values->m, a);
    int dims = tbl->values->dims; 
    Abstract *v = NULL;
    boolean found = FALSE;
    int maxPositions = (sizeof(h->id)*2) - dims;
    for(int i = 0; !found && i < dims && i < TABLE_MAX_DIMS; i++){
        for(int j = 0; !found && j < maxPositions; j++){
            int hkey = getReQueryKey(h->id, j, dims);
            Abstract *v = Span_Get(tbl->values, hkey);
            while(v != NULL){
                if(rbl->eq(a, v)){
                    return v;
                }
                v = v->next;
            }
        }
    }
}

Abstract *Table_Get(Table *tbl, Abstract *a){
    Hashed *h = Table_GetHashed(tbl, a);
    if(h->type.status == SUCCESS){
        return h->value;
    }else{
        return NULL;
    }
}

int Table_GetIdx(Table *tbl, Abstract *a){
    Hashed *h = Table_GetHashed(tbl, a);
    if(h->type.status == SUCCESS){
        return h->idx;
    }else{
        return NULL;
    }
}
