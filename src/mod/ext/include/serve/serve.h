#define SERVE_READ_SIZE 1024

typedef struct serve {
    Type type;
    MemCh *m;
    util u;
    Queue *q;
    HandlerDef *def;
    struct {
        Buff *out;
        Buff *err;
    } log;
    union {
        HostEnt *ent;
        StrVec *path;
    } address;
    struct {
        struct timespec start; 
        util open;
        util served;
        util error;
        util total;
    } metrics;
    Inst *config;
    void *source;
} Serve;

void Serve_LogOpen(Serve *srv, Req *req);
void Serve_LogFinalized(Serve *srv, Req *req);

Serve *Serve_Make(MemCh *m);
