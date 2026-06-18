enum req_flags {
    HTTP_REQ_RESPONSE = 1 << 8,
};

typedef struct http_req {
    Type type;
    MemCh *m;
    i32 idx;
    util u;
    Iter routeIt;
    /* req end */
    HostEnt *clientEnt;
    StrVec *path;
    Buff *in;
    Buff *out;
    Iter headersIt;
    Iter queryIt;
    Table *meta;
    Roebling *rbl;
    void *body;
    Span *sections;
} HttpReq;

void HttpReq_ReadToRbl(HttpReq *req);
void HttpReq_RespToRbl(MemCh *m, HttpReq *req, Srv *srv);
void HttpReq_Write(HttpReq *req);

void HttpReq_ExpectRecv(HttpReq *req);
void HttpReq_ExpectSend(HttpReq *req);
void HttpReq_ParseBody(HttpReq *req);
void HttpReq_SetFd(HttpReq *req, i32 fd);
void HttpReq_Close(HttpReq *req);
Req *HttpReq_Mk(Serve *srv);
void HttpReq_Setup(Serve *srv, Req *_req){
void HttpReq_SetToRecv(HttpReq *req);
void HttpReq_SetToResponse(HttpReq *req, i32 fd);

