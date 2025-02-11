enum example_htypes {
    _TYPE_EXAMPLE_H_START = _TYPE_APPS_END,
    TYPE_EXAMPLE_H_START,
    TYPE_EXAMPLE_H_SETUP,
    TYPE_EXAMPLE_H_RECEIVE,
    TYPE_EXAMPLE_H_RESPOND,
    TYPE_EXAMPLE_H_PROCESS,
    TYPE_EXAMPLE_H_COMPLETE,
    _TYPE_EXAMPLE_H_END,
};

Handler *Example_getHandler(Serve *sctx, Req *req);
status Example_Setup(Handler *h, Req *req, Serve *sctx);
status Example_Recieve(Handler *h, Req *req, Serve *sctx);
status Example_Respond(Handler *h, Req *req, Serve *sctx);
status Example_Complete(Handler *h, Req *req, Serve *sctx);
char *Example_ToChars(int range);
