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

    i32 available = poll(srv->endPointPfds->pfds, srv->endPointPfds->length, timeout);
    if(available == -1){
        args[0] = Str_CstrRef(ErrStream->m, strerror(errno));
        args[1] = NULL;
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error connecting to any files or sockets: $\n", args);
        srv->type.state |= ERROR;

        ReturnVoid(m);
    }

    printf("Available %d\n", available);
    fflush(stdout);

    if(available > 0){
        while((Iter_Next(&srv->endPointIt) & END) == 0){
            Hashed *h = Iter_Get(&srv->endPointIt);
            void *ar[] = {
                h, NULL
            };
            Out("^b.Endpoint @^0\n", ar);
            HostEnt *ent = (HostEnt *)h->key;
            HandlerDef *def = (HandlerDef *)h->value;
            if(ent->type.of == TYPE_HOST_ENT){
                struct sockaddr_in cliaddr;
                socklen_t len = sizeof(cliaddr);
                i32 max = 16;
                while(max-- > 0){
                    i32 new_fd = accept(ent->pfd->fd, (struct sockaddr*)&cliaddr, &len);
                    if(new_fd > 0){
                        void *ar[] = {
                            ent, I32_Wrapped(m, new_fd), NULL
                        };
                        Out("^b.Accepting @ fd$^0\n", ar);
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

    void *ar[] = {
        srv->q,
        NULL
    };
    Out("^c.About to call Next on @^0\n", ar);

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

        Req_Handle(req->m, req, srv);

        if(req->type.state & END){
            req->def->finalize(req->m, req, srv);
            Queue_Remove(srv->q, req->idx);
            MemCh_Free(req->m);
        }
    }

    ReturnVoid(m);
}

struct pollfd *Serve_TcpGetPollFd(Req *req){
     return (struct pollfd *)&req->crit->pfd;
}

void Serve_Setup(Serve *srv){
    Debug_Push(srv->m, srv);
    srv->endPointPfds = PfdArr_Make(srv->m, srv->endPointIt.p->nvalues);
    while((Iter_Next(&srv->endPointIt) & END) == 0){
        Hashed *h = Iter_Get(&srv->endPointIt);
        if(h == NULL){
            continue;
        }
        if(((Abstract *)h->key)->type.of == TYPE_HOST_ENT){
            HostEnt *ent = (HostEnt *)h->key;
            ent->pfd = &srv->endPointPfds->pfds[srv->endPointIt.idx];
            ent->type.state |= DEBUG;
            HostEnt_OpenTcp(srv->m, ent);
        }else if(((Abstract *)h->key)->type.of == TYPE_BUFF){
            Buff *bf = (Buff *)h->key;
            if(bf->pfd != NULL){
                memcpy(srv->endPointPfds->pfds+srv->endPointIt.idx,
                    bf->pfd, sizeof(struct pollfd));
            }
            bf->pfd = &srv->endPointPfds->pfds[srv->endPointIt.idx];
            /* setup request for file in Q */
        }
    }
    ReturnVoid(srv->m);
}

void Serve_Serve(Serve *srv){
    if((srv->type.state & PROCESSING) == 0){
        Serve_Setup(srv);
        srv->type.state |= PROCESSING;
    }

    while((srv->type.state & ERROR) == 0){
        ServeTcp_AcceptPoll(srv);
    }

    return;
}


void Serve_LogOpen(Serve *srv, Req *req){
    /*
    srv->def->log.open(srv->m, req, srv);
    */
}

void Serve_LogFinalized(Serve *srv, Req *req){
    /*
    srv->def->log.final(srv->m, req, srv);
    */
}

Serve *Serve_Make(MemCh *m, Table *routes /* <HostEnt, HandlerDef> */, void *source){
    Serve *srv = MemCh_AllocOf(m, sizeof(Serve), TYPE_SERVE);
    srv->type.of = TYPE_SERVE;
    srv->m = m;
    srv->q = Queue_Make(m, (QueueCritFunc)ReqCrit_Func);
    srv->source = source;
    if(routes->type.of == TYPE_TABLE){
        routes = Table_Ordered(m, routes);
    }
    Iter_Init(&srv->endPointIt, routes);
    srv->log.out = OutStream;

    srv->type.state |= DEBUG;

    return srv;
}
