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
    MemCh *m = tsk->m;
    status r = READY;

    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);
    TcpCtx *tcp = (TcpCtx *)as(tsk->source, TYPE_TCP_CTX);
    HttpCtx *ctx = (HttpCtx *)as(proto->data, TYPE_HTTP_CTX);
    
    Route *handler = Object_ByPath(tcp->pages, ctx->path, NULL, SPAN_OP_GET);

    Object *data = Object_Make(m, ZERO);
    Object *nav = Object_Make(m, ZERO);
    Object_Set(nav, (Abstract *)Str_FromCstr(m, "pages", STRING_COPY), (Abstract *)tcp->pages);
    Object_Set(data, (Abstract *)Str_FromCstr(m, "nav", STRING_COPY), (Abstract *)nav);

    if(handler == NULL){
        ctx->code = 404;
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

        Buff *headerBf = NULL;
        if((funcW->type.state & ROUTE_ASSET) == 0){
            StrVec *path = StrVec_From(m, Str_FromCstr(m, "header", ZERO));
            Route *header = Object_ByPath(tcp->inc, path, NULL, SPAN_OP_GET);

            headerBf = Buff_Make(m, ZERO);
            r |= Route_Handle(header, headerBf, data, NULL);
            Buff_Stat(headerBf);
            ctx->contentLength += headerBf->st.st_size;
        }

        Buff *bf = NULL;
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

        HttpCtx_WriteHeaders(proto->out, ctx);
        if(headerBf != NULL){
            Buff_Pipe(proto->out, headerBf);
        }
        Buff_Pipe(proto->out, bf);
        if(footerBf != NULL){
            Buff_Pipe(proto->out, footerBf);
        }
    }

    st->type.state |= SUCCESS;
    DebugStack_Pop();
    return st->type.state;
}
