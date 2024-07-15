#include <external.h>
#include <caneka.h>

#include <proto/http.h>

static status parse_Multi(Parser *prs, Range *range, void *source){
    status r = READY;
    int i = 0;
    int start = range->start.position;
    Match **matches = prs->match.array;
    while(matches[i] != NULL){
        r = SCursor_Find(range, matches[i]);
        if(DEBUG_MATCH){
            Debug_Print((void *)matches[i], matches[i]->type.of, "parse_Multi Match: ", DEBUG_MATCH, TRUE);
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
    Match *mt = prs->match.single;

    r = SCursor_Find(range, mt);
    if(DEBUG_MATCH){
        Debug_Print((void *)mt, mt->type.of, "parse_Single Match: ", DEBUG_MATCH, TRUE);
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
    prs->type.of = type;
    return prs;
}

Parser *Parser_MakeSingle(MemCtx *m, Match *mt, ParseFunc complete){
    Parser *prs = Parser_Make(m, TYPE_PARSER);
    prs->type.of = TYPE_PARSER;
    prs->match.single = mt;
    prs->func = parse_Single;
    prs->complete = complete;
    return prs;
}

Parser *Parser_MakeMulti(MemCtx *m, Match **mt_arr, ParseFunc complete){
    Parser *prs = Parser_Make(m, TYPE_MULTIPARSER);
    prs->type.of = TYPE_MULTIPARSER;
    prs->match.array = mt_arr;
    prs->func = parse_Multi;
    prs->complete = complete;
    return prs;
}

Parser *Parser_Mark(Roebling *sexp){
    Parser *prs = Parser_Make(sexp->m, TYPE_PARSER);
    prs->type.of = TYPE_PARSER;
    prs->flags |= CYCLE_MARK;
    return prs;
}

Parser *Parser_Loop(Roebling *sexp){
    Parser *prs = Parser_Make(sexp->m, TYPE_PARSER);
    prs->type.of = TYPE_PARSER;
    prs->flags |= CYCLE_LOOP;
    return prs;
}

Parser *Parser_Break(Roebling *sexp){
    Parser *prs = Parser_Make(sexp->m, TYPE_PARSER);
    prs->type.of = TYPE_PARSER;
    prs->flags |= CYCLE_BREAK;
    return prs;
}

Abstract *Parser_GetMatchKey(Parser *prs){
    Match *mt = NULL;
    if(prs->type.of == TYPE_MULTIPARSER){
       mt = prs->match.array[prs->idx]; 
    }else{
       mt = prs->match.single; 
    }
    if(mt->type.of == TYPE_STRINGMATCH){
        return (Abstract *)mt->s;
    }
    return NULL;
}

Abstract *Parser_GetContent(Parser *prs, Range *range, void *source){
    return NULL;
}
