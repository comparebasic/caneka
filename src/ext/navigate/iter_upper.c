#include <external.h>
#include <caneka.h>

void *IterUpper_Combine(MemCh *m, void *orig, void *add){
    if(orig == NULL){
        return add;
    }else{
        Abstract *a = (Abstract *)orig;
        if(a->type.of == TYPE_ITER){
            Iter *it = (Iter *)a;
            Iter_Add(it, add);
            return it;
        }else{
            Iter *it = (Iter *)Iter_Make(m, Span_Make(m));
            Iter_Add(it, orig);
            Iter_Add(it, add);
            return it;
        }
    } 
}
