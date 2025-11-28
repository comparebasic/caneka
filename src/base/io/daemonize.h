typedef struct deamon {
    Type type;
    MemCh *m;
    struct {
        Str *err;
        Str *log;
        Str *pid;
    } paths;
} ProcIo;

Daemon *Daemon_Make(MemCh *m);
status Daemonize(MemCh *m, Daemon *dm);
