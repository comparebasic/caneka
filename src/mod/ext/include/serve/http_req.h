enum req_flags {
    HTTP_REQ_RESPONSE = 1 << 8,
};

typedef struct http_req {
    Type type;
    MemCh *m;
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

void HttpReq_ReadToRbl(MemCh *m, Req *req, Serve *srv);
void HttpReq_RespToRbl(MemCh *m, Req *req, Serve *srv);
void HttpReq_Write(MemCh *m, Req *req, Serve *srv);
void HttpReq_Accept(MemCh *m, Req *req, Serve *srv);
void HttpReq_Close(MemCh *m, Req *req, Serve *srv);
void HttpReq_LogOpen(MemCh *m, HttpReq *req, Serve *srv);
void HttpReq_LogFinalized(MemCh *m, HttpReq *req, Serve *srv);
void HttpReq_SetLength(MemCh *m, HttpReq *hreq);
void HttpReq_WriteStatus(MemCh *m, HttpReq *hreq, Req *req);
void HttpReq_WriteHeaders(MemCh *m, HttpReq *hreq);
void HttpReq_WriteBody(MemCh *m, HttpReq *hreq);
void HttpReq_Finalize(MemCh *m, Req *req, Serve *srv);

status HttpReq_Error(MemCh *m, Req *req, ErrorMsg *msg);
void HttpReq_ParseBody(HttpReq *req);
void *HttpReq_SourceMake(MemCh *m, Abstract *key, HandlerDef *def);
void HttpReq_Setup(MemCh *m, Req *req);
void HttpReq_Serve(MemCh *m, HttpReq *req, Serve *srv);
void HttpReq_SetToResponse(HttpReq *hreq, Req *req);
void HttpReq_SetHeader(HttpReq *req, Str *key, void *value);
void HttpReq_RemoveHeader(HttpReq *req, Str *key);
void HttpReq_Init(MemCh *m);
HandlerDef *HttpReq_DefMake(MemCh *m,
    Span *handlers, Node *ext, Abstract *key, Node *config);
