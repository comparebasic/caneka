#include <formats/http.h>
#include <proto/http.h>

enum proto_types {
    _TYPE_APPS_START = _TYPE_CORE_END,
    TYPE_SERVECTX,
    TYPE_REQ,
    TYPE_PROTO,
    TYPE_PROTODEF,
    TYPE_XMLCTX,
    TYPE_HTTP_PROTO,
    TYPE_HTTP_PROTODEF,
    _TYPE_HTTP_METHODS_START,
    TYPE_METHOD_GET,
    TYPE_METHOD_POST,
    TYPE_METHOD_SET,
    TYPE_METHOD_UPDATE, 
    TYPE_METHOD_DELETE,
    _TYPE_APPS_END,
};

status AppsDebug_Init(MemCtx *m);
