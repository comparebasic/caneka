#include <external.h>
#include <caneka.h>

static void HttpStatic_Cmd(MemCh *m, Req *_req, Serve *srv){
    HttpReq *req = (HttpReq *)_req;
    Buff_ReadAmount(req->in, SERVE_READ_SIZE);
    Buff *bf = Buff_Make(m, ZERO);

    Buff_Pipe(bf, req->in);
    void *ar[] = {
        bf->v,
        NULL
    };
    Out("^p.Cmd: @^0\n", ar);
}

static void HttpStatic_Setup(MemCh *m, Req *req, Serve *srv){
    Req_ExpectInternal(req);
    Iter *it = Iter_Make(m);
    Iter_Add(it, Func_Wrapped(m, HttpReq_RespToRbl));
    Iter_Add(&req->chain, it);
    req->type.state |= MORE;
}

void HttpStatic_RetrieveFile(MemCh *m, Req *req, Serve *srv){
    Debug_Push(m, req);
    HttpReq *hreq = (HttpReq *)req->source;

    StrVec *local = IoPath_FromVec(m, hreq->path);
    IoUtil_Relativise(m, local);

    if(IoUtil_IsSep(Span_Get(local->p, local->p->max_idx))){
        StrVec_Add(local, S(m, "index.html"));
    }

    Str *dir = Seel_Get(srv->config, K(m, "dir"));
    StrVec *path = Clone(m, dir);
    StrVec_AddVec(path, local);

    StrVec *ext = IoUtil_GetExt(m, local); 
    Str *mime = Table_Get(MimeByExt, ext);
    if(mime){
        HttpReq_SetHeader(hreq, S(m, "Content-Type"), mime);
    }

    Buff *bf = Buff_Make(m, BUFF_UNBUFFERED);
    File_Open(bf, path, O_RDONLY);
    if(bf->type.state & ERROR){
        req->type.state |= NOOP;
        ReturnVoid(m);
    }
    Buff_Stat(bf);

    Str *timeStr = Time_ToRStr(m, &bf->st.st_mtim);

    StrVec *lastEtag = Table_GetByIter(&hreq->headersIt, K(m, "If-None-Match"));
    StrVec *since = Table_GetByIter(&hreq->headersIt, K(m, "If-Modified-Since"));

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

    HttpReq_SetHeader(hreq, S(m, "Date"), timeStr);

    if(etagStr != NULL && lastEtag != NULL && since != NULL &&
            Equals(since, etagLatest) && Equals(lastEtag, etagStr)){
        req->type.state |= NOOP;
        File_Close(bf);
    }else{
        Span_Add(hreq->sections, bf);
    }

    if(etagStr != NULL){
        HttpReq_SetHeader(hreq, S(m, "Etag"), etagStr);
        HttpReq_SetHeader(hreq, S(m, "Last-Modified"), timeStr);
    }

    Req_ExpectSend(req);
    req->type.state |= SUCCESS;
    ReturnVoid(m);
}
