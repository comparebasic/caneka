#include <external.h>
#include <caneka.h>

static void HttpStatic_Setup(MemCh *m, Req *req, Serve *srv){
    HttpReq_Setup(m, req, srv);
    HttpReq_SetToRecv((HttpReq *)req);
}

static void HttpStatic_Handle(MemCh *m, HttpReq *req, Serve *srv){
    Str *key = K(m, "static");
    Hashed *h = Table_GetHashedByIter(&srv->def->routeIt, key);
    Span *chain = Ifc(m, h->value, TYPE_SPAN);
    if((req->routeIt.type.state & PROCESSING) == 0){
        Iter_Init(&req->routeIt, chain);
        Iter_Next(&req->routeIt);
    }else if(req->type.state & SUCCESS){
        Iter_Next(&req->routeIt);
    }

    Single *sg = Iter_Get(&req->routeIt);
    ReqFunc func = (ReqFunc)sg->val.ptr;
    func(m, (Req *)req, srv);

    return;
}

static void HttpStatic_Finalize(MemCh *m, HttpReq *req, Serve *srv){
    void *ar[] = {req, NULL};
    Out("^y.Finalize @^0\n", ar);
    return;
}

static void HttpStatic_logOpen(MemCh *m, HttpReq *req, Serve *srv){
    return;
}

static void HttpStatic_logFinalized(MemCh *m, HttpReq *req, Serve *srv){
    return;
}

status HttpStatic_RetrieveFile(MemCh *m, HttpReq *req, Serve *srv){
    Debug_Push(m, req);
    StrVec *local = IoPath_FromVec(m, req->path);
    IoUtil_Relativise(m, local);

    Str *dir = Seel_Get(srv->config, K(m, "dir"));
    StrVec *path = Clone(m, dir);
    StrVec_AddVec(path, local);

    Buff *bf = Buff_Make(m, BUFF_UNBUFFERED);
    File_Open(bf, path, O_RDONLY);
    Span_Add(req->sections, bf);

    void *ar[] = {
        path, 
        bf,
        NULL
    };
    Out("^y.File Retrieved $ @^0\n", ar);
    req->type.state |= SUCCESS;

    Return(m, req->type.state);
}

HandlerDef *HttpStatic_DefMake(MemCh *m){
    HandlerDef *def = HandlerDef_Make(m);
    def->mk = HttpReq_Mk;
    def->setup = HttpStatic_Setup;
    def->handle = (ReqFunc) HttpStatic_Handle;
    def->finalize = (ReqFunc) HttpStatic_Finalize;
    def->log.open = (ReqFunc) HttpStatic_logOpen;
    def->log.final = (ReqFunc) HttpStatic_logFinalized;

    Table *tbl = Table_Make(m);

    Span *p = Span_Make(m);
    Span_Add(p, Func_Wrapped(m, HttpReq_RespToRbl));
    Span_Add(p, Func_Wrapped(m, HttpStatic_RetrieveFile));
    Span_Add(p, Func_Wrapped(m, HttpReq_Write));

    Hashed *h = Table_SetHashed(tbl, S(m, "static"), p);
    Iter_Init(&def->routeIt, tbl);
    def->routeIt.metrics.selected = h->idx;

    return def;
}
