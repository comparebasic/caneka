#include <formats/http.h>
#include <formats/rblsh.h>
#include <serve/proto/http.h>
#include <serve/proto/io.h>

enum boundries {
    _APP_BOUNDRY_START = _TYPE_CORE_END,
    _IO_START,
    IO_RECV,
    IO_RUN,
    IO_DONE,
    _IO_END,
    _CASH_START,
    CASH_MARK_START,
    CASH_BETWEEN,
    CASH_VALUE,
    CASH_NOOP,
    _CASH_END,
    _RBLSH_START,
    RBLSH_MARK_START,
    RBLSH_MARK_END,
    RBLSH_TERM,
    RBLSH_HUP,
    RBLSH_WS,
    RBLSH_NL,
    RBLSH_ARG,
    RBLSH_FLAG,
    RBLSH_WORDFLAG,
    RBLSH_OP,
    RBLSH_STRING_LIT,
    RBLSH_STRING_OP,
    _RBLSH_END,
    _APPS_BOUNDRIES_START = _TYPE_CORE_END,
    _APPS_CASH_START,
    _APPS_RBLSH_START = _CASH_END,
    _APPS_TYPE_START = _RBLSH_END,
};

enum proto_types {
    _TYPE_APPS_START = _APPS_TYPE_START,
    TYPE_RBLSH_CTX,
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
    TYPE_IO_PROTO,
    TYPE_IO_PROTODEF,
    _TYPE_APPS_END,
};

status AppsDebug_Init(MemCtx *m);
char *AppRange_ToChars(word range);
