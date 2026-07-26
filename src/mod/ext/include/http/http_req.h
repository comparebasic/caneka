enum req_flags {
    HTTP_REQ_RESPONSE = 1 << 8,
};

typedef struct http_req {
    Type type;
    MemCh *m;
    NetAddr *addr;
    Str *key;
    struct {
        i16 method;
        i16 proto;
    } address;
    StrVec *path;
    Buff *in;
    Buff *out;
    Capsule *cap;
    Iter headersIt;
    Table *headersOut;
    Iter queryIt;
    Table *meta;
    Roebling *rbl;
    void *body;
    Span *sections;
} HttpReq;

status HttpReq_ReadToRbl(MemCh *m, Req *req, Serve *srv);
status HttpReq_RespToRbl(MemCh *m, Req *req, Serve *srv);
status HttpReq_Write(MemCh *m, Req *req, Serve *srv);
status HttpReq_Accept(MemCh *m, Req *req, Serve *srv);
status HttpReq_Close(MemCh *m, Req *req, Serve *srv);

void HttpReq_ParseBody(HttpReq *req);
Req *HttpReq_Mk(MemCh *m, Serve *srv);
void HttpReq_Setup(MemCh *m, Req *req, Serve *srv);
void HttpReq_Serve(MemCh *m, HttpReq *req, Serve *srv);
void HttpReq_SetToRecv(HttpReq *hreq, Req *req);
void HttpReq_SetToResponse(HttpReq *hreq, Req *req, i32 fd);
void HttpReq_SetHeader(HttpReq *req, Str *key, void *value);
void HttpReq_RemoveHeader(HttpReq *req, Str *key);
