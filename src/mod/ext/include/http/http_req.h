enum req_flags {
    HTTP_REQ_RESPONSE = 1 << 8,
};

typedef struct http_req {
    Type type;
    MemCh *m;
    i32 idx;
    util u;
    /* req end */
    IpAddress address;
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
void HttpReq_Write(HttpReq *req);
void HttpReq_ExpectRecv(HttpReq *req);
void HttpReq_ExpectSend(HttpReq *req);
void HttpReq_ParseBody(HttpReq *req);
void HttpReq_SetFd(HttpReq *req, i32 fd);
void HttpReq_Close(HttpReq *req);
void HttpReq_SetToResponse(HttpReq *req, i32 fd);
Req *HttpReq_Mk(IoCtx *ctx);
