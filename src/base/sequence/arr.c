#include <external.h>
#include <caneka.h>
#include "../module.h"

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
            Fatal(FUNCNAME, FILENAME, LINENUMBER, 
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
