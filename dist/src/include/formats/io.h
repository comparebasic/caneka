enum io_methods {
    IO_RECV = 1,
    IO_RUN = 1,
    IO_DONE = 1,
};

Roebling *IoParser_Make(MemCtx *m, String *s, Abstract *source);
char *IoProto_MethodToChars(int methodIdx);
