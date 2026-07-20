enum req_flags {
    HTTP_REQ_RESPONSE = 1 << 8,
};

typedef struct req_http {
    Type type;
    NetAddr *addr;
    struct {
        i16 method;
        i16 proto;
    } address;
    StrVec *path;
    Buff *in;
    Buff *out;
    Iter headersIt;
    Table *headersOut;
    Iter queryIt;
    Table *meta;
    Roebling *rbl;
    void *body;
    Span *sections;
} ReqHttp;

status ReqHttp_ReadToRbl(MemCh *m, ReqHttp *req, Serve *srv);
status ReqHttp_RespToRbl(MemCh *m, ReqHttp *req, Serve *srv);
status ReqHttp_Write(MemCh *m, ReqHttp *req, Serve *srv);
status ReqHttp_Accept(MemCh *m, Req *_req, Serve *srv);
status ReqHttp_Close(MemCh *m, Req *_req, Serve *srv);

void ReqHttp_ExpectRecv(ReqHttp *req);
void ReqHttp_ExpectSend(ReqHttp *req);
void ReqHttp_ExpectInternal(ReqHttp *req);
void ReqHttp_ParseBody(ReqHttp *req);
void ReqHttp_SetFd(ReqHttp *req, i32 fd);
Req *ReqHttp_Mk(MemCh *m, Serve *srv);
void ReqHttp_Setup(MemCh *m, Req *req, Serve *srv);
void ReqHttp_Serve(MemCh *m, ReqHttp *req, Serve *srv);
void ReqHttp_SetToRecv(ReqHttp *req);
void ReqHttp_SetToResponse(ReqHttp *req, i32 fd);
void ReqHttp_SetHeader(ReqHttp *req, Str *key, void *value);
void ReqHttp_RemoveHeader(ReqHttp *req, Str *key);
