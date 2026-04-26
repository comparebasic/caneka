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
void HttpReq_ParseBody(HttpReq *req, Buff *bf);
Req *HttpReq_Mk(IoCtx *ctx);
