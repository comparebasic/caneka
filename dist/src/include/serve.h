#define ACCEPT_CADANCE 6;
#define SERV_MAX_EVENTS 1024
#define SERV_READ_SIZE 512
#define SERV_WRITE_SIZE 512
#define EPOLL_WAIT 10
#define ROUND_DELAY_SEC 0 
#define ROUND_DELAY_TVSEC 5000000
#define ACCEPT_AT_ONEC_MAX 8 
#define ACCEPT_DELAY_MILLIS 50
#define ACCEPT_LONGDELAY_MILLIS 5000

#ifdef LINUX
    #define SOCK_OUT EPOLLOUT
    #define SOCK_IN EPOLLIN
#else
    #define SOCK_IN POLLIN
    #define SOCK_OUT POLLOUT
#endif

typedef struct serve_ctx {
    Type type;
    MemCtx *m;
    Queue queue;
    Span *pollMap;
    int socket_fd;
    int port;
    boolean serving;
    ProtoDef *def;
    struct serve_req *active;
    struct {
        int open;
        int served;
        int error;
        int collected;
        struct timespec started;
        int tickNanos;
    } metrics;
} Serve;

Serve *Serve_Make(MemCtx *m, ProtoDef *def);
status Serve_PreRun(Serve *sctx, int port);
struct serve_req *Serve_AddFd(Serve *sctx, int fd);
status Serve_RunPort(Serve *sctx, int port);
status Serve_Run(Serve *sctx);
status Serve_Stop(Serve *sctx);
status Serve_NextState(Serve *sctx, struct serve_req *req);
status Serve_AcceptPoll(Serve *sctx, int delay);
status Serve_ServeRound(Serve *sctx);
status Serve_CloseReq(Serve *sctx, struct serve_req *req, int idx);
void Delay();
