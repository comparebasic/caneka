enum procio_flags {
    PROCIO_INREQ = 1 << 8,
    PROCIO_OUTREQ = 1 << 9, 
    PROCIO_ERRREQ = 1 << 10,
};

typedef struct procioset {
    Type type;
    Span *cmds;
    pid_t pid;
    int code;
    struct req *in;
    struct req *out;
    struct req *err;
}ProcIoSet;

ProcIoSet *ProcIoSet_Make(MemCtx *m);
status ProcIoSet_Add(ProcIoSet *set, struct req *req);
status ProcIoSet_FlagAll(ProcIoSet *pio, word set, word unset);
