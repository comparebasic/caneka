enum http_status {
    HTTP_STATUS_OK = 200,
    HTTP_STATUS_REDIRECT = 301,
    HTTP_STATUS_TEMP_REDIRECT = 302,
    HTTP_STATUS_FORBIDDON = 403,
    HTTP_STATUS_NOT_FOUND = 404,
    HTTP_STATUS_ERROR = 500,
};

typedef struct {
    Type type;
    i32 method;
    i32 code;
    i64 contentLength;
    Str *mime;
    StrVec *httpVersion;
    StrVec *path;
    StrVec *body;
    Route *route;
    Table *data;
    Span *errors;
    Iter headersIt;
} HttpCtx;

HttpCtx *HttpCtx_Make(MemCh *m);
status HttpCtx_WriteHeaders(Buff *bf, HttpCtx *ctx);
status HttpCtx_Init(MemCh *m);
