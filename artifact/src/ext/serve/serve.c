#include <external.h>
#include <caneka.h>

status Serve_CloseReq(Serve *sctx, i32 idx){
    Req *req = Span_Get(sctx->q->itemsIt.p, idx);
    Queue_Remove(sctx->q, idx);
    struct pollfd *pfd = *req->id;
    close(pfd->fd);
    sctx->metrics.open--;
    if((req->type.state & ERROR) != 0){
        sctx->metrics.error++;
    }else{
        sctx->metrics.served++;
    }
    if(req->m->owner == NULL || req->m->owner == (Abstract *)req){
        MemCh_Free(req->m);
    }
    return SUCCESS;
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

status Serve_Stop(Serve *sctx){
    status r = READY;
    /*
    Queue *q = &sctx->queue;
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
    */
    return r;
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

status Serve_Run(Serve *sctx){
    if(sctx->start->func(sctx->start->func) & SUCCESS){
        sctx->type.state |= SERVE_RUNNING;
        serve->metrics.start = MicroTime_Now();
        while(sctx->type.state & SERVE_RUNNING){
            Serve_AcceptPoll(sctx, SERVE_DELAY);
            Serve_ServeRound(sctx);
            sctx->metrics.ticks++;
        }
        return SUCCESS;
    }
    return ERROR;
}

Serve *Serve_Make(MemCtx *m, ServeMaker req, ServeMaker handlers){
    Serve *sctx = (Serve *)MemCtx_Alloc(m, sizeof(Serve)); 
    sctx->type.of = TYPE_SERVECTX;
    sctx->m = m;
    sctx->q = Queue_Make(m);
    sctx->socket_fd = -1;
    QueueCrit *crit = QueueCrit_Make(m, QueueCrit_Fds, ZERO);
    Queue_AddHandler(q, crit);
    return sctx;
}
