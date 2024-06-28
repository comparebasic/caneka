#include <external.h>
#include <caneka.h>

Abstract *Table_Get(MemCtx *m, Table *tbl, Abstract *a){
    Hashed *h = Hashed_Make(m, a);

    Abstract *v = NULL;
    boolean found = FALSE;
    for(int i = 0; i < tbl->values->dims && i < TABLE_MAX_DIMS; i++){
        int hkey = h->id & TABLE_DIM_LOOKUPS[i];
        Abstract *v = Span_Get(tbl->values, hkey);
        if(Compare(a, b)){
            found = TRUE;
            break;
        }
    }
    return v;
}
