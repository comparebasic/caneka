#include <external.h>
#include <caneka.h>

#ifdef LINUX
status Serve_Epoll(Serve *sctx){
    status r = ERROR;

    int ev_count;
	struct epoll_event *curev;
	struct epoll_event event;
    struct epoll_event events[SERV_MAX_EVENTS];
	char buff[SERV_READ_SIZE + 1];

    ev_count = epoll_wait(sctx->epoll_fd, events, SERV_MAX_EVENTS, EPOLL_WAIT);
    if(ev_count == 0){
        return NOOP;
    }

    for(int i = 0; i < ev_count; i++){
        Req *req = (Req *)events[i].data.ptr;
        if(req == NULL){
            Error("bad req from epoll");
            continue;
        }

        if((req->type.state & (END|ERROR)) != 0){
            int logStatus = ((req->type.state & ERROR) != 0) ? 1 : 0;
            Log(logStatus, "Served %s - mem: %ld", req->proto->toLog(req), MemCount(0));
            r = Serve_CloseReq(sctx, req);
        }else{
            ServeReq_Handle(sctx, req);
        }
    }

    return r;
}

status Serve_EpollEvUpdate(Serve *sctx, Req *req, int direction){
    int r;
    struct epoll_event event;

    event.events = direction;
    event.data.ptr = (void *)req;

    r = epoll_ctl(sctx->epoll_fd, EPOLL_CTL_MOD, req->fd, &event);
    if(r != 0){
        Error("Failed to modify file descriptor\n");
        return ERROR;
    }

    return SUCCESS;
}

status Serve_EpollEvAdd(Serve *sctx, Req *req, int fd, int direction){
    int r;
    struct epoll_event event;

    event.events = direction;
    event.data.ptr = (void *)req;

    r = epoll_ctl(sctx->epoll_fd, EPOLL_CTL_ADD, fd, &event);
    if(r != 0){
        Error("Failed to add file descriptor to epoll");
        return ERROR;
    }

    req->fd = fd;
    req->direction = direction;

    return SUCCESS;
}

status Serve_EpollEvRemove(Serve *sctx, Req* req){
    int r = epoll_ctl(sctx->epoll_fd, EPOLL_CTL_DEL, req->fd, NULL);
    if(r != 0){
        Error("Failed to remove file descriptor to epoll");
        return ERROR;
    }

    return SUCCESS;
}

#endif
