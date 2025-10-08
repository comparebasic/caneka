#include <external.h>
#include <caneka.h>

status HttpTask_PrepareResponse(Step *st, Task *tsk){
    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);
    Str *s = Str_CstrRef(tsk->m, "HTTP/1.1 200 OK\r\nServer: caneka\r\n"
        "Content-Length: 2\r\n"
        "\r\n"
        "ok");
    Cursor_Add(proto->out, s);
    st->type.state |= SUCCESS;
    return st->type.state;
}

status HttpTask_InitResponse(Task *tsk, Abstract *arg, Abstract *source){
    status r = READY;
    tsk->data = (Abstract *)HttpProto_Make(tsk->m);
    tsk->source = source;
    r |= Task_AddStep(tsk, TcpTask_WriteFromOut, NULL, NULL, STEP_IO_OUT);
    r |= Task_AddStep(tsk, HttpTask_PrepareResponse, NULL, NULL, ZERO);
    return r;
}

status HttpTask_AddRecieve(Task *tsk, Abstract *arg, Abstract *source){
    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);
    Roebling *rbl = HttpRbl_Make(tsk->m, proto->in, (Abstract *)proto);
    status r = Task_AddStep(tsk, TcpTask_ReadToRbl, (Abstract *)rbl, source, STEP_IO_IN);
    r |= TcpTask_ExpectRead(NULL, tsk);
    return r;
}
