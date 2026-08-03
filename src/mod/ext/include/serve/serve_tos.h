status Serve_TosInit(MemCh *m);
status Http_TosInit(MemCh *m);
Str *HttpMethodStr(MemCh *m, cls method);
StrVec *Serve_PollFlagVec(MemCh *m, struct pollfd *pfd);
