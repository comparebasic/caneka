#include <external.h>
#include <caneka.h>

static status setKey(Parser *prs, Range *range, void *source){
    Req *req = (Req *) source;
    Table_SetKey(req->proto->headers_tbl, (Abstract *)Range_Copy(req->m, range));
    return SUCCESS;
}

static status setHdr(Parser *prs, Range *range, void *source){
    Req *req = (Req *) as(source, TYPE_REQ);
    Abstract *value = (Abstract *)Range_Copy(req->m, range);

    String *key = Span_Get(req->proto->headers_tbl, req->proto->headers_tbl->metrics.set);
    Single *wrp = (Single *)Table_Get(req->sctx->def->hdrHandlers_tbl_mk, (Abstract *)key);
    if(wrp != NULL){
        value = ((Maker)wrp->val.ptr)(req->m, (Abstract *)value); 
    }

    Table_SetValue(req->proto->headers_tbl, value);
    return SUCCESS;
}

static status setBody(Parser *prs, Range *range, void *source){
    /*
    Req *req = (Req *) as(source, TYPE_REQ);
    Table_SetValue(req->proto->headers_tbl, (Abstract *)Range_Copy(req->m, range));
    */
    return SUCCESS;
}

static status hdrComplete(Parser *prs, Range *range, void *source){
    return SUCCESS;
}

static Parser *keyParserMk(Roebling *rlb){
    word nl[] = {
        PAT_INVERT|PAT_IGNORE, ':', ':', PAT_MANY, 'a', 'z', PAT_MANY|PAT_TERM, 'A', 'Z',
        PAT_INVERT|PAT_IGNORE, ':', ':', PAT_MANY, 'a', 'z', PAT_MANY, 'A', 'Z', PAT_MANY, '-', '-', PAT_MANY, '_', '_', PAT_MANY|PAT_TERM, '0', '9'};
    Parser *prs = Parser_MakeSingle(rlb->m, Match_MakePat(rlb->m, bytes(nl), 9, ANCHOR_START), setKey); 
    prs->jump = BREAK;
    return prs;
}

static Parser *hdrSepParserMk(Roebling *rlb){
    word sep[] = {
        PAT_SINGLE|PAT_TERM, ':', ':',
        PAT_ANY, '\t', '\t', PAT_ANY|PAT_TERM, ' ', ' ', PAT_END, 0, 0};
    return Parser_MakeSingle(rlb->m, Match_MakePat(rlb->m, bytes(sep), 3, ANCHOR_START), NULL); 
}

static Parser *hdrBodyParserMk(Roebling *rlb){
    word hdr[] = {PAT_ANY, '\r', '\r', PAT_ANY, '\n', '\n', PAT_MANY|PAT_TERM, '*', '~', PAT_END, 0, 0};
    Parser *prs = Parser_MakeSingle(rlb->m, Match_MakePat(rlb->m, bytes(hdr), 3, ANCHOR_START), setHdr); 
    word nl[] = {PAT_SINGLE|PAT_TERM, '\r', '\r', PAT_SINGLE|PAT_TERM, '\n', '\n', PAT_END, 0, 0};
    prs->ko = Match_MakePat(rlb->m, bytes(nl), 2, ANCHOR_START);

    return prs;
}

static Parser *hdrNlParserMk(Roebling *rlb){
    word nl[] = {PAT_TERM, '\r', '\r', PAT_TERM, '\n', '\n', PAT_END, 0, 0};
    return Parser_MakeSingle(rlb->m, Match_MakePat(rlb->m, bytes(nl), 2, ANCHOR_START), NULL); 
}

static Parser *nlDblParserMk(Roebling *rlb){
    word nl[] = {PAT_TERM, '\r', '\r', PAT_TERM, '\n', '\n', PAT_END, 0, 0};
    Parser *prs = Parser_MakeSingle(rlb->m, Match_MakePat(rlb->m, bytes(nl), 2, ANCHOR_START), NULL); 
    word enumval = RBL_HEADERS;
    prs->jump = Span_GetIdx(rlb->marks, &enumval, Mark_Eq);
    return prs;
}

static Parser *headerMarkMk(Roebling *rbl){
    return (Parser *)Mark_Make(rbl->m, RBL_HEADERS);
}

Span *HeadersParser_Make(MemCtx *m, ProtoDef *def){
    Span *p =  Span_From(m, 6, 
        (Abstract *)headerMarkMk,
        (Abstract *)keyParserMk,
        (Abstract *)hdrSepParserMk,
        (Abstract *)hdrBodyParserMk,
        (Abstract *)hdrNlParserMk,
        (Abstract *)nlDblParserMk);
    return p;
}
