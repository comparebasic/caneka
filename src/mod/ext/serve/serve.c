#include <external.h>
#include <caneka.h>

static Table *services = NULL;
Table *ServeProtoTable = NULL;

static i16 _g = 0;
static void ServeTcp_AcceptPoll(Serve *srv){
    MemCh *m = srv->m;
    Debug_Push(m, srv);

    srv->type.state &= ~SUCCESS;
    void *args[8];

    Time_Now(&srv->q->time.present);

    i64 timeout = 0;
    if(srv->q->it.p->nvalues <= srv->metrics.fixed){
        timeout = TCP_ZERO_REQ_DELAY;
    }

    i32 available = PfdSpan_Poll(srv->pfds, timeout);
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
            struct sockaddr_in cliaddr4;
            struct sockaddr_in6 cliaddr6;
            socklen_t len = 0;
            if(ent->type.of == TYPE_HOST_ENT && ent->addr != NULL){
                void *cliaddr = NULL;
                if(ent->addr->type.of == TYPE_NET_ADDR6){
                    cliaddr = &cliaddr6; 
                    len = sizeof(cliaddr6);
                }else if(ent->addr->type.of == TYPE_NET_ADDR4){
                    cliaddr = &cliaddr4; 
                    len = sizeof(cliaddr4);
                }

                sizeof(cliaddr);
                i32 max = 16;
                while(max-- > 0){
                    i32 new_fd = accept(ent->pfd->fd, (struct sockaddr*)cliaddr, &len);
                    if(new_fd > 0){
                        fcntl(new_fd, F_SETFL, O_NONBLOCK);

                        MemCh *rm = MemCh_Make();
                        NetAddr *addr = NetAddr_Make4(m);

                        if(ent->addr->type.of == TYPE_NET_ADDR4){
                            struct sockaddr_in *cli = (struct sockaddr_in *)cliaddr;
                            cli->sin_port = ntohs(cli->sin_port); 
                            memcpy(&addr->net.ip4addr, cliaddr, len);
                        }else if(ent->addr->type.of == TYPE_NET_ADDR6){
                            struct sockaddr_in6 *cli = (struct sockaddr_in6 *)cliaddr;
                            cli->sin6_port = ntohs(cli->sin6_port); 
                            memcpy(&addr->net.ip6addr, cliaddr, len);
                        }

                        Req *req = Req_Make(rm,
                            def, addr, new_fd, def->extra(rm, (Abstract *)ent, def));
                        req->idx = Queue_Add(srv->q, req, req->crit);
                        rm->owner = req;
                        Time_Now(&req->metrics.start);

                        srv->metrics.open++;

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
            Out("^y.   Handling qIdx:$^0", ar);
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

i32 Serve_PortByService(Str *s){
    Single *sg = Table_Get(services, s);
    if(s == NULL){
        return -1;
    }else{
        return sg->val.i;
    }
}

void Serve_AddEndpoint(Serve *srv, Abstract *key, HandlerDef *def){
    Debug_Push(srv->m, srv);

    if(key->type.of == TYPE_HOST_ENT){
        HostEnt *ent = (HostEnt *)key;
        ent->pfd = PfdSpan_GetNextPfd(srv->pfds);
        ent->type.state |= DEBUG;
        HostEnt_OpenTcp(srv->m, ent);
    }else if(key->type.of == TYPE_BUFF){
        Buff *bf = (Buff *)key;

        struct pollfd *pfd = PfdSpan_GetNextPfd(srv->pfds);
        if(bf->pfd != NULL){
            memcpy(pfd, bf->pfd, sizeof(struct pollfd));
        }
        pfd->events = RECV_FLAGS;
        bf->pfd = pfd;

        MemCh *rm = MemCh_Make();
        Req *req = Req_MakeFile(rm, def, bf, def->extra(rm, (Abstract *)bf, def));
        req->crit->type.of = TYPE_FILE_CRIT;
        req->idx = Queue_Add(srv->q, req, req->crit);
        rm->owner = req;
        Time_Now(&req->metrics.start);
        srv->metrics.fixed++;
    }

    Hashed *h = Hashed_Make(srv->m, key);
    h->value = def;
    Iter_Add(&srv->endPointIt, h);

    ReturnVoid(srv->m);
}

void Serve_Serve(Serve *srv){
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

Serve *Serve_Make(MemCh *m){
    Serve *srv = MemCh_AllocOf(m, sizeof(Serve), TYPE_SERVE);
    srv->type.of = TYPE_SERVE;
    srv->m = m;
    srv->q = Queue_Make(m, (QueueCritFunc)ReqCrit_Func);
    Iter_Init(&srv->endPointIt, Span_Make(m));
    srv->log.out = OutStream;

    srv->etags = Table_Make(m);
    srv->pfds = Span_Make(m);

    return srv;
}

void Serve_Init(MemCh *m){
    if(services == NULL){
        services = Table_Make(m);
        Table_Set(services, S(m, "http"), I32_Wrapped(m, 80));
        Table_Set(services, S(m, "https"), I32_Wrapped(m, 443));
        Lookup_Add(m, HashLookup, TYPE_HOST_ENT, (void *)HostEnt_Hash);
        Lookup_Add(m, EqualsLookup, TYPE_HOST_ENT, (void *)HostEnt_Equals);
        ServeProtoTable = Table_Make(m);
        Table_Set(ServeProtoTable, S(m, "https"), Func_Wrapped(m, HttpTlsReq_DefMake, ZERO));
        Table_Set(ServeProtoTable, S(m, "http"), Func_Wrapped(m, HttpReq_DefMake, ZERO));
        Table_Set(ServeProtoTable, S(m, "cmd-file"), Func_Wrapped(m, CmdFile_DefMake, ZERO));

        Lookup_Add(m, ErrorHandlers, TYPE_REQ, (void *)Req_Error);
    }
}
