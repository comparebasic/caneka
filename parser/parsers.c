#include <external.h>
#include <caneka.h>

#include <proto/http.h>

static status parse_Multi(Parser *prs, Range *range, void *source){
    status r = READY;
    int i = 0;
    int start = range->start.position;
    Match **matches = prs->match.array;
    while(matches[i] != NULL){
        r = SCursor_Find(range, matches[i], prs->ko);
        if(DEBUG_PARSER){
            Debug_Print((void *)matches[i], matches[i]->type.of, "parse_Multi Match: ", DEBUG_PARSER, TRUE);
        }
        if(r == COMPLETE){
            if(DEBUG_ROEBLING_COMPLETE){
                Debug_Print((void *)prs, 0, "parse_Multi COMPLETE: ", DEBUG_ROEBLING_COMPLETE, TRUE);
                printf("\n");
            }
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
    if(mt->state == MISS){
        return mt->state;
    }

    r = SCursor_Find(range, mt, prs->ko);
    if(DEBUG_PARSER){
        Debug_Print((void *)mt, mt->type.of, "parse_Single Match: ", DEBUG_PARSER, TRUE);
        printf("\n");
    }
    if(r == COMPLETE){
        if(DEBUG_ROEBLING_COMPLETE){
            Debug_Print((void *)prs, 0, "parse_Multi COMPLETE: ", DEBUG_ROEBLING_COMPLETE, TRUE);
            printf("\n");
        }
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
    prs->jump = -1;
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

Parser *Parser_StringSet(MemCtx *m, word flags, ParseFunc complete, ...){
	va_list args;
	va_list args_copy;
    va_start(args, complete);
    va_copy(args_copy, args);

    int l = 0;
    String *s = va_arg(args, String*);
    while(s != NULL){
        l++; 
        String *s = va_arg(args, String*);
    }
    Array arr = Array_Make(m, l);
    for(int i = 0; i < l; i++){
        s = va_arg(args_copy, String*);
        arr[i] = (Abstract *)Match_Make(m, s, flags);
    }
    
    return Parser_MakeMulti(m, (Match **)arr, complete);
}

Parser *Parser_StringLookup(MemCtx *m, word flags, ParseFunc complete, Lookup *lk){
    Array arr = Array_Make(m, lk->values->nvalues);
    for(int i = 0; i < lk->values->nvalues; i++){
        String *s = (String *)Span_Get(lk->values, i);
        Match *mt = Match_Make(m, s, flags);
        arr[i] = (Abstract *)mt;
    }
    
    return Parser_MakeMulti(m, (Match **)arr, complete);
}

Parser *Parser_String(MemCtx *m, word flags, ParseFunc complete, byte *b){
    return Parser_MakeSingle(m, Match_Make(m, String_From(m, b), flags), complete); 
}
