#define INREQ SERVE_ALPHA
#define OUTREQ SERVE_BRAVO 
#define ERRREQ SERVE_CHARLIE 
#define DRIVEREQ SERVE_DELTA 

typedef struct procioset {
    Type type;
    Span *cmds;
    struct req *in;
    struct req *out;
    struct req *err;
}ProcIoSet;

ProcIoSet *ProcIoSet_Make(MemCtx *m);
status ProcIoSet_Add(ProcIoSet *set, struct req *req);
