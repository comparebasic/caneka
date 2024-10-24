#include <external.h>
#include <caneka.h>

status Persisted_Save(MemCtx *m, MemCtx *keyed, String *path, IoCtx *ioctx){
    /* re-label */;
    /* write oset with memory coordinates for index table */
    /* save memory slabs to disk with */
    return SUCCESS;
}

MemCtx *Persisted_Load(MemCtx *m, String *path, IoCtx *ioctx){
    /* restore the MemCtx into memory */;
    return NULL;
}
