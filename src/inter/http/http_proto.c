#include <external.h>
#include <caneka.h>

status HttpProto_AddBuff(ProtoCtx *proto, Buff *bf){
    HttpCtx *ctx = (HttpCtx *)proto->ctx;
    Buff_Stat(bf);
    ctx->contentLength += bf->st.st_size;
    Span_Add(proto->outSpan, bf);
    return bf->type.state;
}

status HttpProto_PrepareResponse(ProtoCtx *proto, Task *tsk){
    DebugStack_Push(NULL, ZERO);

    Buff *bf = Buff_Make(tsk->m, ZERO);
    HttpCtx_WriteHeaders(bf, (HttpCtx *)proto->ctx);
    Buff_Stat(bf);
    Span_Add(proto->outSpan, bf);

    TcpTask_ExpectSend(NULL, tsk);

    DebugStack_Pop();
    return ZERO;
}

ProtoCtx *HttpProto_Make(MemCh *m){
    ProtoCtx *ctx = ProtoCtx_Make(m);
    ctx->in = Buff_Make(m, ZERO);
    ctx->out = Buff_Make(m, BUFF_UNBUFFERED);
    ctx->ctx = HttpCtx_Make(m); 
    return ctx;
}
