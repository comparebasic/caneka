#include <external.h>
#include <caneka.h>

Lookup *AttsTableLookup;

i16 AttTable_Offset(Abstract *a, Abstract *key){
    Table *tbl = Lookup_Get(AttsTableLookup, a->type.of);
    Single *sg = NULL;
    if(tbl == NULL || ((sg = (Single *)Table_Get(tbl, key)) == NULL)){
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

Single *AttTable_Get(Abstract *a, Abstract *key){
    Table *tbl = Lookup_Get(AttsTableLookup, a->type.of);
    Single *sg = NULL;
    if(tbl == NULL || ((sg = (Single *)Table_Get(tbl, key)) == NULL)){
        Abstract *args[] = {
            (Abstract *)Type_ToStr(ErrStream->m, a->type.of),
            NULL
        };
        Error(ErrStream->m, a, FUNCNAME, FILENAME, LINENUMBER, 
            "Atts table not found for $", args);
        return NULL;
    }
    return sg;
}

status AttTable_Att(MemCh *m, Abstract *a, Abstract *key, Abstract **value){
    i16 offset = 0;
    *value = NULL;
    Single *sg = AttTable_Get(a, key);
    if(sg != NULL){
        if(sg->type.of == TYPE_WRAPPED_I16){
            if(sg->objType.of == ZERO || sg->objType.of > _TYPE_RAW_END){
                void **ptr = (void **)(((void *)a)+sg->val.w);
                *value = *ptr; 
                return SUCCESS;
            }else{
                cls type = sg->objType.of;
                if(type == TYPE_UTIL){
                    util *ptr = ((void *)a)+sg->val.w;
                    *value = (Abstract *)Util_Wrapped(m, *ptr); 
                    return SUCCESS;
                }else if(type == TYPE_I32){
                    i32 *ptr = ((void *)a)+sg->val.i;
                    *value = (Abstract *)I32_Wrapped(m, *ptr); 
                    return SUCCESS;
                }else if(type == TYPE_I64){
                    i64 *ptr = ((void *)a)+sg->val.i;
                    *value = (Abstract *)I64_Wrapped(m, *ptr); 
                    return SUCCESS;
                }else if(type == TYPE_I16){
                    i16 *ptr = ((void *)a)+sg->val.w;
                    *value = (Abstract *)I16_Wrapped(m, *ptr);
                    return SUCCESS;
                }else if(type == TYPE_I8){
                    i8 *ptr = ((void *)a)+sg->val.b;
                    *value = (Abstract *)I16_Wrapped(m, *ptr);
                    return SUCCESS;
                }else if(type == TYPE_BYTE){
                    byte *ptr = ((void *)a)+sg->val.b;
                    *value = (Abstract *)B_Wrapped(m, *ptr, ZERO, ZERO); 
                    return SUCCESS;
                }
            }
        }
        Error(m, a, FUNCNAME, FILENAME, LINENUMBER, 
            "Att table type not yet implemented", NULL);
        return ERROR;
    }
    Error(m, a, FUNCNAME, FILENAME, LINENUMBER, 
        "Att table type not found", NULL);
    return ERROR;
}

status AttTable_Init(MemCh *m){
    if(AttsTableLookup == NULL){
        AttsTableLookup = Lookup_Make(m, _TYPE_ZERO, NULL, NULL);
        return SUCCESS;
    }
    return NOOP;
}
