#include <external.h>
#include <caneka.h>

PfdArr *PfdArr_Make(MemCh *m, i32 length){
    PfdArr *arr = MemCh_AllocOf(m, sizeof(PfdArr), TYPE_POLL_FD_ARR);
    arr->pfds = (struct pollfd *)Bytes_Alloc(m,
        (word)(sizeof(struct pollfd)*length), TYPE_BYTE);
    return arr;
}
