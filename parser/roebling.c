#include <external.h>
#include <caneka.h>

status Roebling_Run(Roebling *rbl){
    /*
    int i = 0;
    ParserMaker pmk = rbl->parsers[i];
    int mark = -1;
    boolean escaping = FALSE;
    Parser *prs;
    status r = READY;
    word escape_fl = (CYCLE_BREAK|COMPLETE);
    while(pmk != NULL){
        prs = pmk(rbl);
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
                        mark = i;
                    }else if(((r & CYCLE_LOOP) != 0) && mark != -1){
                        i = mark; 
                    }
                }else{
                    rbl->state = ERROR;
                    return rbl->state;
                }
            }
        }
        pmk = rbl->parsers[++i];
    }

    if(pmk == NULL){
        rbl->state = COMPLETE;
    }else{
        rbl->state = PROCESSING;
    }

    printf("State is %s\n", State_ToString(rbl->state));
    */
        
    return rbl->state;
}

Roebling *Roebling_Make(MemCtx *m, cls type, Span *parsers, String *s, Abstract *source){
    Roebling *rbl = (Roebling *)MemCtx_Alloc(m, sizeof(Roebling));
    rbl->type.of = TYPE_ROEBLING;
    rbl->m = m;
    rbl->parsers_pmk = parsers;
    rbl->source = source;
    Range_Set(&(rbl->range), s);
    return rbl;
}
