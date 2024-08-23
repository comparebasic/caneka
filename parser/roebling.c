#include <external.h>
#include <caneka.h>

Match *Roebling_GetMatch(Roebling *rbl){
    if(HasFlag(rbl->type.state, KO)){
        return Span_GetSelected(rbl->matches.ko);
    }else{
        return Span_GetSelected(rbl->matches.values);
    }
}

int Roebling_GetMatchIdx(Roebling *rbl){
    if(HasFlag(rbl->type.state, KO)){
        return rbl->matches.ko->metrics.selected;
    }else{
        return rbl->matches.ko->metrics.selected;
    }
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

static status Roebling_RunMatches(Roebling *rbl){
    int i = 0;
    Span *ko = rbl->matches.ko;
    byte c = 0;
    while(!HasFlag(rbl->type.state, BREAK)){
        c = Range_GetNextByte(&(rbl->range));
        for(int i = 0; i < ko->nvalues; i++){
            Match *mt = Span_Get(ko, i);
            if(mt != NULL){
                Match_Feed(mt, c);
                if(HasFlag(mt->type.state, COMPLETE)){
                     ko->metrics.selected = i;
                     if(mt->jump > -1){
                        rbl->jump = mt->jump;
                     }
                     rbl->type.state = (NEXT|KO|BREAK);
                     printf("\x1b[%dmKO\x1b[0m\n", COLOR_YELLOW);
                     break;
                }
            }
        }
        Span *posative = rbl->matches.values;
        for(int i = 0; i < posative->nvalues; i++){
            Match *mt = Span_Get(posative, i);
            if(mt != NULL){
               Match_Feed(mt, c);
               if(HasFlag(mt->type.state, COMPLETE)){
                     posative->metrics.selected = i;
                     if(mt->jump > -1){
                        rbl->jump = mt->jump;
                     }
                     rbl->type.state = (NEXT|BREAK);
                     break;
               }
            }
        }
    }
    printf("MATCH RESULT %d\n", rbl->type.state);
    rbl->type.state &= ~BREAK;
    return rbl->type.state;
}

status Roebling_Run(Roebling *rbl){
    printf("RblRun %d, flags:%d\n", rbl->idx, rbl->type.state);

    if(HasFlag(rbl->type.state, NEXT)){
        printf("INCR\n");
        rbl->idx++;
        rbl->type.state &= ~NEXT; 
        Range_Incr(&(rbl->range));
    }

    printf("Getting %d, flags:%d\n", rbl->idx, rbl->type.state);
    Single *wdof = Span_Get(rbl->parsers_do, rbl->idx);
    if(wdof == NULL){
        rbl->type.state = COMPLETE;
        if(DEBUG_ROEBLING){
            printf("\x1b[%dmRbl DONE wof == NULL\x1b[0m\n", DEBUG_ROEBLING);
        }
    }else{
        wdof = as(wdof, TYPE_WRAPPED_DO);
        wdof->val.dof((MemHandle *)rbl);
        Roebling_RunMatches(rbl);
        if(DEBUG_ROEBLING){
            printf("\x1b[%dmRbl Run idx:\x1b[1;%dm%d\x1b[0;%dm ", DEBUG_ROEBLING, DEBUG_ROEBLING, rbl->idx, DEBUG_ROEBLING);
            Debug_Print((void *)rbl, 0, "", DEBUG_ROEBLING, TRUE);
            printf("\n");
        }
    }

    if(HasFlag(rbl->type.state, NEXT)){
        if(rbl->dispatch != NULL){
            rbl->dispatch((MemHandle *)rbl);
        }
        if(rbl->jump > -1){
            rbl->idx = rbl->jump;
            rbl->type.state &= ~NEXT; 
            rbl->jump = -1;
        }
        if(rbl->jumpMiss > -1){
            rbl->idx = rbl->jumpMiss;
            rbl->type.state &= ~NEXT; 
            rbl->jumpMiss = -1;
        }
    }
        
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
    rbl->jump = -1;
    rbl->jumpMiss = -1;
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

    return rbl;
}
