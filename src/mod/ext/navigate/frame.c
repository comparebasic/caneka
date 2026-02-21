#include <external.h> 
#include <caneka.h> 

Frame *Frame_Make(MemCh *m, i32 originIdx, void *originKey, void *value, Span *p){
    Frame *fm = (Frame *)MemCh_Alloc(m, sizeof(Frame));
    fm->type.of = TYPE_FRAME;
    fm->originIdx = originIdx;
    fm->originKey = (Abstract *)originKey;
    fm->value = (Abstract *)value;
    Iter_Init(&fm->it, p);
    return fm;
}
