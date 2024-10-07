Handler *Example_getHandler(Serve *sctx, Req *req);
status Example_Setup(Handler *h, Req *req, Serve *sctx);
status Example_Recieve(Handler *h, Req *req, Serve *sctx);
status Example_Respond(Handler *h, Req *req, Serve *sctx);
status Example_Complete(Handler *h, Req *req, Serve *sctx);
