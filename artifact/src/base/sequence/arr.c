#include <external.h>
#include <caneka.h>

Abstract **Span_ToArr(MemCh *m, Span *p){
    if((p->nvalues+1) > MAX_PTR_ARR){
        Abstract *args[] = {
            (Abstract *)I32_Wrapped(m, MAX_PTR_ARR),
            (Abstract *)I32_Wrapped(m, p->nvalues),
            NULL
        };
        Error(m, FUNCNAME, FILENAME, LINENUMBER, 
            "Too many values to make into a static array of pointers, max is $, have $", 
            args
        );
        return NULL;
    }
    size_t sz = sizeof(Abstract *)*(p->nvalues+1);
    Abstract **arr = (Abstract **)MemCh_Alloc(m, sz);
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

Abstract **Arr_Make(MemCh *m, i32 nvalues){
    if(nvalues > MAX_PTR_ARR){
        Abstract *args[] = {
            (Abstract *)I32_Wrapped(m, MAX_PTR_ARR),
            (Abstract *)I32_Wrapped(m, nvalues),
            NULL
        };
        Error(m, FUNCNAME, FILENAME, LINENUMBER, 
            "Too many values to make into a static array of pointers, max is $, have $", 
            args
        );
        return NULL;
    }
    size_t sz = sizeof(Abstract *)*(nvalues+1);
    return (Abstract **)Bytes_Alloc((m), sz, TYPE_POINTER_ARRAY);
}
