enum req_flags {
    HTTP_REQ_RESPONSE = 1 << 8,
};

typedef struct http_req {
    Type type;
    MemCh *m;
    i32 idx;
    util *slot;
    Iter routeIt;
    struct {
        struct timespec start;
        struct timespec end;
    } metrics;
    /* req end */
    HostEnt *clientEnt;
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
} HttpReq;

status HttpReq_ReadToRbl(MemCh *m, HttpReq *req, Serve *srv);
status HttpReq_RespToRbl(MemCh *m, HttpReq *req, Serve *srv);
status HttpReq_Write(MemCh *m, HttpReq *req, Serve *srv);

void HttpReq_ExpectRecv(HttpReq *req);
void HttpReq_ExpectSend(HttpReq *req);
void HttpReq_ExpectInternal(HttpReq *req);
void HttpReq_ParseBody(HttpReq *req);
void HttpReq_SetFd(HttpReq *req, i32 fd);
void HttpReq_Close(HttpReq *req);
Req *HttpReq_Mk(MemCh *m, Serve *srv);
void HttpReq_Setup(MemCh *m, Req *req, Serve *srv);
void HttpReq_Serve(MemCh *m, HttpReq *req, Serve *srv);
void HttpReq_SetToRecv(HttpReq *req);
void HttpReq_SetToResponse(HttpReq *req, i32 fd);
void HttpReq_SetHeader(HttpReq *req, Str *key, void *value);
void HttpReq_RemoveHeader(HttpReq *req, Str *key);
