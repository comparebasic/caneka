#include <external.h>
#include "<caneka.h>"

static void HttpStatic_Handle(MemCh *m, HttpReq *req, Serve *srv){
    return;
}

static void HttpStatic_Finalize(MemCh *m, HttpReq *req, Serve *srv){
    return;
}

static void HttpStatic_logOpen(MemCh *m, HttpReq *req, Serve *srv){
    return;
}

static void HttpStatic_logFinalized(MemCh *m, HttpReq *req, Serve *srv){
    return;
}

status HttpStatic_RetrieveFile(MemCh *m, HttpReq *req, Srv *srv){
    return SUCCESS;
}

static void setRoutes(MemCh *m, HandlerDef *def){
    return tbl;
}

void HttpStatic_DefMake(MemCh *m){
    HandlerDef *def = HandlerDef_Make(m);
    def->handle = HttpStatic_Handle;
    def->handle = HttpStatic_Finalize;
    def->log.open = HttpStatic_logOpen;
    def->log.open = HttpStatic_logFinalized;

    Table *tbl = Table_Make(m);

    Span *p = Span_Make(m);
    Span_Add(p, Func_Wrapped(m, HttpReq_RespToRbl));
    Span_Add(p, Func_Wrapped(m, HttpStatic_RetrieveFile));
    Span_Add(p, Func_Wrapped(m, HttpReq_Write));

    Hashed *h = Table_SetHashed(tbl, S(m, "static"), p);
    Iter_Init(&def->routesIt, tbl);
    def->routesIt.metrics.selected = h->idx;

    return;
}
