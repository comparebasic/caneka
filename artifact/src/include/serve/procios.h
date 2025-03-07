enum procio_flags {
    PROCIO_INREQ = 1 << 8,
    PROCIO_OUTREQ = 1 << 9, 
    PROCIO_ERRREQ = 1 << 10,
};

typedef struct procioset {
    Type type;
    Span *cmds;
    struct procdets pd;
    int code;
    struct req *in;
    struct req *out;
    struct req *err;
}ProcIoSet;

char *ProcIoSet_FlagsToChars(word flags);
ProcIoSet *ProcIoSet_Make(MemCtx *m);
status ProcIoSet_Add(ProcIoSet *set, struct req *req);
status ProcIoSet_SegFlags(ProcIoSet *pio, struct serve_ctx *sctx, status flags);
ProcIoSet *ProcIoSet_SubProc(Serve *sctx, Span *cmds);
