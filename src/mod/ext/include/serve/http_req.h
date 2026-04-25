enum http_types {
    _METHOD_START = _SERVE_PROTO_END,
    METHOD_GET,
    METHOD_POST,
    METHOD_DELETE,
    METHOD_PUT,
    _METHOD_END,
};

typedef struct http_req {
    Type type;
    MemCh *m;
    i32 idx;
    util u;
    /* req end */
    cls method;
    cls proto;
    StrVec *path;
    Table *headers;
    Table *meta;
    Roebling *rbl;
    Buff *in;
    Buff *out;
    Span *sections;
} HttpReq;

void HttpReq_ReadToRbl(Req *req);
void HttpReq_Write(Req *req);
void HttpReq_ExpectRecv(Req *req);
void HttpReq_ExpectSend(Req *req);
Req *HttpReq_Mk(IoCtx *ctx);
