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

status Serve_OpenTcp(Handler *h){
    ServeTcpArg *args = as(h->args, TYPE_SERVE_TCP_ARG);
    i32 fd = openPortToFd(args->port);
    if(fd > 0){
        h->type.state |= SUCCESS;
    }
    return h->type.state;
}

status Serve_AcceptPoll(Serve *sctx, i32 delay){
    status r = READY;
    Abstract *args[5];
    i32 new_fd = 0;
    struct pollfd pfd = {
        sctx->socket_fd,
        POLLIN,
        0
    };
    i32 available = min(poll(&pfd, 1, delay), ACCEPT_AT_ONEC_MAX);
    if(available == -1){
        args[0] = (Abstract *)Str_CstrRef(ErrStream->m, strerror(errno));
        args[1] = NULL;
        Error(ErrStream->m, (Abstract *)sctx), 
            FUNCNAME, FILENAME, LINENUMBER,
            "Error connecting to test socket: $\n", args);
        sctx->type.state |= ERROR;
        return sctx->type.state;
    }
    i32 accepted = 0;
    while(available-- > 0){
        new_fd = accept(sctx->socket_fd, (struct sockaddr*)NULL, NULL);
        if(new_fd > 0){
            sctx->metrics.open++;
            fcntl(new_fd, F_SETFL, O_NONBLOCK);
            Req *req = (Req *)sctx->makers.req((Abstract *)sctx, NULL);
            req->chain = sctx->makers.handlers(sctx, req);
            req->idx = Queue_Add(sctx->q, (Abstract *)req);
            Handler *h = Handler_Get(req->chain);
            Queue_SetCriteria(q, 0, req->idx, (util *)&h->id);
            accepted++;
            r |= req->type.state;
        }else{
            break;
        }
    }

    return r;
}


status Serve_ServeRound(Serve *sctx){
    DebugStack_Push("Serve_ServeRound", TYPE_CSTR); 
    status r = READY;
    /*
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
    */
    DebugStack_Pop();
    return r;
}
