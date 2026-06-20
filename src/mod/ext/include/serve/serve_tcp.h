#define TCP_LISTEN_BACKLOG 192
#define TCP_ZERO_REQ_DELAY 5000

struct pollfd *Serve_TcpGetPollFd(Req *req);
void Serve_ServeTcp(Serve *srv);

Serve *Serve_MakeTcp(MemCh *m, HandlerDef *def, HostEnt *ent);
