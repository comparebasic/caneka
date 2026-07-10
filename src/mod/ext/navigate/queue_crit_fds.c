#include <external.h>
#include <caneka.h>

boolean QueueCrit_Fds(QueueCrit *crit, util *u){
    if(*u == 0){
        ((struct pollfd *)u)->fd = -1;
    }
    struct pollfd *pfd = (struct pollfd *)u;
    return pfd->fd && 
        (pfd->events & (POLLIN|POLLOUT) == 0) || 
        poll((struct pollfd *)u, 1, 0) == 1;
}
