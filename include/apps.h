enum proto_types {
    _TYPE_PROTO_START = _TYPE_CORE_END,
    TYPE_HTTP_PROTO,
    TYPE_HTTP_REQ,
    TYPE_HTTP_PARSER,
    TYPE_HTTP_PROTODEF,
    _TYPE_PROTO_END,
    _TYPE_HTTP_START,
    TYPE_METHOD_GET,
    TYPE_METHOD_POST,
    TYPE_METHOD_SET,
    TYPE_METHOD_UPDATE, 
    TYPE_METHOD_DELETE,
    _TYPE_HTTP_END,
};

#define _TYPE_PROTO_END _TYPE_HTTP_END
