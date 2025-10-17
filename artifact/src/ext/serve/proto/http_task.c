#include <external.h>
#include <caneka.h>

status HttpTask_PrepareResponse(Step *st, Task *tsk){
    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);
    HttpCtx *ctx = (HttpCtx *)as(proto->data, TYPE_HTTP_CTX);
    if(ctx->code == 404 || ctx->content == NULL || ctx->content->total == 0){
        Str *s = Str_CstrRef(tsk->m, "HTTP/1.1 404 Not Found\r\n"
            "Server: caneka\r\n"
            "Content-Length: 9\r\n"
            "\r\n"
            "not found");
        Cursor_Add(proto->out, s);
    }else{
        Str *s = Str_CstrRef(tsk->m, 
            "HTTP/1.1 200 OK\r\n"
            "Server: Caneka\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: ");
        Cursor_Add(proto->out, s);
        s = Str_FromI64(tsk->m, ctx->content->total);
        Cursor_Add(proto->out, s);
        Cursor_Add(proto->out, Str_CstrRef(tsk->m, "\r\n\r\n"));

        Cursor_AddVec(proto->out, ctx->content);
    }
    Cursor_Setup(proto->out, StrVec_ReAlign(tsk->m, proto->out->v));
    tsk->type.state |= TcpTask_ExpectSend(NULL, tsk);
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
    tsk->type.state |= TcpTask_ExpectRecv(NULL, tsk);
    return r;
}
