#include <external.h>
#include <filestore.h>

status StructExp_Run(StructExp *sexp){
    int i = 0;
    ParserMaker pmk = sexp->parsers[i];
    int mark = -1;
    boolean escaping = FALSE;
    Parser *prs;
    status r = READY;
    word escape_fl = (CYCLE_ESCAPE|COMPLETE);
    while(pmk != NULL){
        printf("Func %d\n", i);
        prs = pmk(sexp);
        if(prs->func == NULL){
            r = COMPLETE | prs->flags;
            printf("skip %d/%d\n", r, prs->flags);
        }else{
            r = prs->func(prs, &(sexp->range), sexp->source);
            printf("run %d %s\n", r, State_ToString(r));
        }

        if(escaping && ((r & CYCLE_LOOP) != 0)){
            escaping = FALSE;
        }else{
            if((r & escape_fl) == escape_fl){
                escaping = TRUE;
            }else{
                if((r & COMPLETE) != 0){
                    if((r & CYCLE_MARK) != 0){
                        printf(" -> SETTING MARK %d\n", i+1);
                        mark = i;
                    }else if(((r & CYCLE_LOOP) != 0) && mark != -1){
                        printf(" -> BACKT TO LOOP SETTING MARK %d\n", i+1);
                        i = mark; 
                    }else{
                        printf("NO LOOP OR MARK%d\n", i+1);
                    }
                }else{
                    sexp->state = ERROR;
                    return sexp->state;
                }
            }
        }
        pmk = parsers[++i];
    }
        
    sexp->state = PROCESSING;
    return sexp->state;
}

StructExp *StructExp_Make(MemCtx *m, cls type, ParserMake *parsers, String *s, void *source){
    StructExp *sexp = (StructExp *)MemCtx_Alloc(m, sizeof(StructExp));
    sexp->m = m;
    sexp->type = type;
    sexp->parsers = parsers;
    sexp->source = source;
    Range_Set(&(sexp->range), s);
    return sexp;
}
