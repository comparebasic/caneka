#include <external.h>
#include <caneka.h>

status HttpE_Stats(MemCh *m, Handler *chain, Req *req){
    Handler *h = Handler_Get(chain);
    TcpCtx *ctx = (TcpCtx *)as(req->source, TYPE_TCP_CTX);
    HttpCtx *http = (HttpCtx *)as(req->data, TYPE_HTTP_CTX);
    Abstract *args[] = {
        (Abstract *)MicroTime_ToStr(m, MicroTime_Now()),
        (Abstract *)I64_Wrapped(m, ctx->metrics.served),
        (Abstract *)I64_Wrapped(m, ctx->metrics.error),
        NULL
    }
    http->body = Fmt_ToStrVec(m, "Time is $, served: $, error: $", args); 
    http->code = 200;
    h->type.state |= SUCCESS;
    return h->type.state;
}

status HttpE_Make(MemCh *m, Handler *chain, Req *req){
    Handler *h = chain;
    h = Handler_Add(m, h, Http_RespondMake, NULL, (Abstract *)req);
    h = Handler_Add(m, h, HttpE_Stats, NULL, (Abstract *)req);
    h = Handler_Add(m, h, Http_RecieveMake, NULL, (Abstract *)req);
    return SUCCESS;
}
