#include <external.h>
#include <caneka.h>

Match *Roebling_GetMatch(Roebling *rbl){
    return Span_GetSelected(rbl->matches.values);
}

status Roebling_SetPattern(Roebling *rbl, PatCharDef *def){
    Match *mt = Span_ReserveNext(rbl->matches.values);
    return Match_SetPattern(mt, def);
}

status Roebling_SetKOPattern(Roebling *rbl, PatCharDef *def){
    Match *mt = Span_ReserveNext(rbl->matches.ko);
    return Match_SetPattern(mt, def);
}

status Roebling_SetLookup(Roebling *rbl, Lookup *lk){
    for(int i = 0; i < lk->values->nvalues; i++){
        Match *mt = Span_ReserveNext(rbl->matches.values);
        String *s = (String *)Span_Get(lk->values, i);
        Match_SetString(mt, s);
    }
    return SUCCESS;
}

status Roebling_Prepare(Roebling *rbl){
    rbl->idx = 0;
    Abstract *dof = Span_Get(rbl->parsers_do, rbl->idx);
    while(dof != NULL){
        if(dof->type.of == TYPE_WRAPPED_UTIL){
            int mark = ((Single *)dof)->val.value;
            Roebling_SetMark(rbl, mark);
        }
        rbl->idx++;
        dof = Span_Get(rbl->parsers_do, rbl->idx);
    }
    rbl->idx = 0;
    return SUCCESS;
}

status Roebling_Run(Roebling *rbl){
    if(DEBUG_ROEBLING){
        printf("\x1b[%dmRbl Run idx:\x1b[1;%dm%d\x1b[0;%dm ", DEBUG_ROEBLING, DEBUG_ROEBLING, rbl->idx, DEBUG_ROEBLING);
        Debug_Print((void *)rbl, 0, "", DEBUG_ROEBLING, TRUE);
        printf("\n");
    }

    Single *wdof = Span_Get(rbl->parsers_do, rbl->idx);
    if(wdof == NULL){
        rbl->type.state = COMPLETE;
    }else{
        wdof = as(wdof, TYPE_WRAPPED_DO);
        wdof->val.dof((MemHandle *)rbl);
        int i = 0;
        for(int i = 0; i < rbl->matches.ko->nvalues; i++){
            Match *mt = Span_Get(rbl->matches.ko, i);
            if(mt != NULL){
               SCursor_Find(&(rbl->range), mt); 
               if(HasFlag(mt->type.state, COMPLETE)){
                     rbl->matches.ko->metrics.selected = i;
                     rbl->type.state = NEXT|KO;
                     break;
               }
            }
        }
        for(int i = 0; i < rbl->matches.values->nvalues; i++){
            Match *mt = Span_Get(rbl->matches.values, i);
            if(mt != NULL){
               SCursor_Find(&(rbl->range), mt); 
               if(HasFlag(mt->type.state, COMPLETE)){
                     rbl->matches.values->metrics.selected = i;
                     rbl->type.state = NEXT;
                     break;
               }
            }
        }
    }

    if(HasFlag(rbl->type.state, NEXT)){
        if(rbl->dispatch != NULL){
            Match *mt = (HasFlag(rbl->type.state, KO) ? 
                 Span_GetSelected(rbl->matches.ko) :
                 Span_GetSelected(rbl->matches.values));

            rbl->dispatch(rbl, mt);
        }
    }


    /*
    Abstract *pmk = Span_Get(rbl->parsers_pmk, rbl->idx);
    boolean escaping = FALSE;
    Parser *prs;
    status r = READY;
    word escape_fl = (CYCLE_BREAK|COMPLETE);
    while(pmk != NULL){
        if(DEBUG_ROEBLING_NAME){
            printf("\x1b[%dmRbl ParserName %d %p\x1b[0m\n", DEBUG_ROEBLING_NAME,
                rbl->idx, pmk);
        }
        rbl->type.state = PROCESSING;

        if(pmk->type.of == TYPE_WRAPPED_UTIL){
            rbl->idx++;
            pmk = Span_Get(rbl->parsers_pmk, rbl->idx);
            continue;
        }else if(pmk->type.of == TYPE_WRAPPED_PTR){
            Single *_sgl = (Single *)pmk;
            ParserMaker p = (ParserMaker)_sgl->val.ptr;
            prs = p(rbl);
            if(prs == NULL){
                if(DEBUG_ROEBLING_COMPLETE){
                    Debug_Print((void *)prs, 0, "Finish in Error (prs is NULL): ", DEBUG_ROEBLING_COMPLETE, TRUE);
                    printf("\n");
                }
                rbl->type.state = ERROR; 
                return rbl->type.state;
            }else{
                if(DEBUG_ROEBLING_CURRENT){
                    Debug_Print((void *)prs, 0, "Current Parser: ", DEBUG_ROEBLING_CURRENT, TRUE);
                    printf("\n");
                }
            }
        }else{
            rbl->type.state = ERROR;
            return rbl->type.state;
        }

        if(DEBUG_ROEBLING){
            printf("Roebling idx %d\n", rbl->idx);
            Debug_Print((void *)prs, 0, "Parser in run: ", DEBUG_ROEBLING, TRUE);
            printf("\x1b[0m\n");
        }
        r = Parser_Run(prs, rbl);
        if((r & COMPLETE) != COMPLETE){
            if(prs->failJump != -1){
                rbl->idx = prs->failJump;
                pmk = Span_Get(rbl->parsers_pmk, rbl->idx);
                if(DEBUG_ROEBLING_MARK){
                    printf("Jumping %d\n", rbl->idx);
                    Debug_Print((void *)pmk, 0, "Mark failJump: ", DEBUG_ROEBLING_MARK, TRUE);
                    printf("\n");
                }
                continue;
            }

            if(DEBUG_ROEBLING_COMPLETE){
                Debug_Print((void *)prs, 0, "Finish in Error (prs->func not COMPLETE): ", DEBUG_ROEBLING_COMPLETE, TRUE);
                printf("\n");
            }

            rbl->type.state = ERROR;
            if(DEBUG_ROEBLING){
                Debug_Print((void *)rbl, 0, "Roebling_Run ERROR: ", DEBUG_ROEBLING, TRUE);
                printf("\n");
            }
            return rbl->type.state;
        }

        if(rbl->idx == rbl->parsers_pmk->max_idx){
            rbl->type.state = COMPLETE;
            break;
        }else{
            if(prs->jump > 0){
                rbl->idx = prs->jump;
                printf("\x1b[%dm%d\n", COLOR_RED, prs->jump);
                Debug_Print((void *)prs, 0, "Jump", COLOR_RED, TRUE);
                printf("\n");
            }else{
                rbl->idx++;
            }
            pmk = Span_Get(rbl->parsers_pmk, rbl->idx);
            Range_Incr(&(rbl->range));
        }
    }

    if(DEBUG_ROEBLING){
        Debug_Print((void *)rbl, 0, "Roebling_Run END for: ", DEBUG_ROEBLING, TRUE);
        printf("\n");
    }
    */
        
    return rbl->type.state;
}

int Roebling_GetMarkIdx(Roebling *rlb, int mark){
    Single *mrk = (Single *)Lookup_Get(rlb->gotos, mark); 
    if(mrk != NULL){
        return mrk->val.value;
    }
    return -1; 
}

status Roebling_ResetPatterns(Roebling *rbl){
    Span_ReInit(rbl->matches.values);
    Span_ReInit(rbl->matches.ko);
    return READY;
}

status Roebling_SetMark(Roebling *rbl, int mark){
    Single *sgl = Int_Wrapped(rbl->m, rbl->idx);
    return Lookup_Add(rbl->m, rbl->gotos, mark, (void *)sgl);
}

status Roebling_AddBytes(Roebling *rbl, byte bytes[], int length){
    return String_AddBytes(rbl->m, rbl->range.search, bytes, length);
}

Roebling *Roebling_Make(MemCtx *m, cls type, Span *parsers, String *s, Abstract *source){
    Roebling *rbl = (Roebling *)MemCtx_Alloc(m, sizeof(Roebling));
    rbl->type.of = TYPE_ROEBLING;
    rbl->m = m;
    rbl->parsers_do = parsers;
    rbl->source = source;
    rbl->gotos = Lookup_Make(m, XML_START, NULL, (Abstract *)rbl);
    rbl->matches.values = Span_MakeInline(rbl->m, TYPE_MATCH, (int)sizeof(Match));  
    rbl->matches.ko = Span_MakeInline(rbl->m, TYPE_MATCH, (int)sizeof(Match));  
    Range_Set(&(rbl->range), s);

    Roebling_Prepare(rbl);
    Debug_Print((void *)rbl->gotos->values, 0, "Marks: ", COLOR_RED, TRUE);

    return rbl;
}
