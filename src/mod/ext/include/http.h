enum http_types {
    _METHOD_HTTP_START = _SERVE_PROTO_END,
    METHOD_HTTP_GET,
    METHOD_HTTP_POST,
    METHOD_HTTP_DELETE,
    METHOD_HTTP_PUT,
    _METHOD_HTTP_END,
};

#include "http/http_roebling.h"
#include "http/http_req.h"
#include "http/http_static.h"
#include "http/http_tos.h"
