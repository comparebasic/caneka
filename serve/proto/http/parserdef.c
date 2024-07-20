#include <external.h>
#include <caneka.h>
#include <proto/http.h>

/* setters */
static status setMethod(Parser *prs, Range *range, void *source){
    Req *req = (Req *)as(source, TYPE_HTTP_REQ);
    HttpProto *proto = (HttpProto*)as(req->proto, TYPE_HTTP_PROTO);
    proto->method = Lookup_AbsFromIdx(req->sctx->def->methods, prs->idx);
    return SUCCESS;
}

static status setPath(Parser *prs, Range *range, void *source){
    Req *req = (Req *)as(source, TYPE_HTTP_REQ);
    HttpProto *proto = (HttpProto*)as(req->proto, TYPE_HTTP_PROTO);
    proto->path = Range_Copy(req->m, range);
    Debug_Print((void *)proto->path, 0, "Setting path: ", COLOR_PURPLE, TRUE);
    return SUCCESS;
}

/* parser makers */
static Parser *methodParserMk(Roebling *rlb){
    Req *req = (Req *) as(rlb->source, TYPE_HTTP_REQ);
    return Parser_StringLookup(req->m, ANCHOR_START, setMethod, req->sctx->def->methods);
}

static Parser *spaceParserMk(Roebling *rlb){
    word pat[] = {PAT_TERM, ' ', ' ', PAT_END};
    return Parser_MakeSingle(rlb->m, Match_MakePat(rlb->m, bytes(pat), 1, ANCHOR_START), NULL); 
}

static Parser *pathParserMk(Roebling *rlb){
    word path[] = {PAT_INVERT|PAT_TERM, ' ', ' ', PAT_MANY|PAT_TERM, '*', '~', PAT_END};
    return Parser_MakeSingle(rlb->m, Match_MakePat(rlb->m, bytes(path), 2, ANCHOR_START), setPath); 
}

static Parser *httvParserMk(Roebling *rlb){
    word httpv[] = {PAT_TERM, 'H', 'H', PAT_COUNT|PAT_TERM, 'T', 2, PAT_TERM, 'P', 'P',
        PAT_TERM, '/', '/', PAT_TERM, '1', '1', PAT_ANY|PAT_TERM, '.', '.', 
        PAT_COUNT, '1', 1, PAT_COUNT, '2', 1, PAT_END}; 
    return Parser_MakeSingle(rlb->m, Match_MakePat(rlb->m, bytes(httpv), 8, ANCHOR_START), NULL); 
}

static Parser *nlParserMk(Roebling *rlb){
    word nl[] = {PAT_TERM, '\r', '\r', PAT_TERM, '\n', '\n', PAT_INVERT|PAT_TERM, '\t', '\t', PAT_INVERT|PAT_TERM, ' ', ' ', PAT_END};
    return Parser_MakeSingle(rlb->m, Match_MakePat(rlb->m, bytes(nl), 4, ANCHOR_START), NULL); 
}

/* public */
Span *HttpParser_Make(MemCtx *m, ProtoDef *def){
    printf("ADDR of method %p\n", methodParserMk);
    return Span_From(m, 5, (Abstract *)methodParserMk, (Abstract *)spaceParserMk, (Abstract *)pathParserMk, (Abstract *)spaceParserMk, (Abstract *)nlParserMk);
}
