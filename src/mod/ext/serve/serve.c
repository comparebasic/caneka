#include <external.h>
#include <caneka.h>

static i16 _g = 0;
static void ServeTcp_AcceptPoll(Serve *srv){
    MemCh *m = srv->m;
    Debug_Push(m, srv);

    srv->type.state &= ~SUCCESS;
    void *args[8];

    Time_Now(&srv->q->time.present);

    i64 timeout = 0;
    if(srv->q->it.p->nvalues == 0){
        timeout = TCP_ZERO_REQ_DELAY;
    }

    i32 available = poll(ent->pfd, srv->endPointIt.p->nvalues, timeout);
    if(available == -1){
        args[0] = Str_CstrRef(ErrStream->m, strerror(errno));
        args[1] = NULL;
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error connecting to any files or sockets: $\n", args);
        srv->type.state |= ERROR;

        ReturnVoid(m);
    }

    if(available > 0){
        while((Iter_Next(&srv->endPointIt) & END) == 0){
            Hashed *h = Iter_Get(&srv->endPointIt);
            HostEnt *ent = (HostEnt *)h->key;
            HandlerDef *def = (HandlerDef *)h->value;
            if(ent->type.of == TYPE_HOST_ENT){
                struct sockaddr_in cliaddr;
                socklen_t len = sizeof(cliaddr);
                i32 max = 16;
                while(max-- > 0){
                    i32 new_fd = accept(ent->pfd->fd, (struct sockaddr*)&cliaddr, &len);
                    if(new_fd > 0){
                        fcntl(new_fd, F_SETFL, O_NONBLOCK);

                        MemCh *rm = MemCh_Make();
                        TcpSource *ts = (TcpSource *)srv->source;
                        ts->new_fd = new_fd;
                        ts->addr = NetAddr_Make4(m);
                        memcpy(&ts->addr->net.ip4addr, &cliaddr, sizeof(cliaddr));

                        Req *req = Req_Make(m, def, def->extra(m, srv, def));
                        Time_Now(&req->metrics.start);
                        req->idx = Queue_Add(srv->q, req, req->crit);

                        if(srv->type.state & DEBUG){
                            Buff_SetTemp(OutStream); 
                            void *ar[] = {
                               I32_Wrapped(OutStream->m, new_fd), 
                               I32_Wrapped(OutStream->m, req->idx), 
                               I32_Wrapped(OutStream->m, srv->q->it.p->nvalues), 
                               NULL
                            };
                            Out("^y.NewConnection fd:$ qIdx:$ $requests now^0\n", ar);
                        }
                    }else{
                        break;
                    }
                }
            }
        }
    }

    srv->type.state |= (NOOP|PROCESSING);
    while((Queue_Next(srv->q) & END) == 0){
        if(srv->type.state & DEBUG){
            Buff_SetTemp(OutStream); 
            void *ar[] = {
               I32_Wrapped(OutStream->m, srv->q->it.idx), 
               NULL
            };
            Out("^y.   Handling qIdx:$ ", ar);
        }
        srv->type.state &= ~(NOOP|PROCESSING);
        Req *req = (Req *)Queue_Get(srv->q);

        srv->def->handle(req->m, req, srv);
        if(req->type.state & END){
            srv->def->finalize(req->m, req, srv);
            Queue_Remove(srv->q, req->idx);
            MemCh_Free(req->m);
        }
    }

    ReturnVoid(m);
}

struct pollfd *Serve_TcpGetPollFd(Req *req){
     return (struct pollfd *)&req->crit->pfd;
}

void Serve_Serve(Serve *srv){
    srv->pfds = MemCh_Alloc(srv->m,
        sizeof(struct pollfd)*srv->routIt.p->nvalues, TYPE_POLLFD_PTR); 
    while((Iter_Next(&srv->endPointIt) & END) == 0){
        Abstract *a = Iter_Get(&srv->endPointIt);
        if(a->type.of == TYPE_HOST_ENT){
            HostEnt *ent = (HostEnt *)a;
            ent->pfd = &srv->pfds[srv->endPointIt.idx];
            HostEnt_OpenTcp(ent);
        }else if(a->type.of == TYPE_BUFF){
            Buff *bf = (Buff *)a;
            if(bf->pfd != NULL){
                memcpy(srv->pfds+srv->endPointIt.idx, bf->pfd, sizeof(struct pollfd));
            }
            bf->pfd = &srv->pfds[srv->endPointIt.idx];
            /* setup request for file in Q */
        }
    }

    while((srv->type.state & ERROR) == 0){
        ServeTcp_AcceptPoll(srv);
    }

    return;
}


void Serve_LogOpen(Serve *srv, Req *req){
    srv->def->log.open(srv->m, req, srv);
}

void Serve_LogFinalized(Serve *srv, Req *req){
    srv->def->log.final(srv->m, req, srv);
}

Serve *Serve_Make(MemCh *m, Table *routes /* <HostEnt, HandlerDef> */, void *source){
    Serve *srv = MemCh_AllocOf(m, sizeof(Serve), TYPE_SERVE);
    srv->type.of = TYPE_SERVE;
    srv->m = m;
    srv->q = Queue_Make(m, (QueueCritFunc)ReqCrit_Func);
    srv->source = source;
    Iter_Init(&srv->endPointIt, routes);
    srv->log.out = OutStream;

    return srv;
}
