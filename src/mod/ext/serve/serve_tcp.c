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

    return fd;
}

static void ServeTcp_OpenTcp(Server *srv){
    i32 fd = openPortToFd(srv->ctx->address.ip.port);
    void *args[4];

    struct pollfd *pfd = (struct pollfd *)&srv->u;
    if(fd > 0){
        pfd->fd = fd;
        pfd->events = POLLIN;
        pfd->revents = 0;
    }else{
        srv->type.state |= ERROR;
    }

    args[0] = I32_Wrapped(OutStream->m, fd);
    args[1] = srv;
    args[0] = NULL;
    Out("^c.Opened Socket ^D.$^d.fd for @. Ready to Serve^0\n", args);
}

static void ServeTcp_AcceptPoll(Server *srv){
    MemCh *m = srv->m;
    Debug_Push(m, srv);

    srv->type.state &= ~SUCCESS;
    void *args[5];

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

    i32 accepted = 0;
    while(available-- > 0){
        i32 new_fd = accept(pfd->fd, (struct sockaddr*)NULL, NULL);
        if(new_fd > 0){
            srv->metrics.open++;

            fcntl(new_fd, F_SETFL, O_NONBLOCK);

            Req *req = srv->ctx->mk(srv->ctx);
            req->idx = Queue_Add(srv->q, req);

            struct pollfd *pfd = (struct pollfd *)&req->u;
            Queue_SetCriteria(srv->q, 0, req->idx, &req->u);
        }else{
            break;
        }
    }

    srv->type.state |= (NOOP|PROCESSING);
    while((Queue_Next(srv->q) & END) == 0){
        srv->type.state &= ~(NOOP|PROCESSING);
        Req *req = (Req *)Queue_Get(srv->q);

        srv->ctx->handle(m, req);
        if(req->type.state & (SUCCESS|ERROR)){
            Queue_Remove(srv->q, req->idx);
            srv->ctx->finalize(m, req);    
            MemCh_Free(req->m);
            if(req->type.state & ERROR){
                srv->metrics.error++;
            }
            srv->metrics.served++;
            srv->metrics.open--;
        }
    }

    if((srv->type.state & DEBUG) && srv->q->type.state & END){
        args[0] = srv;
        args[1] = NULL;
        Out("^c.    No more Reqs @^0\n", args);
    }

    ReturnVoid(m);
}

struct pollfd *Server_TcpGetPollFd(Req *req){
     return (struct pollfd *)&req->u;
}

void Server_ServeTcp(Server *srv){
    QueueCrit *crit = QueueCrit_Make(srv->m, QueueCrit_Fds, ZERO);
    Queue_AddHandler(srv->q, crit);

    ServeTcp_OpenTcp(srv);
    ServeTcp_AcceptPoll(srv);

    return;
}
