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
    HttpReq_Setup(m, req, srv);
    HttpReq_SetToRecv((HttpReq *)req->source, req);
}

static void HttpStatic_Finalize(MemCh *m, HttpReq *req, Serve *srv){
    HttpReq_Close(m, (Req *)req, srv);

    Buff *bf = NULL;
    if(req->type.state & ERROR && srv->log.err != NULL){
        bf = srv->log.err;
    }else{
        bf = srv->log.out;
    }

    Table *tbl = Table_Make(m);

    Single *st = NULL;
    Str *format = NULL;

    if((req->type.state & (ERROR|NOOP)) == (ERROR|NOOP)){
        st = I32_Wrapped(m, 404);
        if(bf->type.state & BUFF_COLOR){
            format = AnsiYellow;
        }
    }else if(req->type.state & ERROR){
        st = I32_Wrapped(m, 500);
        if(bf->type.state & BUFF_COLOR){
            format = AnsiRed;
        }
    }else if(req->type.state & NOOP){
        st = I32_Wrapped(m, 304);
        if(bf->type.state & BUFF_COLOR){
            format = AnsiCyan;
        }
    }else{
        st = I32_Wrapped(m, 200);
        if(bf->type.state & BUFF_COLOR){
            format = AnsiCyan;
        }
    }

    Time_Now(&req->metrics.end);
    duration d = Time_Duration(m, &req->metrics.end, &req->metrics.start);
    Table_Set(tbl, K(m, "path"), req->path);
    Table_Set(tbl, K(m, "status"), st);
    Table_Set(tbl, K(m, "duration"), Duration_Str(m, d));
    Table_Set(tbl, K(m, "from"), req->addr);

    Log_Flat(m, bf, K(m, "Request "), Table_Ordered(m, tbl), format);
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
    if(bf->type.state & ERROR){
        req->type.state |= NOOP;
        Return(m, req->type.state);
    }
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

    HttpReq_ExpectSend(req);
    req->type.state |= SUCCESS;
    Return(m, req->type.state);
}


