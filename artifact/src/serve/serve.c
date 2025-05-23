#include <external.h>
#include <caneka.h>

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

static int pollSkipSlab(Abstract *source, int idx){
    DebugStack_Push("pollSkipSlab", TYPE_CSTR); 
    Serve *sctx = as(source, TYPE_SERVECTX);
    int ready = 0;
    SpanQuery sq;
    SpanQuery_Setup(&sq, sctx->pollMap, SPAN_OP_GET, idx);
    Span_Query(&sq);
    if(sq.stack[0].slab != NULL){
        ready += poll(sq.stack[0].slab, SPAN_STRIDE, 1);
        if(DEBUG_SERVE_POLLING){
            struct pollfd *pfd = sq.stack[0].slab;
            printf("\x1b[%dmPoll Found %d, first fd:%d events:%d\x1b[0m\n", DEBUG_SERVE_POLLING, ready, pfd->fd, pfd->events);
        }
    }

    SpanQuery_Setup(&sq, sctx->flagMap, SPAN_OP_GET, idx);
    Span_Query(&sq);
    if(sq.stack[0].slab != NULL){
        for(int i = 0; i < SPAN_STRIDE; i++){
            status *ptr = (status *)Slab_valueAddr(sq.stack[0].slab, i);
            if((*ptr) != 0){
                ready++;
            }
        }
    }

    DebugStack_Pop();
    return ready;
}

void Delay(){
    struct timespec ts = {
        ROUND_DELAY_SEC,
        ROUND_DELAY_TVSEC,
    };
    struct timespec remaining;
    nanosleep(&ts, &remaining);
}

status Serve_ClobberFlags(Serve *sctx, Req *req, status flags){
    i64 slot = 0;
    slot += flags;
    if(Span_Set(sctx->flagMap, req->queueIdx, (Abstract *)&slot) != NULL){
        return SUCCESS;
    }
    return ERROR;
}

status Serve_CloseReq(Serve *sctx, Req *req, int idx){
    Serve_ClobberFlags(sctx, req, ZERO);
    Queue_Remove(&(sctx->queue), idx);
    close(req->fd);
    sctx->metrics.open--;
    if((req->type.state & ERROR) != 0){
        sctx->metrics.error++;
    }else{
        sctx->metrics.served++;
    }
    if(req->m->owner == NULL || req->m->owner == (Abstract *)req){
        MemCtx_Free(req->m);
    }
    return SUCCESS;
}

status Serve_SetPollFds(Serve *sctx, Req *req){
    struct pollfd pfd = {
        req->fd,
        req->handler->direction,
        POLLHUP
    };
    Span_Set(sctx->pollMap, req->queueIdx, (void *)&pfd);
    return SUCCESS;
}

status Serve_AcceptPoll(Serve *sctx, int delay){
    status r = READY;
    int new_fd = 0;
    struct pollfd pfd = {
        sctx->socket_fd,
        POLLIN,
        POLLHUP
    };
    int available = min(poll(&pfd, 1, delay), ACCEPT_AT_ONEC_MAX);
    if(available == -1){
        LogError("Error connecting to test socket %s\n", strerror(errno));
    }
    int accepted = 0;
    while(available-- > 0){
        new_fd = accept(sctx->socket_fd, (struct sockaddr*)NULL, NULL);
        if(new_fd > 0){
            accepted++;
            sctx->metrics.open++;
            fcntl(new_fd, F_SETFL, O_NONBLOCK);
            Req *req = (Req *)sctx->def->req_mk(NULL, (Abstract *)sctx, sctx->type.state);
            req->fd = new_fd;
            req->handler = sctx->def->getHandler(sctx, req);
            if(DEBUG_SERVE){
                Debug_Print(sctx->def, 0, "Accept proto: ", DEBUG_SERVE, TRUE);
                printf("\n");
            }
            if(DEBUG_SERVE){
                Debug_Print(req, 0, "Accept req: ", DEBUG_SERVE, TRUE);
                printf("\n");
            }
            req->queueIdx = Queue_Add(&(sctx->queue), (Abstract *)req); 
            Serve_SetPollFds(sctx, req);
            r |= req->type.state;
        }else{
            printf("none");
            break;
        }
    }

    if(DEBUG_SERVE_ACCEPT){
        printf("\x1b[%dmAccepted %d new connections:", DEBUG_SERVE_ACCEPT, accepted);
        Debug_Print(sctx, 0, "", DEBUG_SERVE_ACCEPT, FALSE);
        printf("\n");
    }

    return r;
}

status Serve_ServeRound(Serve *sctx){
    DebugStack_Push("Serve_ServeRound", TYPE_CSTR); 
    status r = READY;
    Queue *q = &sctx->queue;

    if(q->count == 0){
        DebugStack_Pop();
        return NOOP;
    }

    while(TRUE){
        /*
        user poll on a slab of pollfds here to determine if the whole block should be skipped
        */
        QueueIdx *qidx = Queue_Next(q, pollSkipSlab);
        if(DEBUG_SERVE_ROUNDS){
            Debug_Print(sctx, 0, "Round Serve: ", DEBUG_SERVE_ROUNDS, TRUE);
            printf("\n");
        }
        if((q->type.state & END) != 0 || (r & ERROR) != 0){
            q->type.state &= ~END;
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
            Log(logStatus, "Served %s - mem: %ld/%ld - QIdx:%d", req->proto->toLog(req), MemCtx_Used(req->m), MemCount(0), sctx->queue.current.idx);
            r |= Serve_CloseReq(sctx, req, q->current.idx);
        }else{
            if(DEBUG_REQ){
                Debug_Print((void *)req, 0, "ServeReq_Handle: ", DEBUG_REQ, FALSE);
                printf("\n");

                char *msg = req->proto->toLog(req);
                printf("\x1b[%dmmsg: %s\x1b[0m\n",DEBUG_REQ, msg);
            }

            Handler *h = Handler_Get(req->handler);
            if(h == NULL || (h->type.state & (SUCCESS|ERROR)) != 0){
                if(DEBUG_REQ){
                    Debug_Print((void *)req, 0, "ServeReq_Handle(END): ", DEBUG_REQ, FALSE);
                    printf("\n");
                }
                if(h == NULL){
                    req->type.state |= ERROR;
                }else{
                    req->type.state |= (h->type.state & (SUCCESS|ERROR))|END;
                }
            }else{
                h->func(h, req, sctx);
                if(DEBUG_REQ){
                    printf("\x1b[%dm   called handler %s\x1b[0m\n", DEBUG_REQ, State_ToChars(h->type.state));
                }
            }
            r |= req->type.state;
        }
    }

    sctx->metrics.ticks++;
    Delay();
    DebugStack_Pop();
    return r;
}

status Serve_Stop(Serve *sctx){
    Queue *q = &sctx->queue;
    status r = READY;
    q->type.state &= ~END;
    while((q->type.state & END) == 0 && (r & ERROR) == 0){
        QueueIdx *qidx = Queue_Next(q, NULL);
        if(qidx != NULL){
            Req *req = (Req *)qidx->item;
            sctx->active = req;
            r |= Serve_CloseReq(sctx, req, q->current.idx);
        }
    }
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

Req *Serve_AddFd(MemCtx *m, Serve *sctx, int fd, word flags){
    Req *req = (Req *)sctx->def->req_mk(m, (Abstract *)sctx, flags);
    if(fcntl(fd, F_SETFL, O_NONBLOCK) == -1){
        Fatal("Unable to set non block when adding fd to Req", 0); 
        return NULL;
    }
    req->fd = fd;
    req->handler = sctx->def->getHandler(sctx, req);
    req->queueIdx = Queue_Add(&(sctx->queue), (Abstract *)req); 
    Serve_SetPollFds(sctx, req);

    return req;
}

status Serve_RunPort(Serve *sctx, int port){
    status r = Serve_PreRun(sctx, port);
    if(r == SUCCESS){
        return Serve_Run(sctx);
    }
    return r;
}

status Serve_Run(Serve *sctx){
    sctx->serving = TRUE;
    while(sctx->serving){
        int delay = (sctx->queue.count == 0 ? 
            ACCEPT_LONGDELAY_MILLIS : 
            0);
        Serve_AcceptPoll(sctx, delay);
        Serve_ServeRound(sctx);
        sctx->metrics.ticks++;
    }

    return SUCCESS;
}

status Serve_RunFds(Serve *sctx){
    sctx->serving = TRUE;
    while(sctx->serving){
        int delay = (sctx->queue.count == 0 ? 
            ACCEPT_LONGDELAY_MILLIS : 
            0);
        Serve_ServeRound(sctx);
    }

    return SUCCESS;
}

Serve *Serve_Make(MemCtx *m, ProtoDef *def){
    Serve *sctx = (Serve *)MemCtx_Alloc(m, sizeof(Serve)); 
    sctx->type.of = TYPE_SERVECTX;
    sctx->m = m;
    sctx->def = def;
    sctx->pollMap = Span_Make(m);
    sctx->flagMap = Span_Make(m);
    Queue_Init(m, &sctx->queue, def->getDelay);
    sctx->queue.source = (Abstract *)sctx;
    sctx->socket_fd = -1;
    sctx->port = -1;
    return sctx;
}
