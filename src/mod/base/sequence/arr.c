/* Base.sequence.Arr
 *
 * Utility functions for creating void *[] and Str*[]
 * 
 * Used much less frequently than the type-wrapped Span object
 *
 */

#include <external.h>
#include "base_module.h"

void **Span_ToArr(MemCh *m, Span *p){
    if((p->nvalues+1) > MAX_PTR_ARR){
        void *args[] = {
            I32_Wrapped(m, MAX_PTR_ARR),
            I32_Wrapped(m, p->nvalues),
            NULL
        };
        Error(m, FUNCNAME, FILENAME, LINENUMBER, 
            "Too many values to make into a static array of pointers, max is $, have $", 
            args
        );
        return NULL;
    }
    size_t sz = sizeof(void *) * (p->nvalues+1);
    void **arr = (void **)MemCh_Alloc(m, sz);
    Iter it;
    Iter_Init(&it, p);
    i32 i = 0;
    while((Iter_Next(&it) & END) == 0){
        if(i > p->nvalues){
            Fatal(m, FUNCNAME, FILENAME, LINENUMBER, 
                "nvalue mismatch", NULL); 
            return NULL;
        }
        if(it.value != NULL){
            arr[i++] = it.value;
        }
    }
    return arr;
}

void **Arr_Make(MemCh *m, i32 nvalues){
    if(nvalues > MAX_PTR_ARR){
        void *args[] = {
            I32_Wrapped(m, MAX_PTR_ARR),
            I32_Wrapped(m, nvalues),
            NULL
        };
        Error(m, FUNCNAME, FILENAME, LINENUMBER, 
            "Too many values to make into a static array of pointers, max is $, have $", 
            args
        );
        return NULL;
    }
    size_t sz = sizeof(void *) * (nvalues+1);
    return (void **)Bytes_Alloc((m), sz, TYPE_POINTER_ARRAY);
}

void *TypedArr_Get(MemCh *m, TypedArr *arr, i32 idx){
    if(idx >= arr->rangeType.range){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Idx is our of range", NULL);
        arr->type.state |= ERROR;
        return NULL;
    }
    void **dptr = arr->data+idx;
    return *dptr;
}

void TypedArr_Set(MemCh *m, TypedArr *arr, i32 idx, void *value){
    if(idx >= arr->rangeType.range){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Idx is our of range", NULL);
        arr->type.state |= ERROR;
        return;
    }
    void **dptr = arr->data+idx;
    *dptr = value;
}

TypedArr *TypedArr_Make(MemCh *m, cls typeOf, i16 size){
    TypedArr *arr = MemCh_Alloc(m, sizeof(TypedArr));
    arr->type.of = TYPE_TYPED_ARR;
    arr->data = (void **)Bytes_Alloc(m, size *sizeof(void *), TYPE_BYTES_POINTER);
    arr->rangeType.range = size;
    return arr;
}
