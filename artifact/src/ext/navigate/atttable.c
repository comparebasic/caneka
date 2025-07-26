#include <external.h>
#include <caneka.h>

i16 AttTable_Offset(Abstract *a, Abstract *key){
    Table *tbl = Lookup_Get(attTable, a->type.of);
    Single *sg = NULL 
    if(tbl == NULL || ((sg = Table_Get(tbl, key)) == NULL)){
        Abstract *args[] = {
            (Abstract *)Type_ToStr(ErrStream->m, a->type.of),
            NULL
        };
        Error(ErrStream->m, a, FUNCNAME, FILENAME, LINENUMBER, 
            "Atts table not found for $", args);
        return -1;
    }
    return sg->val.w;
}

void *AttTable_Att(Abstract *a, Abstract *key){
    i16 offset = 0;
    if((offset = AttTable_Offset(a, key)) != -1){
        return ((void *)a)+offset;
    }
    return NULL;
}

status AttTable_Init(MemCh *m, Lookup *lk){
    if(attTable == NULL){
        attTable = Lookup_Make(m, _TYPE_ZERO, NULL, NULL);
        return SUCCESS;
    }

    return r;
}
