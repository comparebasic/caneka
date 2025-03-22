#include <external.h>
#include <caneka.h>

i32 Queue_Add(Queue *q, Abstract *value){
    return -1;
}

Abstract *Queue_Next(Queue *q){
    return NULL;
}

status Queue_Remove(Queue *q, int idx){
    return NOOP;
}

Queue *Queue_Make(MemCtx *m, GetDelayFunc getDelay){
    return NULL;
}
