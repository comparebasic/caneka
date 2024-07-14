#include <external.h>
#include <caneka.h>

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
        prs = pmk(rbl);
        if(DEBUG_ROEBLING){
            Debug_Print((void *)prs, 0, "Parser in run: ", DEBUG_ROEBLING, TRUE);
            printf("\x1b[0m\n");
        }
        if(prs->func == NULL){
            r = COMPLETE | prs->flags;
        }else{
            r = prs->func(prs, &(rbl->range), rbl->source);
        }

        if(escaping && ((r & CYCLE_LOOP) != 0)){
            escaping = FALSE;
        }else{
            if((r & escape_fl) == escape_fl){
                escaping = TRUE;
            }else{
                if((r & COMPLETE) != 0){
                    if((r & CYCLE_MARK) != 0){
                        rbl->mark = rbl->idx;
                    }else if(((r & CYCLE_LOOP) != 0) && rbl->mark != -1){
                        rbl->idx = rbl->mark; 
                    }
                }else{
                    rbl->state = ERROR;
                    if(DEBUG_ROEBLING){
                        Debug_Print((void *)rbl, 0, "Roebling_Run ERROR: ", DEBUG_ROEBLING, TRUE);
                        printf("\n");
                    }
                    return rbl->state;
                }
            }
        }
        rbl->idx++;
        pmk = Span_Get(rbl->parsers_pmk, rbl->idx);
    }

    if(pmk == NULL){
        rbl->state = COMPLETE;
    }else{
        rbl->state = PROCESSING;
    }

    if(DEBUG_ROEBLING){
        Debug_Print((void *)rbl, 0, "Roebling_Run END for: ", DEBUG_ROEBLING, TRUE);
        printf("\n");
    }
    if(DEBUG_ROEBLING){
        Debug_Print((void *)prs, 0, "Parser END of run: ", DEBUG_ROEBLING, TRUE);
        printf("\x1b[0m\n");
    }
        
    return rbl->state;
}

Roebling *Roebling_Make(MemCtx *m, cls type, Span *parsers, String *s, Abstract *source){
    Roebling *rbl = (Roebling *)MemCtx_Alloc(m, sizeof(Roebling));
    rbl->type.of = TYPE_ROEBLING;
    rbl->m = m;
    rbl->parsers_pmk = parsers;
    rbl->source = source;
    rbl->mark = -1;
    Range_Set(&(rbl->range), s);
    return rbl;
}
