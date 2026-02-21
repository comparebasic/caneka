#include <external.h>
#include <caneka.h>

static StrVec *getPollFlagVec(MemCh *m, struct pollfd *pfd){
    StrVec *v = StrVec_Make(m);
    if(pfd->events & POLLIN){
        StrVec_Add(v, S(m, "IN"));
    }
    if(pfd->events & POLLOUT){
        StrVec_Add(v, S(m, "OUT"));
    }
    if(poll(pfd, 1, 0)){
        StrVec_Add(v, S(m, "ACTIVE"));
    }
    return v;
}

static i64 ProtoCtx_Print(Buff *bf, void *a, cls type, word flags){
    ProtoCtx *ctx = (ProtoCtx*)as(a, TYPE_PROTO_CTX);
    void *args[] = {
        Type_StateVec(bf->m, ctx->type.of, ctx->type.state),
        Util_Wrapped(bf->m, ctx->u),
        ctx->in,
        ctx->out,
        ctx->ctx,
        NULL,
    };
    return Fmt(bf, "Proto<$ u:$ in:@ out:@ data:@>", args);
}

static status TcpTask_Print(Buff *bf, void *a, cls type, word flags){
    Task *tsk = (Task *)as(a, TYPE_TCP_TASK);
    void *args[8];
    struct pollfd *pfd = TcpTask_GetPollFd(tsk);
    args[0] = Type_StateVec(bf->m, TYPE_TASK, tsk->type.state);
    args[1] = I32_Wrapped(bf->m, tsk->idx);
    args[2] = I32_Wrapped(bf->m, pfd->fd);
    args[3] = getPollFlagVec(bf->m, pfd);
    args[4] = NULL;

    Fmt(bf, "TcpTask<@ ^D.@^d.idx ^D$^d.fd @", args);
    if(flags & DEBUG){
        args[0] = I32_Wrapped(bf->m, tsk->chainIt.idx);
        args[1] = I32_Wrapped(bf->m, tsk->chainIt.p->max_idx);
        args[2] = Iter_Get(&tsk->chainIt);
        args[3] = NULL;
        Fmt(bf, "$of$ \\@@>", args);
    }else{
        Buff_AddBytes(bf, (byte *)">", 1);
    }
    return SUCCESS;
}

static i64 TcpCtx_Print(Buff *bf, void *a, cls type, word flags){
    TcpCtx *ctx = (TcpCtx*)as(a, TYPE_TCP_CTX);
    void *args[] = {
        Type_StateVec(bf->m, ctx->type.of, ctx->type.state),
        ctx->path,
        I32_Wrapped(bf->m, ctx->port),
        ctx->inet4,
        ctx->inet6,
        Time_Wrapped(bf->m, &ctx->metrics.start),
        I64_Wrapped(bf->m, ctx->metrics.open),
        I64_Wrapped(bf->m, ctx->metrics.error),
        I64_Wrapped(bf->m, ctx->metrics.served),
        NULL,
    };
    return Fmt(bf, "Tcp<$ $ ^D.$^d.port ^D.$^d.inet4 ^D.$^d.inet6 $start $open $error $served>", args);
}

status Serve_ToSInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_TCP_CTX, (void *)TcpCtx_Print);
    r |= Lookup_Add(m, lk, TYPE_TCP_TASK, (void *)TcpTask_Print);
    r |= Lookup_Add(m, lk, TYPE_PROTO_CTX, (void *)ProtoCtx_Print);
    return r;
}

status Serve_TosInit(MemCh *m){
    return Serve_ToSInit(m, ToStreamLookup);
}
