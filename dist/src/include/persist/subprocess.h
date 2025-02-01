status SubProcess(MemCtx *m, Span *cmd, String *msg_s, Abstract *source);
int SubCall(MemCtx *m, Span *cmd_p, String *msg_s, Abstract *source);
int SubStatus(int pid, boolean wait);
