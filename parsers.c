#include "external.h"
#include "filestore.h"

static status parse_Multi(Parser *prs, Range *range, void *source){
    status r = READY;
    int i = 0;
    int start = range->start.position;
    Match **matches = (Match **)prs->matches;
    while(matches[i] != NULL){
        r = SCursor_Find(range, matches[i]);
        Debug_Print((void *)matches[i], TYPE_MATCH, "After ", COLOR_YELLOW, TRUE);
        if(r == COMPLETE){
            prs->idx = i;
            if(prs->complete != NULL){
                prs->complete(prs, range, source);
            }
            return SUCCESS;
        }
        i++;
    }
    return ERROR;
}

static status parse_Single(Parser *prs, Range *range, void *source){
    status r = READY;
    Match *mt = (Match *)prs->matches;
    r = SCursor_Find(range, mt);
    if(r == COMPLETE){
        if(prs->complete != NULL){
            prs->complete(prs, range, source);
        }
        return SUCCESS;
    }
    return ERROR;
}

Parser *Parser_Make(MemCtx *m, cls type){
    Parser *prs = (Parser *) MemCtx_Alloc(m, sizeof(Parser));
    prs->type = type;
    return prs;
}

Parser *Parser_MakeSingle(MemCtx *m, Match *mt, ParseFunc complete){
    Parser *prs = Parser_Make(m, TYPE_PARSER);
    prs->type = TYPE_PARSER;
    prs->matches = (void *)mt;
    prs->func = parse_Single;
    prs->complete = complete;
    return prs;
}

Parser *Parser_MakeMulti(MemCtx *m, Match **mt_arr, ParseFunc complete){
    Parser *prs = Parser_Make(m, TYPE_MULTIPARSER);
    prs->type = TYPE_MULTIPARSER;
    prs->matches = (void *)mt_arr;
    prs->func = parse_Multi;
    prs->complete = complete;
    return prs;
}

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
    printf("PATH COMPLETE CALLED: %s\n", req->path->bytes);
    return SUCCESS;
}

Parser *Parser_Path(Serve *sctx, Req *req){
    return Parser_MakeSingle(req->m, Match_Make(req->m, space_tk, ANCHOR_CONTAINS, 0), Parser_PathComplete);
}

status Parse_HttpV(Req *req, Range *range){
    return ERROR;
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
