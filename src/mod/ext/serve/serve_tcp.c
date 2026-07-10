#include <external.h>
#include <caneka.h>


static i32 openPortToFd(i32 port){
    i32 fd = 0;
	struct sockaddr_in serv_addr;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);

    if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
        Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER,
            "openPortToFd setting nonblock", NULL);
		return -1;
    }

    i32 one = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(i32)) < 0) {
        Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER,
            "openPortToFd setting reuse addr", NULL);
		return -1;
	}
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &one, sizeof(i32)) < 0) {
        Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER,
            "openPortToFd setting reuse addr", NULL);
		return -1;
	}

	if(bind(fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) != 0){
        Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER,
            "openPortToFd binding", NULL);
		return -1;
    }

	if(listen(fd, TCP_LISTEN_BACKLOG) != 0){
        Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER,
            "openPortToFd listening", NULL);
		return -1;
    };

    printf("opened socket fd %d\n", fd);
    fflush(stdout);

    return fd;
}

static void ServeTcp_OpenTcp(Serve *srv){
    i32 fd = openPortToFd(srv->address.ent->port);
    void *args[4];

    struct pollfd *pfd = (struct pollfd *)&srv->u;
    if(fd > 0){
        pfd->fd = fd;
        pfd->events = POLLIN;
        pfd->revents = 0;
    }else{
        srv->type.state |= ERROR;
    }

    Buff_SetTemp(OutStream);
    args[0] = I32_Wrapped(OutStream->m, fd);
    args[1] = srv;
    args[2] = I32_Wrapped(OutStream->m, srv->address.ent->port);
    args[3] = NULL;
    Out("^c.Opened Socket ^D.$^d.fd for @. Ready to Serve on port $^0\n", args);
}

static i16 _g = 0;
static void ServeTcp_AcceptPoll(Serve *srv){
    MemCh *m = srv->m;
    Debug_Push(m, srv);
    Guard_Incr(m, &_g, 200, FUNCNAME, FILENAME, LINENUMBER);

    srv->type.state &= ~SUCCESS;
    void *args[8];

    struct pollfd *pfd = (struct pollfd *)&srv->u;

    i64 timeout = 0;
    if(srv->q->it.p->nvalues == 0){
        timeout = TCP_ZERO_REQ_DELAY;
    }

    i32 available = poll(pfd, 1, timeout);
    if(available == -1){
        args[0] = Str_CstrRef(ErrStream->m, strerror(errno));
        args[1] = NULL;
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error connecting to test socket: $\n", args);
        srv->type.state |= ERROR;

        ReturnVoid(m);
    }

    if(srv->type.state & DEBUG){
        Buff_SetTemp(OutStream); 
        MemBookStats st;
        MemBook_GetStats(OutStream->m, &st);
        args[0] = srv;
        args[1] = Str_MemCount(OutStream->m, st.total * PAGE_SIZE);
        args[2] = I32_Wrapped(OutStream->m, srv->q->it.p->nvalues);
        args[3] = I32_Wrapped(OutStream->m, available);
        args[4] = NULL;
        Out("^D.Serve.AcceptPoll @ Mem Used @ $requests $available^0\n", args);
    }

    if(available){
        i32 max = 16;
        while(max-- > 0){
            i32 new_fd = accept(pfd->fd, (struct sockaddr*)NULL, NULL);
            if(new_fd > 0){
                fcntl(new_fd, F_SETFL, O_NONBLOCK);

                MemCh *rm = MemCh_Make();
                TcpSource *ts = (TcpSource *)srv->source;
                ts->new_fd = new_fd;
                ts->clientEnt = NULL;

                Req *req = rm->owner = srv->def->mk(rm, srv);
                Time_Now(&req->metrics.start);
                srv->def->setup(req->m, req, srv);
                req->idx = Queue_Add(srv->q, req, req->crit);

                if(1 || srv->type.state & DEBUG){
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

    srv->type.state |= (NOOP|PROCESSING);
    while((Queue_Next(srv->q) & END) == 0){
        if(srv->type.state & DEBUG){
            Buff_SetTemp(OutStream); 
            void *ar[] = {
               I32_Wrapped(OutStream->m, srv->q->it.idx), 
               NULL
            };
            Out("^y.   Handling qIdx:$ ", ar);
            Bits_Print(OutStream, (byte *)&srv->q->go, sizeof(util), ZERO);
            Out("^0\n", NULL);
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
     return (struct pollfd *)req->slot;
}

void Serve_ServeTcp(Serve *srv){
    ServeTcp_OpenTcp(srv);

    while((srv->type.state & ERROR) == 0){
        ServeTcp_AcceptPoll(srv);
    }

    return;
}

Serve *Serve_MakeTcp(MemCh *m, HandlerDef *def, HostEnt *ent){
    Serve *srv = Serve_Make(m);
    srv->source = TcpSource_Make(m);
    srv->def = def;
    srv->address.ent = ent;

    srv->type.state |= DEBUG;
    return srv;
}
