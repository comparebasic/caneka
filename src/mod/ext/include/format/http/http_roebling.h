enum http_types {
    _METHOD_HTTP_START = 0,
    METHOD_HTTP_GET,
    METHOD_HTTP_POST,
    METHOD_HTTP_DELETE,
    METHOD_HTTP_PUT,
    _METHOD_HTTP_END,
};

Roebling *HttpRbl_Make(MemCh *m, Cursor *curs, void *source);
Roebling *HttpRespRbl_Make(MemCh *m, Cursor *curs, void *source);
