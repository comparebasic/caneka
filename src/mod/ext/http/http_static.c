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

    if(IoUtil_IsSep(Span_Get(local->p, local->p->max_idx))){
        StrVec_Add(local, S(m, "index.html"));
    }

    void *ar[] = {
        local,
        req->path,
        NULL
    };
    Out("^y.Local @ from @^0\n", ar);

    Str *dir = Seel_Get(srv->config, K(m, "dir"));
    StrVec *path = Clone(m, dir);
    StrVec_AddVec(path, local);

    StrVec *ext = IoUtil_GetExt(m, local); 
    Str *mime = Table_Get(MimeByExt, ext);
    if(mime){
        HttpReq_SetHeader(req, S(m, "Content-Type"), mime);
    }

    Buff *bf = Buff_Make(m, BUFF_UNBUFFERED);
    File_Open(bf, path, O_RDONLY);
    Buff_Stat(bf);

    Str *timeStr = Time_ToRStr(m, &bf->st.st_mtim);

    StrVec *lastEtag = Table_GetByIter(&req->headersIt, K(m, "If-None-Match"));
    StrVec *since = Table_GetByIter(&req->headersIt, K(m, "If-Modified-Since"));

    Table *tags = Seel_Get(srv->config, K(m, "etags"));
    Inst *etag = (Inst *)Table_Get(tags, local);
    Str *etagStr = NULL;
    Str *etagLatest = NULL;
    Str *tag = NULL;

    if(etag != NULL){
        etagLatest =  Seel_Get(etag, K(m, "last-modified"));
    }

    if(etag == NULL || !Equals(timeStr, etagLatest)){
        util parity = Parity_FromBuff(bf);
        Buff_PosAbs(bf, 0);
        StrVec *name = Clone(srv->m, local);
        etag = Etag(srv->m, tags->nvalues, name, parity, &bf->st.st_mtim); 
        Table_Set(tags, name, etag);
        etagStr = Seel_Get(etag, K(m, "tag"));
        etagLatest = Seel_Get(etag, K(m, "last-modified"));
    }else{
        etagStr = Seel_Get(etag, K(m, "tag"));
    }

    HttpReq_SetHeader(req, S(m, "Date"), timeStr);

    if(etagStr != NULL && lastEtag != NULL && since != NULL &&
            Equals(since, etagLatest) && Equals(lastEtag, etagStr)){
        req->type.state |= NOOP;
        File_Close(bf);
    }else{
        Span_Add(req->sections, bf);
    }

    if(etagStr != NULL){
        HttpReq_SetHeader(req, S(m, "Etag"), etagStr);
        HttpReq_SetHeader(req, S(m, "Last-Modified"), timeStr);
    }

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
