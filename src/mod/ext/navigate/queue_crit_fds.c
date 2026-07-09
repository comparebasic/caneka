#include <external.h>
#include <caneka.h>

gobits QueueCrit_Fds(QueueCrit *crit, util *values){
    gobits go = 0;
    util *u = values;
    for(i32 i = 0; i < CRIT_SLAB_STRIDE; i++){
        if(*u == 0){
            ((struct pollfd *)u)->fd = -1;
        }
        u++;
    }
    struct pollfd *pfds = (struct pollfd *)values;
    i32 ready = poll(pfds, CRIT_SLAB_STRIDE, 0);

    util base = 1;
    for(i32 i = 0; i < CRIT_SLAB_STRIDE && ready > 0; i++){
        struct pollfd *pfd = pfds+i;

        if(poll(pfd, 1, 0) > 0){
            go |= (base << i);
            ready--;
        }
    }
    return go;
}

gobits QueueCrit_WorkFds(QueueCrit *crit, util *values){
    gobits go = 0;
    util *u = values;
    for(i32 i = 0; i < CRIT_SLAB_STRIDE; i++){
        if(*u == 0){
            ((struct pollfd *)u)->fd = -1;
        }
        u++;
    }

    struct pollfd *pfds = (struct pollfd *)values;
    util base = 1;
    for(i32 i = 0; i < CRIT_SLAB_STRIDE; i++){
        struct pollfd *pfd = pfds+i;

        boolean internal = pfd->fd > 0 && (pfd->events & (POLLIN|POLLOUT)) == 0;
        if(internal || poll(pfd, 1, 0) > 0){
            go |= (base << i);
        }
    }
    return go;
}
