typedef struct server {
    Type type;
    MemCh *m;
    IoCtx *ctx;
    Table *routes; /*<chain>*/
    Table *keys;
    Queue *q;
    struct {
        Buff *out;
        Buff *err;
    } log;
} Server;

Server *Server_Make(MemCh *m, IoCtx *ctx);
