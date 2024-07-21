#include <external.h>
#include <caneka.h>

static status setKey(Parser *prs, Range *range, void *source){
    Req *req = (Req *) source;
    Table_SetKey(req->proto->headers_tbl, (Abstract *)Range_Copy(req->m, range));
    return SUCCESS;
}

static status setHdr(Parser *prs, Range *range, void *source){
    Req *req = (Req *) as(source, TYPE_REQ);
    Table_SetValue(req->proto->headers_tbl, (Abstract *)Range_Copy(req->m, range));
    return SUCCESS;
}

static status hdrComplete(Parser *prs, Range *range, void *source){
    return SUCCESS;
}

static Parser *keyParserMk(Roebling *rlb){
    word nl[] = {
        PAT_INVERT, ':', ':', PAT_SINGLE, 'a', 'z', PAT_SINGLE|PAT_TERM, 'A', 'Z',
        PAT_INVERT, ':', ':', PAT_SINGLE, 'a', 'z', PAT_SINGLE, 'A', 'Z', PAT_SINGLE, '-', '-', PAT_SINGLE, '_', '_', PAT_SINGLE|PAT_TERM, '0', '9'};
    Parser *prs = Parser_MakeSingle(rlb->m, Match_MakePat(rlb->m, bytes(nl), 9, ANCHOR_START), setKey); 
    prs->jump = BREAK;
    return prs;
}

static Parser *hdrBodyParserMk(Roebling *rlb){
    word nl[] = {
        PAT_INVERT|PAT_TERM, '\r', '\r', PAT_INVERT|PAT_TERM, '\n', '\n',
        PAT_MANY|PAT_TERM, '*', '~', PAT_END, 0, 0};
    Parser *prs = Parser_MakeSingle(rlb->m, Match_MakePat(rlb->m, bytes(nl), 3, ANCHOR_START), setKey); 
    word enumval = RBL_HEADERS;
    prs->jump = Span_GetIdx(rlb->marks, &enumval, Mark_Eq);
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

static Parser *headerMarkMk(Roebling *rbl){
    return (Parser *)Mark_Make(rbl->m, RBL_HEADERS);
}

Span *HeadersParser_Make(MemCtx *m, ProtoDef *def){
    Span *p =  Span_From(m, 5, 
        (Abstract *)headerMarkMk,
        (Abstract *)keyParserMk,
        (Abstract *)hdrBodyParserMk,
        (Abstract *)hdrNlParserMk,
        (Abstract *)nlDblParserMk);
    return p;
}
