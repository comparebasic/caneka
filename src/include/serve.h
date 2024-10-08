#define ACCEPT_CADANCE 6;
#define SERV_MAX_EVENTS 1024
#define SERV_READ_SIZE 512
#define SERV_WRITE_SIZE 512
#define EPOLL_WAIT 10
#define ROUND_DELAY_SEC 0 
#define ROUND_DELAY_TVSEC 100

#ifdef LINUX
    #define SOCK_OUT EPOLLOUT
    #define SOCK_IN EPOLLIN
#else
    #define SOCK_IN 1
    #define SOCK_OUT 2
#endif

typedef struct serve_ctx {
    Type type;
    MemCtx *m;
#ifdef LINUX
    int epoll_fd;
#else
    Span *queue;
#endif
    int socket_fd;
    int port;
    boolean serving;
    ProtoDef *def;
    struct serve_req *active;
} Serve;

Serve *Serve_Make(MemCtx *m, ProtoDef *def);
status Serve_PreRun(Serve *sctx, int port);
status Serve_Run(Serve *sctx, int port);
status Serve_Stop(Serve *sctx);
status Serve_NextState(Serve *sctx, struct serve_req *req);
status Serve_AcceptRound(Serve *sctx);
status Serve_ServeRound(Serve *sctx);
