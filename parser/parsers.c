#include <external.h>
#include <caneka.h>

status Parser_Run(Parser *prs, Roebling *rbl){
    status r = READY;
    status ko_r = READY;
    int i = 0;
    while(i < rbl->matches.values->nvalues){
        Match *mt = Span_Get(rbl->matches.values, i);
        r = SCursor_Find(&(rbl->range), mt);
        if((r & COMPLETE) != 0){
            rbl->matches.values->metrics.selected = i;
            if(prs->complete != NULL){
                prs->complete(prs, &(rbl->range), rbl->source);
            }
            if(mt->jump >= 0){
                prs->jump = mt->jump;
            }
            break;
        }
        i++;
    }

    i = 0;
    while(i < rbl->matches.ko->nvalues){
        Match *mt = Span_Get(rbl->matches.ko, i);
        ko_r = SCursor_Find(&(rbl->range), mt);
        if((ko_r & COMPLETE) != 0){
            r = (r & ~COMPLETE) | ko_r;
            break;
        }
        i++;
    }

    prs->type.state = r;
    return prs->type.state;
}

