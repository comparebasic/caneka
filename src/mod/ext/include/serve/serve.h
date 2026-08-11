#define SERVE_READ_SIZE 1024
#define TCP_LISTEN_BACKLOG 192
#define TCP_ZERO_REQ_DELAY 5000
#define REQ_HANDLE_MAX 64 

typedef struct serve {
    Type type;
    MemCh *m;
    Queue *q;
    i32 fixedIdx;
    Iter endPointIt /* Hashed<HostEnt/Str, HandlerDef> */;
    Span *pfds;
    Inst *config;
    Table *etags;
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

extern Table *ServeProtoTable;
i32 Serve_PortByService(Str *s);

void Serve_LogOpen(Serve *srv, Req *req);
void Serve_LogFinalized(Serve *srv, Req *req);
struct pollfd *Serve_TcpGetPollFd(Req *req);
void Serve_AddEndpoint(Serve *srv, Abstract *key, HandlerDef *def);
void Serve_Serve(Serve *srv);
void Serve_Init(MemCh *m);

Serve *Serve_Make(MemCh *m);
