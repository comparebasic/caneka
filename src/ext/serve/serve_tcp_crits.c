#include <external.h>
#include <caneka.h>

gobits QueueCrit_Fds(QueueCrit *crit, util *values){
    word go = 0;
    util *u = values;
    for(i32 i = 0; i < SPAN_STRIDE; i++){
        if(*u == 0){
            ((struct pollfd *)u)->fd = -1;
        }
        struct pollfd *pfd = (struct pollfd *)u;
        if(pfd->fd > 0){
            printf("fd%d in%d out%d\n", pfd->fd, pfd->events & POLL_IN, pfd->events & POLL_OUT); 
        }
        u++;
    }
    struct pollfd *pfds = (struct pollfd *)values;
    i32 ready = poll(pfds, SPAN_STRIDE, 1);

    printf("crit fds ready %d\n", ready);
    fflush(stdout);

    word base = 1;
    for(i32 i = 0; i < SPAN_STRIDE && ready > 0; i++){
        struct pollfd *pfd = pfds+i;

        printf("poll %d\n", pfd->fd);
        fflush(stdout);

        if(poll(pfd, 1, 1) > 0){
            go |= (base << i);
            ready--;
        }
    }
    return go;
}
