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

status Serve_CloseReq(Serve *sctx, Req *req, int idx){
    Queue_Remove(sctx->queue, idx);
    close(req->fd);
    MemCtx_Free(req->m);
    return SUCCESS;
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
        req->fd = new_fd;
        req->handler = sctx->def->getHandler(sctx, req);
        if(DEBUG_SERVE){
            Debug_Print(req, 0, "Accept req: ", DEBUG_SERVE, TRUE);
            printf("\n");
        }
        Queue_Add(sctx->queue, (Abstract *)req); 
        return req->type.state;
    }

    return NOOP;
}

status Serve_ServeRound(Serve *sctx){
    status r = READY;
    Queue *q = sctx->queue;

    if(sctx->queue->span.nvalues == 0){
        return NOOP;
    }

    while(TRUE){
        QueueIdx *qidx = Queue_Next(sctx->queue);
        if((q->span.type.state & END) != 0 || (r & ERROR) != 0){
            q->span.type.state &= ~END;
            break;
        }

        if(qidx == NULL || qidx->item == NULL){
            Error("bad req from queue");
            continue;
        }

        Req *req = (Req *)qidx->item;
        sctx->active = req;

        if((req->type.state & (END|ERROR)) != 0){
            int logStatus = ((req->type.state & ERROR) != 0) ? 1 : 0;
            Log(logStatus, "Served %s - mem: %ld/%ld", req->proto->toLog(req), MemCtx_Used(req->m), MemCount());
            r |= Serve_CloseReq(sctx, req, q->sq.idx);
        }else{
            if(DEBUG_REQ){
                Debug_Print((void *)req, 0, "ServeReq_Handle: ", DEBUG_REQ, FALSE);
                printf("\n");
            }

            Handler *h = Handler_Get(req->handler);
            status hstate = h->type.state & (SUCCESS|ERROR);
            if(hstate != 0){
                if(DEBUG_REQ){
                    Debug_Print((void *)req, 0, "ServeReq_Handle(END): ", DEBUG_REQ, FALSE);
                    printf("\n");
                }
                req->type.state |= hstate|END;
            }else{
                if(DEBUG_REQ){
                    printf("\x1b[%dm   calling handler %s\x1b[0m\n", DEBUG_REQ, State_ToString(h->type.state));
                }
                h->func(h, req, sctx);
            }
            r |= req->type.state;
        }
    }

    return r;
}

status Serve_Stop(Serve *sctx){
    close(sctx->socket_fd);
    return SUCCESS;
}

status Serve_PreRun(Serve *sctx, int port){
    int fd = openPortToFd(port);
    sctx->port = port;
    sctx->socket_fd = fd;

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
    sctx->queue = Queue_Make(sctx->m, def->getDelay);
    return sctx;
}
