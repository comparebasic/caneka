extern struct lookup * HttpMethods;
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
    Str *mime;
    StrVec *path;
    Buff *content;
    Buff *body;
    Table *headers;
    Span *errors;
} HttpCtx;

HttpCtx *HttpCtx_Make(MemCh *m);
ProtoCtx *HttpProto_Make(MemCh *m);
