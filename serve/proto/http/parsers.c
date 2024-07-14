#include <external.h>
#include <caneka.h>
#include <proto/http.h>

static status Parser_MethodComplete(Parser *prs, Range *range, void *_req){
    Req *req = (Req *)_req;
    Match **matches = prs->match.array;
    HttpProto *proto = (HttpProto *)req->proto;
    proto->method = matches[prs->idx]->intval;
    return SUCCESS;
}

Parser *Parser_Method(Roebling *rbl){
    Req *req = (Req *)rbl->source;
    int length = req->sctx->proto->methods->values->max_idx;
    Match **matches = (Match **)Array_Make(rbl->m, length); 
    Iter *it = Iter_Make(rbl->m, req->sctx->proto->methods->values);
    while(Iter_Next(it) == SUCCESS){
        String *s = (String *)Iter_Get(it);
        matches[it->idx] = Match_Make(rbl->m, s, ANCHOR_START, it->idx);
    }

    return Parser_MakeMulti(rbl->m, matches, Parser_MethodComplete);
}

Parser *Parser_Space(Roebling *rbl){
    return Parser_MakeSingle(rbl->m, Match_Make(rbl->m, String_From(rbl->m, bytes(" ")), ANCHOR_START, 0), NULL);
}

static status Parser_PathComplete(Parser *prs, Range *range, void *_req){
    Req *req = (Req *)_req;
    HttpProto *proto = (HttpProto *)req->proto;
    proto->path = String_FromRange(req->m, range);
    return SUCCESS;
}

Parser *Parser_Path(Roebling *rbl){
    return Parser_MakeSingle(rbl->m, Match_Make(rbl->m, String_From(rbl->m, bytes(" ")), ANCHOR_UNTIL, 0), Parser_PathComplete);
}

Parser *Parser_HttpV(Roebling *rbl){
    return Parser_MakeSingle(rbl->m, 
        Match_MakePat(rbl->m, (byte *)HttpV_RangeDef, Match_PatLength(HttpV_RangeDef), ANCHOR_START, 0), NULL);
}

Parser *Parser_EndNl(Roebling *rbl){
    Match *mt = Match_MakePat(rbl->m, 
        (byte *)EndNl_RangeDef, Match_PatLength(EndNl_RangeDef), ANCHOR_START, 0);
    return Parser_MakeSingle(rbl->m, mt, NULL);
}

static status Parser_HComplete(Parser *prs, Range *range, void *_req){
    Req *req = (Req *)_req;
    String *s = String_FromRange(req->m, range);
    HttpProto *proto = (HttpProto *)req->proto;
    if(proto->nextHeader != NULL){
        printf("Header: %s -> %s\n", proto->nextHeader->bytes, (char *)s->bytes);
    }else{
        printf("Header: NULL -> %s\n", (char *)s->bytes);
    }
    return SUCCESS;
}

Parser *Parser_HEndNl(Roebling *rbl){
    Match *mt = Match_MakePat(rbl->m, 
        (byte *)EndNl_RangeDef, Match_PatLength(EndNl_RangeDef), ANCHOR_UNTIL, 0);
    Parser *prs =  Parser_MakeSingle(rbl->m, mt, Parser_HComplete);
    prs->flags |= CYCLE_LOOP;
    return prs;
}

Parser *Parser_HEndAllNl(Roebling *rbl){
    Match *mt = Match_MakePat(rbl->m, 
        (byte *)EndNl_RangeDef, Match_PatLength(EndNl_RangeDef), ANCHOR_START, 0);
    mt->flags |= CYCLE_BREAK;
    return Parser_MakeSingle(rbl->m, mt, NULL);
}

static status Parser_HKeyComplete(Parser *prs, Range *range, void *_req){
    Req *req = (Req *)_req;
    HttpProto *proto = (HttpProto *)req->proto;
    proto->nextHeader = String_FromRange(req->m, range);
    return SUCCESS;
}

Parser *Parser_HColon(Roebling *rbl){
    return Parser_MakeSingle(rbl->m, Match_Make(rbl->m, String_Make(rbl->m, (byte *)":"), ANCHOR_UNTIL, 0), Parser_HKeyComplete);
}
