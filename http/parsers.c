#include "external.h"
#include "filestore.h"
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

Parser *Parse_HttpV(Serve *sctx, Req *req){
    return Parser_MakeSingle(req->m, Match_MakePat(req->m, (byte *)HttpV_RangeDef, 7, ANCHOR_START, 0), NULL);
}

status Parse_Colon(Req *req, Range *range){
    return ERROR;
}

status Parse_LineEnd(Req *req, Range *range){
    return ERROR;
}

status Parse_DoubleLineEnd(Req *req, Range *range){
    return ERROR;
}

status Parse_Json(Req *req, Range *range){
    return ERROR;
}
