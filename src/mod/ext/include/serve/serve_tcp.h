#define TCP_LISTEN_BACKLOG 192
#define TCP_ZERO_REQ_DELAY 5000
typedef struct tcp_source {
    Type type;
    i32 new_fd;
    HostEnt *clientEnt;
} TcpSource;

struct pollfd *Serve_TcpGetPollFd(Req *req);
void Serve_ServeTcp(Serve *srv);
TcpSource *TcpSource_Make(MemCh *m);

Serve *Serve_MakeTcp(MemCh *m, HandlerDef *def, HostEnt *ent);
