#include "external.h"
#include "filestore.h"

static status parse_Multi(Parser *prs, Req *req, Range *range){
    int i = 0;
    int start = range->start.position;
    Match **matches = (Match **)prs->matches;
    Match *mt = matches[0];
    while(mt != NULL){
        if(SCursor_Find(range, mt) == COMPLETE){
            req->method = mt->intval;
            return SUCCESS;
        }
        mt++;
    }
    return ERROR;
}

static status parse_Single(Parser *prs, Req *req, Range *range){
    Match *mt = (Match *)prs->matches;
    if(SCursor_Find(range, mt) == COMPLETE){
        return SUCCESS;
    }
    return ERROR;
}

Parser *Parser_Make(MemCtx *m, cls type){
    Parser *prs = (Parser *) MemCtx_Alloc(m, sizeof(Parser));
    prs->type = type;
    return prs;
}

Parser *Parser_MakeSingle(MemCtx *m, Match *mt){
    Parser *prs = Parser_Make(m, TYPE_PARSER);
    prs->type = TYPE_PARSER;
    prs->matches = (void *)mt;
    prs->func = parse_Single;
    return prs;
}

Parser *Parser_MakeMulti(MemCtx *m, Match **mt_arr){
    printf("Parsing multi\n");
    fflush(stdout);
    Parser *prs = Parser_Make(m, TYPE_MULTIPARSER);
    prs->type = TYPE_MULTIPARSER;
    prs->matches = (void *)mt_arr;
    prs->func = parse_Multi;
    return prs;
}

Parser *Parser_Method(Serve *sctx, Req *req){
    printf("Made method\n");
    int length = Array_Length((void **)sctx->methods);
    printf("methods length is %d\n", length);
    Match **matches = (Match **)Array_Make(req->m, length); 
    for(int i = 0; i < length; i++){
        printf("Making match %i\n", i);
        matches[i] = Match_Make(req->m, String_From(req->m, sctx->methods[i]), ANCHOR_START);
    }
    printf("after Making match\n");

    return Parser_MakeMulti(req->m, matches);
}

Parser *Parser_Space(Serve *sctx, Req *req){
    return Parser_MakeSingle(req->m, Match_Make(req->m, space_tk, ANCHOR_START));
}

Parser *Parser_Path(Serve *sctx, Req *req){
    return Parser_MakeSingle(req->m, Match_Make(req->m, space_tk, ANCHOR_CONTAINS));
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
