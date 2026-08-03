#define RECV_FLAGS (POLLIN|POLLNVAL|POLLHUP|POLLERR)
#define BIDIR_FLAGS (POLLOUT|POLLIN|POLLNVAL|POLLHUP|POLLERR)
#define SEND_FLAGS (POLLOUT|POLLNVAL|POLLHUP|POLLERR)
#define INTERNAL_FLAGS (POLLNVAL|POLLHUP|POLLERR)

typedef struct req {
    Type type;
    MemCh *m;
    i32 idx;
    ReqCrit *crit;
    HandlerDef *def;
    Span *keys;
    Iter route;
    struct netaddr *addr;
    struct {
        struct timespec start;
        struct timespec end;
    } metrics;
    void *source;
} Req;

void Req_ExpectRecv(Req *req);
void Req_ExpectSend(Req *req);
void Req_ExpectInternal(Req *req);
void Req_SetFd(Req *req, i32 fd);
void Req_Handle(MemCh *m, Req *req, struct serve *srv);
Req *Req_Make(MemCh *m, HandlerDef *def, struct netaddr *addr, i32 fd, void *source);
