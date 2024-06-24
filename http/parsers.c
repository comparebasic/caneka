#include <external.h>
#include <filestore.h>
#include "app.h"

static status Parser_MethodComplete(Parser *prs, Range *range, void *_req){
    Req *req = (Req *)_req;
    Match **matches = (Match **)prs->matches;
    req->in.method = matches[prs->idx]->intval;
    return SUCCESS;
}

Parser *Parser_Method(StructExp *sexp){
    Req *req = (Req *)sexp->source;
    int length = Array_Length((void **)req->sctx->methods);
    Match **matches = (Match **)Array_Make(sexp->m, length); 
    for(int i = 0; i < length; i++){
        String *s = String_From(sexp->m, bytes(req->sctx->methods[i]));
        matches[i] = Match_Make(sexp->m, s, ANCHOR_START, (int)*(req->sctx->method_vals[i]));
    }

    return Parser_MakeMulti(sexp->m, matches, Parser_MethodComplete);
}

Parser *Parser_Space(StructExp *sexp){
    return Parser_MakeSingle(sexp->m, Match_Make(sexp->m, space_tk, ANCHOR_START, 0), NULL);
}

static status Parser_PathComplete(Parser *prs, Range *range, void *_req){
    Req *req = (Req *)_req;
    req->in.path = String_FromRange(req->m, range);
    return SUCCESS;
}

Parser *Parser_Path(StructExp *sexp){
    return Parser_MakeSingle(sexp->m, Match_Make(sexp->m, space_tk, ANCHOR_UNTIL, 0), Parser_PathComplete);
}

Parser *Parser_HttpV(StructExp *sexp){
    return Parser_MakeSingle(sexp->m, 
        Match_MakePat(sexp->m, (byte *)HttpV_RangeDef, Match_PatLength(HttpV_RangeDef), ANCHOR_START, 0), NULL);
}

Parser *Parser_EndNl(StructExp *sexp){
    Match *mt = Match_MakePat(sexp->m, 
        (byte *)EndNl_RangeDef, Match_PatLength(EndNl_RangeDef), ANCHOR_START, 0);
    return Parser_MakeSingle(sexp->m, mt, NULL);
}

static status Parser_HComplete(Parser *prs, Range *range, void *_req){
    Req *req = (Req *)_req;
    String *s = String_FromRange(req->m, range);
    if(req->in.nextHeader != NULL){
        printf("Header: %s -> %s\n", req->in.nextHeader->bytes, (char *)s->bytes);
    }else{
        printf("Header: NULL -> %s\n", (char *)s->bytes);
    }
    return SUCCESS;
}

Parser *Parser_HEndNl(StructExp *sexp){
    Match *mt = Match_MakePat(sexp->m, 
        (byte *)EndNl_RangeDef, Match_PatLength(EndNl_RangeDef), ANCHOR_UNTIL, 0);
    Parser *prs =  Parser_MakeSingle(sexp->m, mt, Parser_HComplete);
    prs->flags |= CYCLE_LOOP;
    return prs;
}

Parser *Parser_HEndAllNl(StructExp *sexp){
    Match *mt = Match_MakePat(sexp->m, 
        (byte *)EndNl_RangeDef, Match_PatLength(EndNl_RangeDef), ANCHOR_START, 0);
    mt->flags |= CYCLE_BREAK;
    return Parser_MakeSingle(sexp->m, mt, NULL);
}

static status Parser_HKeyComplete(Parser *prs, Range *range, void *_req){
    Req *req = (Req *)_req;
    req->in.nextHeader = String_FromRange(req->m, range);
    return SUCCESS;
}

Parser *Parser_HColon(StructExp *sexp){
    return Parser_MakeSingle(sexp->m, Match_Make(sexp->m, String_Make(sexp->m, (byte *)":"), ANCHOR_UNTIL, 0), Parser_HKeyComplete);
}
