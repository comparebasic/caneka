#include <external.h>
#include <caneka.h>

status Roebling_SetJump(Roebling *rbl, Parser *prs, word mark){
    int jump = Span_GetIdx(rbl->marks, &mark, Mark_Eq);
    if(jump >= 0){
        prs->jump = jump;
        return SUCCESS;
    }
    return ERROR;
}

status Roebling_Run(Roebling *rbl){
    if(DEBUG_ROEBLING){
        Debug_Print((void *)rbl, 0, "Roebling_Run for: ", DEBUG_ROEBLING, TRUE);
        printf("\n");
    }
    ParserMaker pmk = Span_Get(rbl->parsers_pmk, rbl->idx);
    boolean escaping = FALSE;
    Parser *prs;
    status r = READY;
    word escape_fl = (CYCLE_BREAK|COMPLETE);
    while(pmk != NULL){
        rbl->type.state = PROCESSING;
        prs = pmk(rbl);
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
        if(prs->type.of == TYPE_RBL_MARK){
            Single *mrk = (Single *)prs;
            mrk->type.state = rbl->idx;
            Span_Add(rbl->marks, (Abstract *)prs);
            rbl->idx++;
            pmk = Span_Get(rbl->parsers_pmk, rbl->idx);
            continue;
        }
        if(DEBUG_ROEBLING){
            Debug_Print((void *)prs, 0, "Parser in run: ", DEBUG_ROEBLING, TRUE);
            printf("\x1b[0m\n");
        }
        r = prs->func(prs, &(rbl->range), rbl->source);
        if((r & COMPLETE) != COMPLETE){
            if(prs->jump != -1){
                Single *mrk = (Single *)Span_Get(rbl->marks, prs->jump);
                rbl->idx = mrk->type.state;
                pmk = Span_Get(rbl->parsers_pmk, rbl->idx);
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
        
    return rbl->type.state;
}

int Roebling_GetMarkIdx(Roebling *rlb, int mark){
    return Span_GetIdx(rlb->marks, &enumval, Mark_Eq); 
}

Roebling *Roebling_Make(MemCtx *m, cls type, Span *parsers, String *s, Abstract *source){
    Roebling *rbl = (Roebling *)MemCtx_Alloc(m, sizeof(Roebling));
    rbl->type.of = TYPE_ROEBLING;
    rbl->m = m;
    rbl->parsers_pmk = parsers;
    rbl->source = source;
    rbl->marks = Span_Make(m);
    rbl->current = -1;
    Range_Set(&(rbl->range), s);
    return rbl;
}
