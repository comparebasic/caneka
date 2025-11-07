#include <external.h>
#include <caneka.h>

gobits QueueCrit_Fds(QueueCrit *crit, util *values){
    word go = 0;
    util *u = values;
    for(i32 i = 0; i < SPAN_STRIDE; i++){
        if(*u == 0){
            ((struct pollfd *)u)->fd = -1;
        }
        u++;
    }
    struct pollfd *pfds = (struct pollfd *)values;
    i32 ready = poll(pfds, SPAN_STRIDE, 1);

    word base = 1;
    for(i32 i = 0; i < SPAN_STRIDE && ready > 0; i++){
        struct pollfd *pfd = pfds+i;

        if(poll(pfd, 1, 1) > 0){
            go |= (base << i);
            ready--;
        }
    }
    return go;
}
