status Daemonize_StdToFiles(MemCh *m, StrVec *outPath, StrVec *errPath);
status Daemonize(MemCh *m,
        SourceFunc func, void *arg, void *source, StrVec *pidPath, i32 *pid,
        char **fmt,
        void **args);
