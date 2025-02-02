enum procdets_flags {
    PROCDETS_ASYNC = 1 << 8,
};

typedef struct procdets {
    Type type;
    pid_t pid;
    int inFd;
    int outFd;
    int errFd;
    int code;
} ProcDets;

status ProcDets_Init(ProcDets *pd);
