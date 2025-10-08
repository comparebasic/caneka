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
        Error(ErrStream->m, NULL,
            FUNCNAME, FILENAME, LINENUMBER,
            "openPortToFd setting nonblock", NULL);
		return -1;
    }

    i32 one = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(i32)) < 0) {
        Error(ErrStream->m, NULL,
            FUNCNAME, FILENAME, LINENUMBER,
            "openPortToFd setting reuse addr", NULL);
		return -1;
	}
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &one, sizeof(i32)) < 0) {
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

static status ServeTcp_OpenTcp(Step *st, Task *tsk){
    TcpCtx *ctx = (TcpCtx *)as(st->arg, TYPE_TCP_CTX);
    i32 fd = openPortToFd(ctx->port);
    Abstract *args[4];

    struct pollfd *pfd = TcpTask_GetPollFd(tsk);
    if(fd > 0){
        pfd->fd = fd;
        pfd->events = POLLIN;
        pfd->revents = 0;
        st->type.state |= SUCCESS;
    }else{
        st->type.state |= ERROR;
    }

    if(tsk->type.state & DEBUG){
        args[0] = (Abstract *)I32_Wrapped(OutStream->m, fd);
        args[1] = (Abstract *)st;
        args[2] = (Abstract *)tsk;
        args[3] = NULL;
        Out("^c.Opened Socket ^D.$^d.fd for @ of @^0\n", args);
    }

    return st->type.state;
}

static status ServeTcp_AcceptPoll(Step *st, Task *tsk){
    status r = READY;
    st->type.state &= ~SUCCESS;
    Abstract *args[5];
    TcpCtx *ctx = (TcpCtx *)as(st->arg, TYPE_TCP_CTX);
    Queue *q = (Queue *)as(tsk->data, TYPE_QUEUE);
    struct pollfd *pfd = TcpTask_GetPollFd(tsk);;
    i32 available = poll(pfd, 1, TCP_POLL_DELAY);
    if(available == -1){
        args[0] = (Abstract *)Str_CstrRef(ErrStream->m, strerror(errno));
        args[1] = NULL;
        Error(ErrStream->m, (Abstract *)tsk, 
            FUNCNAME, FILENAME, LINENUMBER,
            "Error connecting to test socket: $\n", args);
        st->type.state |= ERROR;
        return st->type.state;
    }

    if(tsk->type.state & DEBUG){
        args[0] = (Abstract *)I32_Wrapped(OutStream->m, available);
        args[1] = (Abstract *)I32_Wrapped(OutStream->m, pfd->fd);
        args[2] = (Abstract *)st;
        args[3] = (Abstract *)tsk;
        args[4] = NULL;
        Out("^c.Accept Found ^D$^d.available on ^D.$^d.fd. @ of @^0\n", args);
    }

    i32 accepted = 0;
    while(available-- > 0){
        i32 new_fd = accept(pfd->fd, (struct sockaddr*)NULL, NULL);
        if(new_fd > 0){
            ctx->metrics.open++;

            fcntl(new_fd, F_SETFL, O_NONBLOCK);

            Task *child = ctx->func(NULL, tsk, NULL, NULL);
            struct pollfd *pfd = TcpTask_GetPollFd(child);
            pfd->fd = new_fd;

            child->idx = Queue_Add(q, (Abstract *)child, &child->u);

            accepted++;
            r |= tsk->type.state;
        }else{
            break;
        }
    }

    while((Queue_Next(q) & END) == 0){
        Task *child = (Task *)Queue_Get(q);
        if(tsk->type.state & DEBUG){
            args[0] = (Abstract *)child;
            args[1] = NULL;
            Out("^c.    Tumbling @^0\n", args);
        }
        Task_Tumble(child);
    }

    if(q->type.state & END){
        args[0] = (Abstract *)tsk;
        args[1] = NULL;
        Out("^c.    No more Reqs @^0\n", args);
    }

    st->type.state |= NOOP;

    return st->type.state;
}

static status ServeTcp_SetupQueue(Step *st, Task *tsk){
    tsk->data = (Abstract *)Queue_Make(tsk->m); 
    st->type.state |= SUCCESS;
    return st->type.state;
}

Task *ServeTcp_Make(TcpCtx *ctx){
    Task *tsk = Task_Make(NULL, (Abstract *)ctx);
    Task_AddStep(tsk, Step_Delay, (Abstract *)Util_Wrapped(tsk->m, 1000), NULL, STEP_LOOP);
    Task_AddStep(tsk, ServeTcp_AcceptPoll, (Abstract *)ctx, NULL, ZERO);
    Task_AddStep(tsk, ServeTcp_OpenTcp, (Abstract *)ctx, NULL, ZERO);
    Task_AddStep(tsk, ServeTcp_SetupQueue, (Abstract *)ctx, NULL, ZERO);
    return tsk;
}
