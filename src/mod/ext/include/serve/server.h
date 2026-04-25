#define SERVE_READ_SIZE 1024

typedef struct server {
    Type type;
    util u;
    MemCh *m;
    IoCtx *ctx;
    Table *routes; /*<chain>*/
    Table *keys;
    Queue *q;
    struct {
        Buff *out;
        Buff *err;
    } log;
    struct {
        struct timespec start; 
        util open;
        util served;
        util error;
        util total;
    } metrics;
} Server;

Server *Server_Make(MemCh *m, IoCtx *ctx);
