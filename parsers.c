#include "external.h"
#include "filestore.h"

#include <app.h>

static status parse_Multi(Parser *prs, Range *range, void *source){
    status r = READY;
    int i = 0;
    int start = range->start.position;
    Match **matches = (Match **)prs->matches;
    while(matches[i] != NULL){
        r = SCursor_Find(range, matches[i]);
        if(DEBUG_MATCH){
            Debug_Print((void *)matches[i], matches[i]->type, "parse_Multi Match: ", DEBUG_MATCH, TRUE);
        }
        if(r == COMPLETE){
            prs->idx = i;
            if(prs->complete != NULL){
                prs->complete(prs, range, source);
            }
            return SUCCESS | prs->flags;
        }
        i++;
    }
    return ERROR | prs->flags;
}

static status parse_Single(Parser *prs, Range *range, void *source){
    status r = READY;
    Match *mt = (Match *)prs->matches;


    r = SCursor_Find(range, mt);
    if(DEBUG_MATCH){
        Debug_Print((void *)mt, mt->type, "parse_Single Match: ", DEBUG_MATCH, TRUE);
    }
    if(r == COMPLETE){
        if(prs->complete != NULL){
            prs->complete(prs, range, source);
        }
        return SUCCESS | prs->flags;
    }
    return ERROR | prs->flags;
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

Parser *Parser_Mark(Serve *sctx, Req *req){
    Parser *prs = Parser_Make(req->m, TYPE_PARSER);
    prs->type = TYPE_PARSER;
    prs->flags |= CYCLE_MARK;
    return prs;
}

Parser *Parser_Loop(Serve *sctx, Req *req){
    Parser *prs = Parser_Make(req->m, TYPE_PARSER);
    prs->type = TYPE_PARSER;
    prs->flags |= CYCLE_LOOP;
    return prs;
}

Parser *Parser_Escape(Serve *sctx, Req *req){
    Parser *prs = Parser_Make(req->m, TYPE_PARSER);
    prs->type = TYPE_PARSER;
    prs->flags |= CYCLE_ESCAPE;
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
