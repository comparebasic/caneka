#include <external.h>
#include <caneka.h>

static void delay(){
    struct timespec ts = {
        ROUND_DELAY_SEC,
        ROUND_DELAY_TVSEC,
    };
    struct timespec remaining;
    nanosleep(&ts, &remaining);
}


static int openPortToFd(int port){
    int fd = 0;
	struct sockaddr_in serv_addr;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);

    if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
        Fatal("openPortToFd setting nonblock\n", TYPE_SERVECTX);
		return -1;
    }

    int one = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)) < 0) {
        Fatal("openPortToFd setting reuse addr\n", TYPE_SERVECTX);
		return -1;
	}
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &one, sizeof(int)) < 0) {
        Fatal("openPortToFd setting reuse addr\n", TYPE_SERVECTX);
		return -1;
	}

	if(bind(fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) != 0){
        Fatal("openPortToFd binding\n", TYPE_SERVECTX);
		return -1;
    }

	if(listen(fd, 10) != 0){
        Fatal("openPortToFd listening\n", TYPE_SERVECTX);
		return -1;
    };

    return fd;
}

static status Serve_EpollEvUpdate(Serve *sctx, Req *req, int direction){
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

static status Serve_EpollEvAdd(Serve *sctx, Req *req, int fd, int direction){
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

static status Serve_EpollEvRemove(Serve *sctx, Req* req){
    int r = epoll_ctl(sctx->epoll_fd, EPOLL_CTL_DEL, req->fd, NULL);
    if(r != 0){
        Error("Failed to remove file descriptor to epoll");
        return ERROR;
    }

    return SUCCESS;
}

static status Serve_CloseReq(Serve *sctx, Req *req){
    status r = Serve_EpollEvRemove(sctx, req);
    if(r == SUCCESS){
        close(req->fd);
        MemCtx_Free(req->m);
        return SUCCESS;
    }
    return r;
}

status Serve_Respond(Serve *sctx, Req *req){
    /*
    if(req->out.cursor->state != COMPLETE){
        SCursor_Prepare(req->out.cursor, SERV_WRITE_SIZE); 
        size_t l = write(req->fd, req->out.cursor->seg->bytes, req->out.cursor->position);
        status r = SCursor_Incr(req->out.cursor, l);
        if(r == COMPLETE){
            req->state = COMPLETE;
        }
    }

    if(req->out.cursor->position >= req->out.response->length){
        req->state = COMPLETE;
    }
    */

    return req->type.state;
}

status Serve_AcceptRound(Serve *sctx){
    int new_fd = accept(sctx->socket_fd, (struct sockaddr*)NULL, NULL);
    if(new_fd > 0){
        fcntl(new_fd, F_SETFL, O_NONBLOCK);

        if(DEBUG_SERVE){
            Debug_Print(sctx->def, 0, "Accept proto: ", DEBUG_SERVE, TRUE);
            printf("\n");
        }
        Req *req = (Req *)sctx->def->req_mk((MemHandle *)sctx, (Abstract *)sctx);
        req->handler = sctx->def->getHandler(sctx, req);
        if(DEBUG_SERVE){
            Debug_Print(req, 0, "Accept req: ", DEBUG_SERVE, TRUE);
            printf("\n");
        }
        status r = Serve_EpollEvAdd(sctx, req, new_fd, EPOLLIN); 
        return req->type.state;
    }

    return NOOP;
}

status ServeReq_Handle(Serve *sctx, Req *req){
    sctx->active = req;
    if(DEBUG_REQ){
        Debug_Print((void *)req, 0, "ServeReq_Handle: ", DEBUG_REQ, FALSE);
        printf("\n");
    }

    Handler *h = Handler_Get(req->handler);
    int direction = req->direction;
    status hstate = h->type.state & (SUCCESS|ERROR);
    if(hstate != 0){
        if(DEBUG_REQ){
            Debug_Print((void *)req, 0, "ServeReq_Handle(END): ", DEBUG_REQ, FALSE);
            printf("\n");
        }
        req->type.state |= hstate|END;
    }else{
        h->func(h, req, sctx);
    }

    if(req->direction != -1 && direction != req->direction){
        Serve_EpollEvUpdate(sctx, req, req->direction);
    }

    return req->type.state;
}

status Serve_ServeRound(Serve *sctx){
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
            Log(logStatus, "Served %s - mem: %ld", req->proto->toLog(req), MemCount());
            r = Serve_CloseReq(sctx, req);
        }else{
            ServeReq_Handle(sctx, req);
        }
    }

    return SUCCESS;
}

status Serve_Stop(Serve *sctx){
    close(sctx->socket_fd);
    close(sctx->epoll_fd);
    return SUCCESS;
}

status Serve_PreRun(Serve *sctx, int port){
    int fd = openPortToFd(port);
    sctx->port = port;
    sctx->socket_fd = fd;

	int epoll_fd =  epoll_create1(0);
	if (epoll_fd == -1) {
		Fatal("Failed to create epoll file descriptor\n", TYPE_SERVECTX);
		return ERROR;
	}

    sctx->epoll_fd = epoll_fd;
    sctx->serving = TRUE;
    return SUCCESS;
}

status Serve_Run(Serve *sctx, int port){
    status r = Serve_PreRun(sctx, port);
    if(r == SUCCESS){
        int cadance = 0;
        while(sctx->serving){
            if(cadance-- == 0){
                Serve_AcceptRound(sctx);
                cadance = ACCEPT_CADANCE;
            }
            Serve_ServeRound(sctx);
            delay();
        }

        return SUCCESS;
    }
    return r;
}

Serve *Serve_Make(MemCtx *m, ProtoDef *def){
    Serve *sctx = (Serve *)MemCtx_Alloc(m, sizeof(Serve)); 
    sctx->m = m;
    sctx->def = def;
    return sctx;
}
