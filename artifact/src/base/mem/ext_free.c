#include <external.h>
#include <caneka.h>

ExtFree *ExtFree_Make(MemCh *m, ExtFreeFunc func, void *arg){
    ExtFree *ex = MemCh_Alloc(m, sizeof(ExtFree));
    ex->type.of = TYPE_EXTERNAL_FREE;
    ex->func = func;
    ex->arg = arg;
    return ex;
}
