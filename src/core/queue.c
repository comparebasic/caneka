#include <external.h>
#include <caneka.h>

status Queue_SetFlags(word set, word unset, quad delayTicks){
    void **sl = Span_GetStack();
    while(*sl != NULL){
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

