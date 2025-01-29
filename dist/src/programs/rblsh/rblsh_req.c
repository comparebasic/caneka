
static Req *RblSh_ReqMake(MemCtx *_m, Serve *sctx){
    MemCtx *m = MemCtx_Make();
    IoProto *proto = (IoProto *)IoProto_Make(m, (ProtoDef *)sctx->def);

    Req *req =  Req_Make(m, sctx, (Proto *)proto);
    if((sctx->type.of & INREQ) != 0){
        req->in.rbl = RblShParser_Make(m, 
            (RblShCtx *)sctx->def->source,
            req->in.shelf);
    }else{
        req->in.rbl = RblTermIo_Make(m, 
            (RblShCtx *)sctx->def->source,
            req->in.shelf);
    }
    req->type.state |= sctx->type.of & (INREQ|OUTREQ|ERRREQ);
    req->data = Span_Make(m, TYPE_SPAN);


    return req;
}
