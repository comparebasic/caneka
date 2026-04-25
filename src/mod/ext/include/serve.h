enum serve_types {
    _SERVE_PROTO_START,
    PROTO_HTTP_1_0,
    PROTO_HTTP_1_1,
    PROTO_HTTP_1_2,
    PROTO_HTTP_2,
    PROTO_HTTP_3,
    _SERVE_PROTO_END,
};

#include "serve/req.h"
#include "serve/ip_address.h"
#include "serve/io_ctx.h"
#include "serve/server.h"
#include "serve/serve_tcp.h"
#include "serve/serve_tos.h"
