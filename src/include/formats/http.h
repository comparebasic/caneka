enum http_marks {
    HTTP_MARK_HEADER,
    HTTP_MARK_END,
    _HTTP_MARK_MAX = 1 << 15
};

enum http_methods {
    HTTP_METHOD_GET = 1,
    HTTP_METHOD_POST,
    HTTP_METHOD_UPDATE,
    HTTP_METHOD_SET,
    HTTP_METHOD_DELETE
};

enum http_captures {
    HTTP_METHOD,
    HTTP_PATH,
    HTTP_PROTO,
    HTTP_HEADER,
    HTTP_HEADER_VALUE,
    _HTTP_MAX = 1 << 15
};

Roebling *HttpParser_Make(MemCtx *m, String *s, Abstract *source);
