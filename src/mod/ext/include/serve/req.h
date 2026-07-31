typedef struct req {
    Type type;
    MemCh *m;
    i32 idx;
    ReqCrit *crit;
    HandlerDef *def;
    Span *extPath;
    union {
        struct host_ent_wr *ent;
        Buff *bf;
    } conn;
    Iter chain;
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
Req *Req_Make(MemCh *m, HandlerDef *def, void *source);
void Req_Prepare(MemCh *m, Req *req, struct serve *srv);
