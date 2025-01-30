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

enum serve_ctx_flags {
    /* reserver for app specific use */
    SERVE_ALPHA = 1 << 8, 
    SERVE_BRAVO = 1 << 9,
    SERVE_CHARLIE = 1 << 10,
    SERVE_DELTA = 1 << 11,
    SERVE_ECHO = 1 << 12,
    SERVE_FOXTROT = 1 << 13,
    SERVE_GOLF = 1 << 14,
    SERVE_HOTEL = 1 << 15,
};

typedef struct serve_ctx {
    Type type;
    MemCtx *m;
    Queue queue;
    Span *pollMap;
    int socket_fd;
    int port;
    boolean serving;
    ProtoDef *def;
    Abstract *group;
    struct req *active;
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
status Serve_StartGroup(Serve *sctx, Abstract *group);
status Serve_PreRun(Serve *sctx, int port);
struct req *Serve_AddFd(Serve *sctx, int fd);
status Serve_RunPort(Serve *sctx, int port);
status Serve_Run(Serve *sctx);
status Serve_RunFds(Serve *sctx);
status Serve_Stop(Serve *sctx);
status Serve_NextState(Serve *sctx, struct req *req);
status Serve_AcceptPoll(Serve *sctx, int delay);
status Serve_ServeRound(Serve *sctx);
status Serve_CloseReq(Serve *sctx, struct req *req, int idx);
void Delay();
