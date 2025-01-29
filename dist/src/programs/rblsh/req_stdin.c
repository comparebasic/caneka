

static status RblSh_Wait(Handler *h, Req *req, Serve *sctx){
    status r = Req_Read(sctx, req);
    return h->type.state;
}
