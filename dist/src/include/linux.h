status Serve_Epoll(Serve *sctx);
status Serve_EpollEvUpdate(Serve *sctx, Req *req, int direction);
status Serve_EpollEvAdd(Serve *sctx, Req *req, int fd, int direction);
status Serve_EpollEvRemove(Serve *sctx, Req* req);
