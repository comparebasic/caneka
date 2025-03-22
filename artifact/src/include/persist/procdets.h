enum procdets_flags {
    PROCDETS_ASYNC = 1 << 8,
    PROCDETS_PIPES = 1 << 9,
};

typedef struct procdets {
    Type type;
    pid_t pid;
    i32 inFd;
    i32 outFd;
    i32 errFd;
    i32 code;
} ProcDets;

status ProcDets_Init(ProcDets *pd);
