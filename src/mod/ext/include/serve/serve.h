#define SERVE_READ_SIZE 1024
#define TCP_LISTEN_BACKLOG 192
#define TCP_ZERO_REQ_DELAY 5000

typedef struct serve {
    Type type;
    MemCh *m;
    Queue *q;
    Iter *routeIt /* Hashed<HostEnt/Str, HandlerDef> */;
    struct pollfd *pfds;
    Inst *config;
    void *source;
    struct {
        Buff *out;
        Buff *err;
        Buff *cmd;
    } log;
    struct {
        struct timespec start; 
        util open;
        util served;
        util error;
        util total;
    } metrics;
} Serve;

void Serve_LogOpen(Serve *srv, Req *req);
void Serve_LogFinalized(Serve *srv, Req *req);
struct pollfd *Serve_TcpGetPollFd(Req *req);
void Serve_ServeTcp(Serve *srv);

Serve *Serve_Make(MemCh *m, Table *routes, Span *addrs, void *source);
