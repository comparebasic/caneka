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
    i32 status;
    Cursor *in;
    Cursor *out;
} HttpCtx;

HttpCtx *HttpCtx_Make(MemCh *m);
HttpCtx *HttpCtx_AddRequestSteps(Task *tsk);
HttpCtx *HttpCtx_AddResponseSteps(Task *tsk);
