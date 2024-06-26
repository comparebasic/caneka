#include <external.h>
#include <filestore.h>

RlbLookup RlbLookup_Make(MemCtx *m, word offset, RlbLookupPopulate populate, Virtual *arg){
    RlbLookup *lk = (RlbLookup *)MemCtx_Alloc(m, sizeof(RlbLookup));
    lk->offset = offset;
    lk->values = Span_Make(m);
    lk->arg = arg;
    populate(m, lk);

    return lk;
}

status Roebling_Run(Roebling *sexp){
    int i = 0;
    ParserMaker pmk = sexp->parsers[i];
    int mark = -1;
    boolean escaping = FALSE;
    Parser *prs;
    status r = READY;
    word escape_fl = (CYCLE_BREAK|COMPLETE);
    while(pmk != NULL){
        prs = pmk(sexp);
        if(prs->func == NULL){
            r = COMPLETE | prs->flags;
        }else{
            r = prs->func(prs, &(sexp->range), sexp->source);
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
                    sexp->state = ERROR;
                    return sexp->state;
                }
            }
        }
        pmk = sexp->parsers[++i];
    }

    if(pmk == NULL){
        sexp->state = COMPLETE;
    }else{
        sexp->state = PROCESSING;
    }

    printf("State is %s\n", State_ToString(sexp->state));
        
    return sexp->state;
}

Roebling *Roebling_Make(MemCtx *m, cls type, ParserMaker *parsers, String *s, void *source){
    Roebling *sexp = (Roebling *)MemCtx_Alloc(m, sizeof(Roebling));
    sexp->m = m;
    sexp->type = type;
    sexp->parsers = parsers;
    sexp->source = source;
    Range_Set(&(sexp->range), s);
    return sexp;
}
