#define ACCEPT_CADANCE 6;
#define SERV_MAX_EVENTS 1024
#define SERV_READ_SIZE 512
#define SERV_WRITE_SIZE 512
#define EPOLL_WAIT 10

typedef struct serve_ctx {
    MemCtx *m;
    int epoll_fd;
    int socket_fd;
    int port;
    boolean serving;
    ProtoDef *def;
} Serve;

Serve *Serve_Make(MemCtx *m, ProtoDef *def);
status Serve_PreRun(Serve *sctx, int port);
status Serve_Run(Serve *sctx, int port);
status Serve_Stop(Serve *sctx);
status Serve_NextState(Serve *sctx, struct serve_req *req);
status Serve_AcceptRound(Serve *sctx);
status Serve_ServeRound(Serve *sctx);
