#include "external.h"
#include "filestore.h"

static status parse_Multi(Parser *prs, Range *range, void *source){
    status r = READY;
    int i = 0;
    int start = range->start.position;
    Match **matches = (Match **)prs->matches;
    while(matches[i] != NULL){
        r = SCursor_Find(range, matches[i]);
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
