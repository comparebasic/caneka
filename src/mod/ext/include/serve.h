enum serve_types {
    _SERVE_PROTO_START,
    PROTO_HTTP_1_0,
    PROTO_HTTP_1_1,
    PROTO_HTTP_1_2,
    PROTO_HTTP_2,
    PROTO_HTTP_3,
    _SERVE_PROTO_END,
};

#include "serve/handlerdef.h"
#include "serve/req_crit.h"
#include "serve/req.h"
#include "serve/inet.h"
#include "serve/mime.h"
#include "serve/etag.h"
#include "serve/hostent.h"
#include "serve/conn.h"
#include "serve/netaddr.h"
#include "serve/serve.h"
#include "serve/log.h"
#include "serve/tcp_source.h"
#include "serve/serve_init.h"
#include "serve/serve_tos.h"
