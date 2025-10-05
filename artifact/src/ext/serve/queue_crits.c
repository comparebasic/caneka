#include <external.h>
#include <caneka.h>

gobits QueueCrit_Fds(QueueCrit *crit, void **items, void *values){
    word go = 0;
    i32 localIdx = 0;
    struct pollfd *fds = (struct pollfd *)values;
    i32 ready = poll(fds, SPAN_STRIDE, 1);
    for(i32 i = 0; i < SPAN_STRIDE && ready > 0; i++){
        if(poll(fds, 1, 1) > 0){
            go |= (1 << i);
            ready--;
        }
        fds++;
    }
    return go;
}
