#define SERVE_READ_SIZE 1024

typedef struct serve {
    Type type;
    MemCh *m;
    util u;
    Queue *q;
    struct {
        Buff *out;
        Buff *err;
    } log;
    HandlerDef def;
    union {
        HostEnt *hostent;
        StrVec *path;
    } address;
    struct {
        struct timespec start; 
        util open;
        util served;
        util error;
        util total;
    } metrics;
    void *source;
} Serve;

void Serve_LogOpen(Server *srv, Req *req);
void Serve_LogFinalized(Server *srv, Req *req);

Serve *Serve_MakeTcp(MemCh *m, Req_Mk mk, DoFunc handle, DoFunc finalize, Table *chain);
Serve *Serve_MakeFileSystem(MemCh *m, Req_Mk mk, DoFunc handle, DoFunc finalize, Table *chain);
