#include <external.h>
#include <filestore.h>
#include "app.h"

static status Parser_MethodComplete(Parser *prs, Range *range, void *_req){
    Req *req = (Req *)_req;
    Match **matches = (Match **)prs->matches;
    req->method = matches[prs->idx]->intval;
    return SUCCESS;
}

Parser *Parser_Method(Serve *sctx, Req *req){
    int length = Array_Length((void **)sctx->methods);
    Match **matches = (Match **)Array_Make(req->m, length); 
    for(int i = 0; i < length; i++){
        String *s = String_From(req->m, sctx->methods[i]);
        matches[i] = Match_Make(req->m, s, ANCHOR_START, (int)*(sctx->method_vals[i]));
    }

    return Parser_MakeMulti(req->m, matches, Parser_MethodComplete);
}

Parser *Parser_Space(Serve *sctx, Req *req){
    return Parser_MakeSingle(req->m, Match_Make(req->m, space_tk, ANCHOR_START, 0), NULL);
}

static status Parser_PathComplete(Parser *prs, Range *range, void *_req){
    Req *req = (Req *)_req;
    req->path = String_FromRange(req->m, range);
    return SUCCESS;
}

Parser *Parser_Path(Serve *sctx, Req *req){
    return Parser_MakeSingle(req->m, Match_Make(req->m, space_tk, ANCHOR_UNTIL, 0), Parser_PathComplete);
}

Parser *Parser_HttpV(Serve *sctx, Req *req){
    return Parser_MakeSingle(req->m, 
        Match_MakePat(req->m, (byte *)HttpV_RangeDef, Match_PatLength(HttpV_RangeDef), ANCHOR_START, 0), NULL);
}

Parser *Parser_EndNl(Serve *sctx, Req *req){
    Match *mt = Match_MakePat(req->m, 
        (byte *)EndNl_RangeDef, Match_PatLength(EndNl_RangeDef), ANCHOR_START, 0);
    return Parser_MakeSingle(req->m, mt, NULL);
}

static status Parser_HComplete(Parser *prs, Range *range, void *_req){
    Req *req = (Req *)_req;
    String *s = String_FromRange(req->m, range);
    if(req->nextHeader != NULL){
        printf("Header: %s -> %s\n", req->nextHeader->bytes, (char *)s->bytes);
    }else{
        printf("Header: NULL -> %s\n", (char *)s->bytes);
    }
    return SUCCESS;
}

Parser *Parser_HEndNl(Serve *sctx, Req *req){
    Match *mt = Match_MakePat(req->m, 
        (byte *)EndNl_RangeDef, Match_PatLength(EndNl_RangeDef), ANCHOR_UNTIL, 0);
    Parser *prs =  Parser_MakeSingle(req->m, mt, Parser_HComplete);
    prs->flags |= CYCLE_LOOP;
    return prs;
}

Parser *Parser_HEndAllNl(Serve *sctx, Req *req){
    Match *mt = Match_MakePat(req->m, 
        (byte *)EndNl_RangeDef, Match_PatLength(EndNl_RangeDef), ANCHOR_START, 0);
    mt->flags |= CYCLE_ESCAPE;
    return Parser_MakeSingle(req->m, mt, NULL);
}

static status Parser_HKeyComplete(Parser *prs, Range *range, void *_req){
    Req *req = (Req *)_req;
    req->nextHeader = String_FromRange(req->m, range);
    return SUCCESS;
}

Parser *Parser_HColon(Serve *sctx, Req *req){
    return Parser_MakeSingle(req->m, Match_Make(req->m, String_Make(req->m, (byte *)":"), ANCHOR_UNTIL, 0), Parser_HKeyComplete);
}
