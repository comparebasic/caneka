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
    ParserMaker *parsers;
    char **methods;
} Serve;

Serve *Serve_Make(MemCtx *m);
status Serve_Run(Serve *sctx, int port);
status Serve_NextState(Serve *sctx, struct serve_req *req);
