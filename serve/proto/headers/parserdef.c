static status setKey(Parser *prs, Range *range, void *source){
    Req *req = (Req *) source;
    Table_SetKey(req->proto->headers, Range_Copy(req->m, range));
}

static status setHdr(Parser *prs, Range *range, void *source){
    Req *req = (Req *) as(source, TYPE_REQ);
    Table_SetValue(req->proto->headers, Range_Copy(req->m, range));
    return SUCCESS;
}

static Parser *nlDblParserMk(Roebling *rlb){
    word nl[] = {
        PAT_INVERT, ':', ':', PAT_SINGLE, 'a', 'z', PAT_SINGLE|PAT_TERM, 'A', 'Z',
        PAT_INVERT, ':', ':', PAT_SINGLE, 'a', 'z', PAT_SINGLE, 'A', 'Z', PAT_SINGLE, '-', '-', PAT_SINGLE, '_', '_', PAT_SINGLE|PAT_TERM, '0', '9'};
    Parser *prs = Parser_MakeSingle(rlb->m, Match_MakePat(rlb->m, bytes(nl), 9, ANCHOR_START), setKey); 
    prs->jump = BREAK;
    return prs;
}

static Parser *nlDblParserMk(Roebling *rlb){
    word nl[] = {PAT_TERM, '\r', '\r', PAT_TERM, '\n', '\n', PAT_TERM, '\r', '\r', PAT_TERM, '\n', '\n'};
    Parser *prs = Parser_MakeSingle(rlb->m, Match_MakePat(rlb->m, bytes(nl), 4, ANCHOR_START), setHdr); 
    prs->jump = BREAK;
    return prs;
}

static Parser *hdrNlParserMk(Roebling *rlb){
    word nl[] = {PAT_TERM, '\r', '\r', PAT_TERM, '\n', '\n', PAT_SET_NOOP, '\t', '\t', PAT_SET_NOOP|PAT_TERM, ' ', ' ', PAT_END};
    return Parser_MakeSingle(rlb->m, Match_MakePat(rlb->m, bytes(nl), 4, ANCHOR_START), hdrComplete); 
}

static Parser *headerMarkMk(Roebling *rlb){
    return Mark_Make(rbl, RBL_HEADERS);
}

Span *HeadersParser_Make(MemCtx *m, ProtoDef *def){
    Span *p =  Span_From(m, 6, 
        (Abstract *)headerMarkMk,
        (Abstract *)keyParserMk,
        (Abstract *)hdrBodyParserMk,
        (Abstract *)hdrNlParserMk,
        (Abstract *)nlDblParserMk,
    return p;
}
