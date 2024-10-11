#include <external.h>
#include <caneka.h>

status Queue_SetFlags(void **stack, word set, word unset, quad delayTicks){
    void **sl = stack;
    int dim = 0;
    while(*sl != NULL){
        printf("set_flags %hu\n", dim++);
        QueueIdx *qidx = *sl;
        qidx->flags |= set;
        qidx->flags &= ~unset;
        if(qidx->delayTicks == 0 || delayTicks < qidx->delayTicks){
            qidx->delayTicks = delayTicks;
        }
        sl++;
    }
    return SUCCESS;
}

void *Queue_Set(Span *p, int idx, void *value, quad delayTicks){
    printf("Queue Set\n");
    if(idx < 0 || value == NULL){
        return NULL;
    }
    SlabResult sr;
    SlabResult_Setup(&sr, p, SPAN_OP_SET, idx);
    void *ptr = _span_Set(&sr, idx, (Abstract *)value);
    word set = SLAB_ACTIVE;
    word unset = 0;
    if(sr.local_idx == (sr.span->def->stride-1)){
        set |= SLAB_FULL;
    }else{
        unset |= SLAB_FULL;
    }
    Queue_SetFlags(sr.stack, set, unset, delayTicks);
    return ptr;
}

