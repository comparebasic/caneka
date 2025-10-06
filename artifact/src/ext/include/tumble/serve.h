enum serve_flags {
    SERVE_RUNNING = 1 << 8,
};

typedef Abstract *(*ServeMaker)(struct serve *ctx, Abstract *arg); 

#define SERVE_DELAY 100

typedef struct serve {
    Type type;
    MemCh *m;
    Queue *q;
    i32 socket_fd;
    Handler *start;
    struct {
        ServeMaker req;
        ServeMaker handlers;
    } makers;
    Abstract *source;
    struct {
        microTime start;
        i64 ticks;
        i64 open;
        i64 error;
        i64 served;
    } metrics;
} Serve;
