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

status ServeTcp_OpenTcp(Step *st, Task *tsk){
    TcpCtx *ctx = (TcpCtx *)as(st->arg, TYPE_TCP_CTX);
    i32 fd = openPortToFd(ctx->port);
    struct pollfd *pfd = TcpTask_GetPollFd(tsk);
    if(fd > 0){
        pfd->fd = fd;
        pfd->events = POLLIN;
        pfd->revents = 0;
        st->type.state |= SUCCESS;
    }else{
        st->type.state |= ERROR;
    }
    return st->type.state;
}

status ServeTcp_AcceptPoll(Step *st, Task *tsk){
    status r = READY;
    st->type.state &= ~SUCCESS;
    Abstract *args[5];
    TcpCtx *ctx = (TcpCtx *)as(st->arg, TYPE_TCP_CTX);
    Queue *q = (Queue *)as(tsk->data, TYPE_QUEUE);
    struct pollfd *pfd = TcpTask_GetPollFd(tsk);;
    i32 available = min(poll(pfd, 1, TCP_POLL_DELAY), ACCEPT_AT_ONEC_MAX);
    if(available == -1){
        args[0] = (Abstract *)Str_CstrRef(ErrStream->m, strerror(errno));
        args[1] = NULL;
        Error(ErrStream->m, (Abstract *)tsk, 
            FUNCNAME, FILENAME, LINENUMBER,
            "Error connecting to test socket: $\n", args);
        st->type.state |= ERROR;
        return st->type.state;
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

            child->idx = Queue_Add(q, (Abstract *)child);
            Queue_SetCriteria(q, 0, child->idx, &child->u);

            accepted++;
            r |= tsk->type.state;
        }else{
            break;
        }
    }

    while((Queue_Next(q) & END) == 0){
        Task *child = (Task *)Queue_Get(q);
        Task_Tumble(child);
    }

    if(q->type.state & END){
        st->type.state |= SUCCESS;
    }

    return st->type.state;
}

status ServeTcp_SetupQueue(Step *st, Task *tsk){
    tsk->data = Queue_Make(m); 
    st->type.state |= SUCCESS;
    return st->type.state;
}

Task *ServeTcp_Make(TcpCtx *ctx){
    Task *tsk = Task_Make(NULL, (Abstract *)ctx);
    Task_AddStep(tsk, ServeTcp_AcceptPoll, (Abstract *)ctx, NULL);
    Task_AddStep(tsk, ServeTcp_OpenTcp, (Abstract *)ctx, NULL);
    Task_AddStep(tsk, ServeTcp_SetupQueue, (Abstract *)ctx, NULL);
    return tsk;
}
