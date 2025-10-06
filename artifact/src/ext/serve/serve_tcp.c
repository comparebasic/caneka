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
        Error(ErrStream->m, NULL,
            FUNCNAME, FILENAME, LINENUMBER,
            "openPortToFd setting nonblock", NULL);
		return -1;
    }

    int one = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)) < 0) {
        Error(ErrStream->m, NULL,
            FUNCNAME, FILENAME, LINENUMBER,
            "openPortToFd setting reuse addr", NULL);
		return -1;
	}
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &one, sizeof(int)) < 0) {
        Error(ErrStream->m, NULL,
            FUNCNAME, FILENAME, LINENUMBER,
            "openPortToFd setting reuse addr", NULL);
		return -1;
	}

	if(bind(fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) != 0){
        Error(ErrStream->m, NULL,
            FUNCNAME, FILENAME, LINENUMBER,
            "openPortToFd binding", NULL);
		return -1;
    }

	if(listen(fd, 10) != 0){
        Error(ErrStream->m, NULL,
            FUNCNAME, FILENAME, LINENUMBER,
            "openPortToFd listening", NULL);
		return -1;
    };

    return fd;
}

status ServeTcp_OpenTcp(Handler *chain){
    Req *req = (Req *)as(chain->source, TYPE_TUMBLE_REQ);
    TcpCtx *ctx = (TcpCtx *)as(req->source, TYPE_TCP_CTX);
    i32 fd = openPortToFd(ctx->port);
    struct pollfd *pfd = (struct pollfd *)&req->id;
    if(fd > 0){
        pfd->fd = fd;
        pfd->events = POLLIN;
        pfd->revents = 0;
        chain->type.state |= SUCCESS;
    }else{
        chain->type.state |= ERROR;
    }
    return chain->type.state;
}

status ServeTcp_AcceptPoll(Handler *chain){
    status r = READY;
    chain->type.state &= ~SUCCESS;
    Req *req = (Req *)as(chain->source, TYPE_TUMBLE_REQ);
    MemCh *m = req->m;
    Abstract *args[5];
    TcpCtx *ctx = (TcpCtx *)as(req->source, TYPE_TCP_CTX);
    Queue *q = (Queue *)as(req->data, TYPE_QUEUE);
    struct pollfd *pfd = (struct pollfd *)&req->id;
    i32 available = min(poll(pfd, 1, TCP_POLL_DELAY), ACCEPT_AT_ONEC_MAX);
    if(available == -1){
        args[0] = (Abstract *)Str_CstrRef(ErrStream->m, strerror(errno));
        args[1] = NULL;
        Error(ErrStream->m, (Abstract *)ctx, 
            FUNCNAME, FILENAME, LINENUMBER,
            "Error connecting to test socket: $\n", args);
        chain->type.state |= ERROR;
        return chain->type.state;
    }
    i32 accepted = 0;
    i32 new_fd = 0;
    while(available-- > 0){
        new_fd = accept(pfd->fd, (struct sockaddr*)NULL, NULL);
        if(new_fd > 0){
            ctx->metrics.open++;
            Req *child = Req_Make(NULL, NULL, (Abstract *)req);
            
            fcntl(new_fd, F_SETFL, O_NONBLOCK);
            child->chain = ctx->mk(req->m, chain, 
                (Abstract *)I32_Wrapped(req->m, new_fd), (Abstract *)req);

            child->idx = Queue_Add(q, (Abstract *)child);
            Queue_SetCriteria(q, 0, child->idx, &child->id);

            accepted++;
            r |= req->type.state;
        }else{
            break;
        }
    }

    while((Queue_Next(q) & END) == 0){
        Req *child = (Req *)Queue_Get(q);
        Req_Cycle(child);
    }

    if(q->type.state & END){
        chain->type.state |= SUCCESS;
    }

    return chain->type.state;
}


/*
status ServeTcp_Round(Serve *sctx){
    DebugStack_Push("Serve_ServeRound", TYPE_CSTR); 
    status r = READY;
    Queue *q = &sctx->queue;

    if(q->count == 0){
        DebugStack_Pop();
        return NOOP;
    }

    while(TRUE){
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
*/

Req *ServeTcp_Make(TcpCtx *ctx){
    Req *req = Req_Make(NULL, NULL, (Abstract *)ctx);
    MemCh *m = req->m;
    req->chain = Handler_Make(m, ServeTcp_AcceptPoll,  (Abstract *)Queue_Make(m), (Abstract *)ctx);
    Handler_Add(m, req->chain, ServeTcp_OpenTcp, NULL, (Abstract *)ctx);
    req->chain->type.state |= HANDLER_QUEUE;
    return req;
}
