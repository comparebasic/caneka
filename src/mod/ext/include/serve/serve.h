#define SERVE_READ_SIZE 1024

typedef struct serve {
    Type type;
    MemCh *m;
    Queue *q;
    Table *routes /* <HostEnt, HandlerDef> */;
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

void Serve_LogOpen(Serve *srv, Inst *tsk);
void Serve_LogFinalized(Serve *srv, Inst *tsk);

Serve *Serve_Make(MemCh *m, Table *routes, Span *addrs, void *source);
