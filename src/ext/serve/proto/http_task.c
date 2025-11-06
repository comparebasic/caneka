#include <external.h>
#include <caneka.h>

status HttpTask_PrepareResponse(Step *st, Task *tsk){
    DebugStack_Push(st, st->type.of);
    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);
    HttpCtx *ctx = (HttpCtx *)as(proto->data, TYPE_HTTP_CTX);

    HttpCtx_WriteHeaders(proto->out, ctx);

    tsk->type.state |= TcpTask_ExpectSend(NULL, tsk);
    st->type.state |= SUCCESS;

    Out("^b.PrepareResponse^0\n", NULL);

    DebugStack_Pop();
    return st->type.state;
}

status HttpTask_InitResponse(Task *tsk, Abstract *arg, Abstract *source){
    DebugStack_Push(tsk, tsk->type.of);
    status r = READY;
    if(tsk->data == NULL){
        tsk->data = (Abstract *)HttpProto_Make(tsk->m);
    }
    tsk->source = source;
    r |= Task_AddStep(tsk, TcpTask_WriteFromOut, NULL, NULL, STEP_IO_OUT);
    r |= Task_AddStep(tsk, HttpTask_PrepareResponse, NULL, NULL, ZERO);

    Out("^b.Init Response^0\n", NULL);

    DebugStack_Pop();
    return r;
}

status HttpTask_AddRecieve(Task *tsk, Abstract *arg, Abstract *source){
    DebugStack_Push(tsk, tsk->type.of);
    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);
    Cursor *curs = Cursor_Make(tsk->m, proto->in->v);
    Roebling *rbl = HttpRbl_Make(tsk->m, curs, (Abstract *)proto);
    status r = Task_AddStep(tsk, TcpTask_ReadToRbl, (Abstract *)rbl, source, STEP_IO_IN);
    tsk->type.state |= TcpTask_ExpectRecv(NULL, tsk);

    Out("^b.AddRecieve^0\n", NULL);

    DebugStack_Pop();
    return r;
}
