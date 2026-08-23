enum persist_flags {
    PERSIST_TRACKED = 1 << 8
};

extern char **environ;
status Persist_Init(MemCh *m);
