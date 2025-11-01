#include <external.h>
#include <caneka.h>

status HttpTask_PrepareResponse(Step *st, Task *tsk){
    DebugStack_Push(st, st->type.of);
    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);
    HttpCtx *ctx = (HttpCtx *)as(proto->data, TYPE_HTTP_CTX);

    if(ctx->code == 404 || ctx->content == NULL || ctx->content->total == 0){
        Str *s = Str_CstrRef(tsk->m, "HTTP/1.1 404 Not Found\r\n"
            "Server: caneka\r\n"
            "Content-Length: 9\r\n"
            "\r\n"
            "not found");
        Buff_Add(proto->out, s);
    }else{
        Buff *bf = Buff_From(tsk->m, proto->out->v);

        Str *statLine = Str_CstrRef(tsk->m, "200 OK");
        if(ctx->code != 200){
            statLine = Str_CstrRef(tsk->m, "500 Server Error");
        }

        Abstract *args[] = {
            (Abstract *)statLine,
            (Abstract *)ctx->mime,
            (Abstract *)I64_Wrapped(tsk->m, ctx->content->total),
            (Abstract *)ctx->content,
            NULL
        };

        Fmt(bf, "HTTP/1.1 $\r\n"
            "Server: Caneka\r\n"
            "Content-Type: $\r\n"
            "Content-Length: $\r\n"
            "\r\n"
            "$", args);
    }

    tsk->type.state |= TcpTask_ExpectSend(NULL, tsk);
    st->type.state |= SUCCESS;
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
    DebugStack_Pop();
    return r;
}
