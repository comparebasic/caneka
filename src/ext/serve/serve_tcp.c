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

	if(listen(fd, 10) != 0){
        Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER,
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
    DebugStack_Push(st, st->type.of);
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
        Error(tsk->m, FUNCNAME, FILENAME, LINENUMBER,
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

            MemCh *tm = MemCh_Make();
            Task *child = Task_Make(Span_Make(tm), (Abstract *)tsk);
            child->type.state |= TASK_CHILD;

            child->type.state |= DEBUG;

            child->parent = tsk;
            child->stepGuardMax = TCP_STEP_MAX;
            tm->owner = (Abstract *)child;
            ctx->populate(tm, child, (Abstract *)I32_Wrapped(tm, new_fd), (Abstract *)tsk->source);

            if(tsk->type.state & DEBUG){
                struct pollfd *pfd = TcpTask_GetPollFd(child);
                args[0] = (Abstract *)child;
                args[1] = (Abstract *) I32_Wrapped(child->m, pfd->fd);
                args[2] = NULL;
                Out("^c.    Adding Child & fd$^0\n", args);
            }

            child->idx = Queue_Add(q, (Abstract *)child);
            q->type.state |= DEBUG;
            Queue_SetCriteria(q, 0, child->idx, &child->u);

            accepted++;
            r |= tsk->type.state;
        }else{
            break;
        }
    }

    while((Queue_Next(q) & END) == 0){
        if(tsk->type.state & DEBUG){
            args[0] = (Abstract *)q;
            args[1] = NULL;
            Out("^c.    Getting next @^0\n", args);
        }
        Task *child = (Task *)Queue_Get(q);
        if(tsk->type.state & DEBUG){
            args[0] = (Abstract *)child;
            args[1] = NULL;
            Out("^c.    Tumbling @^0\n", args);
        }
        child->parent = tsk;
        Task_Tumble(child);
        if(child->type.state & (SUCCESS|ERROR)){
            Queue_Remove(q, child->idx);
            ctx->finalize(NULL, child);    
            MemCh_Free(child->m);
        }
    }

    if((tsk->type.state & DEBUG) && q->type.state & END){
        args[0] = (Abstract *)tsk;
        args[1] = NULL;
        Out("^c.    No more Reqs @^0\n", args);
    }

    st->type.state |= NOOP;

    DebugStack_Pop();
    return st->type.state;
}

static status ServeTcp_SetupQueue(Step *st, Task *tsk){
    Queue *q = Queue_Make(tsk->m);

    QueueCrit *crit = QueueCrit_Make(tsk->m, QueueCrit_Fds, ZERO);
    Queue_AddHandler(q, crit);

    tsk->data = (Abstract *)q;
    tsk->type.state |= TASK_QUEUE;

    st->type.state |= SUCCESS;
    return st->type.state;
}

Task *ServeTcp_Make(TcpCtx *ctx){
    Task *tsk = Task_Make(NULL, (Abstract *)ctx);
    tsk->stepGuardMax = -1;

    tsk->type.state |= DEBUG;

    Task_AddStep(tsk,
        Step_Delay, (Abstract *)Util_Wrapped(tsk->m, TIME_SEC), NULL, STEP_LOOP);
    Task_AddStep(tsk, ServeTcp_AcceptPoll, (Abstract *)ctx, NULL, ZERO);
    Task_AddStep(tsk, ServeTcp_OpenTcp, (Abstract *)ctx, NULL, ZERO);
    Task_AddStep(tsk, ServeTcp_SetupQueue, (Abstract *)ctx, NULL, ZERO);
    return tsk;
}
