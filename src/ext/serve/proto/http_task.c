#include <external.h>
#include <caneka.h>

status HttpTask_PrepareResponse(Step *st, Task *tsk){
    DebugStack_Push(st, st->type.of);
    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);
    HttpCtx *ctx = (HttpCtx *)as(proto->data, TYPE_HTTP_CTX);

    Buff *bf = Buff_Make(tsk->m, ZERO);
    HttpCtx_WriteHeaders(bf, ctx);
    Buff_Stat(bf);
    Task_AddDataStep(tsk, TcpTask_WriteStep, NULL, (Abstract *)bf, NULL, ZERO);
    proto->out = Buff_Make(tsk->m, BUFF_UNBUFFERED);
    proto->out->type.state |= DEBUG;

    tsk->type.state |= TcpTask_ExpectSend(NULL, tsk);
    st->type.state |= SUCCESS;

    Abstract *args[2] = {NULL, NULL};
    Out("^b.^{STACK.name}^0\n", args);

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

    Abstract *args[2] = {NULL, NULL};
    Out("^b.^{STACK.name}^0\n", args);

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

    Abstract *args[2] = {NULL, NULL};
    Out("^b.^{STACK.name}^0\n", args);

    DebugStack_Pop();
    return r;
}
