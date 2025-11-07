#include <external.h>
#include <caneka.h>
#include <web-server.h>

status Example_ServeError(Step *st, Task *tsk){
    DebugStack_Push(st, st->type.of);
    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);
    HttpCtx *ctx = (HttpCtx *)as(proto->data, TYPE_HTTP_CTX);

    Out("^b.ServeError^0\n", NULL);
    st->type.state |= SUCCESS;

    DebugStack_Pop();
    return st->type.state;
}

status Example_ServePage(Step *st, Task *tsk){
    DebugStack_Push(st, st->type.of);
    Abstract *args[5];
    MemCh *m = tsk->m;
    status r = READY;

    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);
    TcpCtx *tcp = (TcpCtx *)as(tsk->source, TYPE_TCP_CTX);
    HttpCtx *ctx = (HttpCtx *)as(proto->data, TYPE_HTTP_CTX);

    DebugStack_SetRef(ctx->path, ctx->path->type.of);
    Route *handler = Object_ByPath(tcp->pages, ctx->path, NULL, SPAN_OP_GET);

    Object *data = Object_Make(m, ZERO);
    Object *nav = Object_Make(m, ZERO);
    Object_Set(nav, (Abstract *)Str_FromCstr(m, "pages", STRING_COPY), (Abstract *)tcp->pages);
    Object_Set(data, (Abstract *)Str_FromCstr(m, "nav", STRING_COPY), (Abstract *)nav);

    if(handler == NULL){
        Str *s = Str_FromCstr(m, "not found", STRING_COPY);

        ctx->contentLength = s->length;
        ctx->code = 404;
        ctx->mime = Str_FromCstr(m, "text/plain", STRING_COPY);

        Buff *bf = Buff_Make(m, ZERO);
        Buff_AddBytes(bf, s->bytes, s->length);
        Task_AddDataStep(tsk, TcpTask_WriteStep, NULL, (Abstract *)bf, NULL, ZERO);
        HttpCtx_PrepareResponse(ctx, tsk);

        st->type.state |= MORE;

    }else{

        Object *config = (Object *)Object_GetPropByIdx(handler, ROUTE_PROPIDX_DATA);
        if(config != NULL){
            Object_Set(data,
                (Abstract *)Str_FromCstr(m, "config", STRING_COPY), (Abstract *)config);
        }

        ctx->mime = (Str *)Object_GetPropByIdx(handler, ROUTE_PROPIDX_MIME);
        Single *funcW = (Single *)as(
            Object_GetPropByIdx(handler, ROUTE_PROPIDX_FUNC),
            TYPE_WRAPPED_FUNC
        );

        Buff *headBf = NULL;
        if((funcW->type.state & ROUTE_ASSET) == 0){
            StrVec *path = StrVec_From(m, Str_FromCstr(m, "header", ZERO));
            Route *header = Object_ByPath(tcp->inc, path, NULL, SPAN_OP_GET);
            headBf = Buff_Make(m, ZERO);
            r |= Route_Handle(header, headBf, data, NULL);
            Buff_Stat(headBf);
            ctx->contentLength += headBf->st.st_size;
        }

        Buff *bf = Buff_Make(m, ZERO);
        r |= Route_Handle(handler, bf, data, NULL);
        Buff_Stat(bf);
        ctx->contentLength += bf->st.st_size;

        Buff *footerBf = NULL;
        if((funcW->type.state & ROUTE_ASSET) == 0){
            StrVec *path = StrVec_From(m, Str_FromCstr(m, "footer", ZERO));
            Route *header = Object_ByPath(tcp->inc, path, NULL, SPAN_OP_GET);

            footerBf = Buff_Make(m, ZERO);
            r |= Route_Handle(header, footerBf, data, NULL);
            Buff_Stat(footerBf);
            ctx->contentLength += footerBf->st.st_size;

        }

        ctx->code = 200;

        if(footerBf != NULL){
            Task_AddDataStep(tsk, TcpTask_WriteStep, NULL, (Abstract *)footerBf, NULL, ZERO);
        }
        Task_AddDataStep(tsk, TcpTask_WriteStep, NULL, (Abstract *)bf, NULL, ZERO);
        if(headBf != NULL){
            Task_AddDataStep(tsk, TcpTask_WriteStep, NULL, (Abstract *)headBf, NULL, ZERO);
        }
        HttpCtx_PrepareResponse(ctx, tsk);
        st->type.state |= MORE;
    }

    st->type.state |= SUCCESS;
    DebugStack_Pop();
    return st->type.state;
}
