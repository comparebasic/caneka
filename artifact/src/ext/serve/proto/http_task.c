#include <external.h>
#include <caneka.h>

status HttpTask_PrepareResponse(Step *st, Task *tsk){
    ProtoCtx *proto = asIfc(tsk->data, TYPE_PROTO);
    Str *s = Str_CstrRef(tsk->m, "HTTP/1.1 200 OK\r\nServer: caneka\r\n"
        "Content-Length: 2\r\n"
        "\r\n"
        "ok");
    Cursor_Add(proto->out, s);
    st->type.state |= SUCCESS;
    return st->type.state;
}

status HttpTask_InitResponse(Task *tsk, Abstract *arg, Abstract *source){
    tsk->data = HttpCtx_Make(tsk->m);
    tsk->source = source;
    Task_AddStep(tsk, TcpTask_WriteFromOut, NULL, NULL);
    Task_AddStep(tsk, HttpTask_PrepareResponse, NULL, NULL);
    return tsk;
}
