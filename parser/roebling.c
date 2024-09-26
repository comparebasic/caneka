#include <external.h>
#include <caneka.h>

String *Roebling_GetMarkDebug(Roebling *rbl, int idx){
    if(idx < 0){
        return NULL;
    }
    for(int i = 0; i <= rbl->marks->values->max_idx; i++){
        int *n = Span_Get(rbl->marks->values, i);
        if(n != NULL && *n == idx){
            return (String *)Span_Get(rbl->markLabels->values, i);
        }
    }
    return NULL;
}

Match *Roebling_GetMatch(Roebling *rbl){
    return Span_GetSelected(rbl->matches);
}

Match *Roebling_GetValueMatch(Roebling *rbl){
    if(rbl->matches->nvalues == 1){
        return Span_Get(rbl->matches, 0);
    }else{
        return Span_GetSelected(rbl->matches);
    }
}

int Roebling_GetMatchIdx(Roebling *rbl){
    return rbl->matches->metrics.selected;
}

status Roebling_SetPattern(Roebling *rbl, PatCharDef *def){
    Match *mt = Span_ReserveNext(rbl->matches);
    return Match_SetPattern(mt, def);
}

status Roebling_SetLookup(Roebling *rbl, Lookup *lk){
    for(int i = 0; i < lk->values->nvalues; i++){
        Match *mt = Span_ReserveNext(rbl->matches);
        String *s = (String *)Span_Get(lk->values, i);
        Match_SetString(mt, s);
    }
    return SUCCESS;
}

int Roebling_GetMarkIdx(Roebling *rlb, int mark){
    int *mrk = (int *)Lookup_Get(rlb->marks, mark); 
    if(mrk != 0){
        return *mrk;
    }
    return -1; 
}

status Roebling_SetMark(Roebling *rbl, int mark, int _idx){
    i64 idx = (i64)_idx;
    return Lookup_Add(rbl->m, rbl->marks, mark, (void *)&_idx);
}

status Roebling_Prepare(Roebling *rbl, Span *parsers){
    int idx = 0;
    int rblIdx = 0;
    Abstract *t = NULL;
    while(idx < parsers->nvalues){
        t = Span_Get(parsers, idx);
        if(t->type.of == TYPE_WRAPPED_UTIL){
            int mark = ((Single *)t)->val.value;
            Roebling_SetMark(rbl, mark, rblIdx);
        }else if(t->type.of == TYPE_WRAPPED_DO){
            rblIdx = Span_Add(rbl->parsers_do, t) + 1;
        }
        idx++;
    }
    return SUCCESS;
}

static status Roebling_RunMatches(Roebling *rbl){
    int i = 0;
    byte c = 0;
    rbl->type.state &= ~(NEXT|KO|BREAK|COMPLETE);
    if(rbl->matches->nvalues == 0){
        rbl->type.state |= (BREAK|NEXT);
    }
    while(!HasFlag(rbl->type.state, BREAK) && !HasFlag(rbl->range.potential.type.state, END)){
        c = Range_GetByte(&(rbl->range));
        Match *mt = NULL;
        for(int i = 0; i < rbl->matches->nvalues; i++){
            mt = Span_Get(rbl->matches, i);
            if(mt != NULL){
               Match_Feed(mt, c);
               if(HasFlag(mt->type.state, COMPLETE)){
                     rbl->matches->metrics.selected = i;
                     if(mt->jump > -1){
                        rbl->jump = mt->jump;
                     }
                     rbl->type.state = (NEXT|BREAK);
                     SCursor_Incr(&(rbl->range.start), mt->lead);
                     if(mt->count){
                         SCursor_Incr(&(rbl->range.end), mt->lead+mt->count);
                     }
                     rbl->tail = mt->tail;
                     break;
               }
            }
        }
        SCursor_Incr(&(rbl->range.potential), 1);
    }
    rbl->type.state &= ~BREAK;
    return rbl->type.state;
}

status Roebling_Run(Roebling *rbl){
    if(DEBUG_ROEBLING_MARK){
        printf("\x1b[%dmRblIdx:%d\x1b[0m\n", DEBUG_ROEBLING_MARK, rbl->idx);
    }
    if(HasFlag(rbl->type.state, NEXT)){
        rbl->idx++;
        SCursor_Incr(&(rbl->range.end), rbl->tail);
        rbl->tail = 0;
        Range_Sync(&(rbl->range), &(rbl->range.end));
        if(rbl->jump > -1){
            rbl->idx = rbl->jump;
            rbl->jump = -1;
            if(DEBUG_ROEBLING_MARK){
                String *mark_s = Roebling_GetMarkDebug(rbl, rbl->idx);
                printf("\x1b[%dmJumping to %s(%d)\n", DEBUG_ROEBLING_MARK, mark_s != NULL ? (char *)mark_s->bytes : "", rbl->idx);
            }
        }
    }else{
        if(rbl->jumpMiss > -1){
            rbl->idx = rbl->jumpMiss;
            rbl->jumpMiss = -1;
            rbl->type.state &= ~NEXT;
            Range_Sync(&(rbl->range), &(rbl->range.end));
        }
    }
    rbl->type.state &= ~(NEXT|NOOP); 

    Single *wdof = Span_Get(rbl->parsers_do, rbl->idx);
    if(wdof == NULL){
        rbl->type.state = COMPLETE;
        if(DEBUG_ROEBLING){
            printf("\x1b[%dmRbl DONE wof == NULL\x1b[0m\n", DEBUG_ROEBLING);
        }
    }else{
        wdof = as(wdof, TYPE_WRAPPED_DO);
        ((RblFunc)(wdof->val.dof))(rbl);
        if(DEBUG_ROEBLING){
            printf("\x1b[%dmRbl Run 0x%lx idx:\x1b[1;%dm%d\x1b[0;%dm ", DEBUG_ROEBLING, (util)wdof, DEBUG_ROEBLING, rbl->idx, DEBUG_ROEBLING);
            Debug_Print((void *)rbl, 0, "", DEBUG_ROEBLING, TRUE);
            printf("\n");
        }
        Roebling_RunMatches(rbl);
    }

    if(HasFlag(rbl->type.state, NEXT)){
        Match *mt = Roebling_GetValueMatch(rbl);
        if(DEBUG_ROEBLING_COMPLETE){
            Debug_Print((void *)mt, 0, "Match Found: ", DEBUG_ROEBLING_COMPLETE, TRUE);
            printf("\n");
            Debug_Print((void *)&(rbl->range), 0, "Range: ", DEBUG_ROEBLING_COMPLETE, FALSE);
            printf("\n");
        }
        if(mt != NULL){
            rbl->type.state |= (mt->type.state & NOOP);
            if(HasFlag(mt->type.state, SUCCESS) && mt->jump > -1){
                rbl->jump = mt->jump; 
            }
        }
        if(rbl->dispatch != NULL){
            rbl->dispatch(rbl);
        }
        if(mt->dispatch){
            mt->dispatch(rbl);
        }
    }
        
    return rbl->type.state;
}

status Roebling_ResetPatterns(Roebling *rbl){
    Span_ReInit(rbl->matches);
    rbl->jump = -1;
    rbl->jumpMiss = -1;
    rbl->dispatch = NULL;
    return READY;
}

status Roebling_AddBytes(Roebling *rbl, byte bytes[], int length){
    return String_AddBytes(rbl->m, rbl->range.search, bytes, length);
}

Roebling *Roebling_Make(MemCtx *m,
        cls type,
        Span *parsers,
        Lookup *markLabels,
        String *s,
        Abstract *source
    ){
    Roebling *rbl = (Roebling *)MemCtx_Alloc(m, sizeof(Roebling));
    rbl->type.of = TYPE_ROEBLING;
    rbl->m = m;
    rbl->source = source;
    rbl->matches = Span_MakeInline(rbl->m, TYPE_MATCH, (int)sizeof(Match));  
    rbl->parsers_do = Span_Make(m, TYPE_SPAN);
    int markStart = 0;
    if(markLabels != NULL){
        markStart = markLabels->offset;
    }
    rbl->marks = LookupInt_Make(m, markStart, (Abstract *)rbl); 
    rbl->markLabels = markLabels;
    Roebling_Prepare(rbl, parsers);
    Range_Set(&(rbl->range), s);

    return rbl;
}
