#include <external.h>
#include <caneka.h>

/* > Run */
static status Roebling_RunMatches(Roebling *rbl){
    Stack(bytes("Roebling_RunMatches"), (Abstract *)rbl);

    if(DEBUG_ROEBLING_CURRENT){
        Debug_Print((void *)rbl, 0, "RblCurrent - RunMatches", DEBUG_ROEBLING_CURRENT, FALSE);
        printf("\n");
    }

    byte c = 0;
    rbl->type.state &= ~(ROEBLING_NEXT|END|SUCCESS);
    if(rbl->matches->nvalues == 0){
        rbl->type.state |= ROEBLING_NEXT;
    }

    int noopCount = 0;
    rbl->type.state |= (rbl->cursor.type.state & END);
    while((rbl->type.state & (ROEBLING_NEXT|END)) == 0){
        Guard_Incr(&rbl->guard);
        c = Cursor_GetByte(&(rbl->cursor));
        Match *mt = NULL;
        Iter it;
        Iter_Init(&it, rbl->matches);
        while((Iter_Next(&it) & END) == 0){
           mt = (Match *)Iter_Get(&it);

           if(DEBUG_PATMATCH){
                String *sec = Roebling_GetMarkDebug(rbl, rbl->idx);
                printf("\x1b[%dmrbl:%s/match:%d - ", DEBUG_PATMATCH, String_ToChars(rbl->m, sec), it.idx);
           }

           if((Match_Feed(rbl->m, mt, c) & SUCCESS) != 0){
                 if(Match_Total(mt) == 0){
                    Fatal("No increment value for successful match", TYPE_PATMATCH);
                 }

                 rbl->type.state = ROEBLING_NEXT;
                 rbl->matches->metrics.selected = it.idx;

                 if(mt->jump > -1){
                    rbl->jump = mt->jump;
                 }

                 String *s = StrSnip_ToString(rbl->m, mt->backlog, rbl->cursor.s);
                 rbl->capture(mt->captureKey, it.idx, s, rbl->source);
                 break;
             }
        }

        if(noopCount == rbl->matches->nvalues){
            rbl->type.state |= (NOOP|END);
        }

        if((mt->type.state & MATCH_INVERTED) != 0){
            break;
        }

        Cursor_Incr(&(rbl->cursor), 1);
        rbl->type.state |= (rbl->cursor.type.state & END);
    }

    if((rbl->type.state & ROEBLING_NEXT)){
        Match *mt = Roebling_GetValueMatch(rbl);
        if(mt != NULL){
        }
    }

    Return rbl->type.state;
}

status Roebling_RunCycle(Roebling *rbl){
    Stack(bytes("Roebling_RunCycle"), (Abstract *)rbl);
    if(rbl->parsers_do->nvalues == 0){
        Fatal("Roebling parsers not set", TYPE_ROEBLING);
    }
    if(DEBUG_ROEBLING_MARK){
        printf("\x1b[%dmRblIdx:%d %s\x1b[0m\n", DEBUG_ROEBLING_MARK, rbl->idx, State_ToChars(rbl->type.state));
    }
    rbl->type.state &= ~END;
    if((rbl->type.state & ROEBLING_NEXT) != 0){
        rbl->idx++;
        rbl->tail = 0;
        if(rbl->jump > -1){
            rbl->idx = rbl->jump;
            rbl->jump = -1;
        }
    }else{
        if(rbl->jumpMiss > -1){
            rbl->idx = rbl->jumpMiss;
            rbl->jumpMiss = -1;
            rbl->type.state &= ~ROEBLING_NEXT;
            /*
            Range_Sync(&(rbl->range), &(rbl->cursor));
            */
        }
    }
    rbl->type.state &= ~(ROEBLING_NEXT|NOOP); 

    Single *wdof = Span_Get(rbl->parsers_do, rbl->idx);
    if(wdof == NULL){
        if((rbl->type.state & ROEBLING_REPEAT) != 0){
            rbl->type.state |= (ROEBLING_NEXT|ROEBLING_LOAD_MATCHES); 
            rbl->jump = 0;
        }else{
            rbl->type.state = SUCCESS;
        }
    }else{
        if((rbl->type.state & ROEBLING_LOAD_MATCHES) == 0){
            wdof = as(wdof, TYPE_WRAPPED_DO);
            ((RblFunc)(wdof->val.dof))(rbl->m, rbl);
            rbl->type.state &= ~ROEBLING_LOAD_MATCHES;
        }
        Roebling_RunMatches(rbl);
    }

    Return rbl->type.state;
}

status Roebling_JumpTo(Roebling *rbl, int mark){
    rbl->jump = Roebling_GetMarkIdx(rbl, mark);
    return rbl->type.state;
}

status Roebling_Run(Roebling *rbl){
    Stack(bytes("Roebling_Run"), (Abstract *)rbl);
    status r = READY;
    while((r & (SUCCESS|ERROR|END)) == 0){
        r = Roebling_RunCycle(rbl);
    }
    rbl->type.state &= ~END;
    Return rbl->type.state;
}

/* > Debug */
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

Match *Roebling_LatestMatch(Roebling *rbl){
    return Span_Get(rbl->matches, rbl->matches->metrics.set);
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

/* > Setup Cycle */
static status roebling_AddReset(Roebling *rbl){
    /*
    if((rbl->range.potential.type.state & END) != 0){
        rbl->range.potential.type.state &= ~END;
        SCursor_Incr(&(rbl->range.potential), 1);
    }
    if((rbl->range.end.type.state & END) != 0){
        SCursor_Incr(&(rbl->range.end), 1);
        rbl->range.end.type.state &= ~END;
    }
    if((rbl->range.start.type.state & END) != 0){
        SCursor_Incr(&(rbl->range.start), 1);
        rbl->range.start.type.state &= ~END;
    }
    */
    return SUCCESS;
}

status Roebling_AddBytes(Roebling *rbl, byte bytes[], int length){
    Stack(bytes("Roebling_AddBytes"), (Abstract *)rbl);
    status r = String_AddBytes(rbl->m, rbl->cursor.s, bytes, length);
    Return roebling_AddReset(rbl);
}

status Roebling_Add(Roebling *rbl, String *s){
    String_Add(rbl->m, rbl->cursor.s, s);
    return roebling_AddReset(rbl);
}

status Roebling_ResetPatterns(Roebling *rbl){
    Span_ReInit(rbl->matches);
    rbl->jump = -1;
    rbl->jumpMiss = -1;
    rbl->guard.count = 0;
    return READY;
}

status Roebling_SetPattern(Roebling *rbl, PatCharDef *def, word captureKey, int jump){
    Match *mt = Span_ReserveNext(rbl->matches);
    status r = READY;

    String *sns = (String *)Span_Get(rbl->snips, rbl->matches->max_idx);
    if(sns == NULL){
        String *s = String_Init(rbl->m, STRING_EXTEND);
        s->type.state |= FLAG_STRING_CONTIGUOUS;
        Span_Add(rbl->snips, (Abstract *)s);
    }else{
        String_Reset(sns);
    }

    r |= Match_SetPattern(mt, def, sns);
    mt->captureKey = captureKey;
    if(jump != -1){
        mt->jump = Roebling_GetMarkIdx(rbl, jump);
    }
    return r;
}

status Roebling_SetLookup(Roebling *rbl, Lookup *lk, word captureKey, int jump){
    for(int i = 0; i < lk->values->nvalues; i++){
        Match *mt = Span_ReserveNext(rbl->matches);
        String *s = (String *)Span_Get(lk->values, i);

        String *sns = (String *)Span_Get(rbl->snips, rbl->matches->max_idx);
        if(sns == NULL){
            String *s = String_Init(rbl->m, STRING_EXTEND);
            s->type.state |= FLAG_STRING_CONTIGUOUS;
            Span_Add(rbl->snips, (Abstract *)s);
        }else{
            String_Reset(sns);
        }

        Match_SetString(rbl->m, mt, s, sns);
        mt->captureKey = captureKey;
        if(jump != -1){
            mt->jump = Roebling_GetMarkIdx(rbl, jump);
        }
    }
    return SUCCESS;
}

int Roebling_GetMarkIdx(Roebling *rbl, int mark){
    int *mrk = (int *)Lookup_Get(rbl->marks, mark); 
    if(mrk != NULL){
        return *mrk;
    }
    return -1; 
}

/* > Reset */
status Roebling_Reset(MemCtx *m, Roebling *rbl, String *s){
    if(s != NULL){
        Cursor_Init(&rbl->cursor, s);
    }
    Roebling_ResetPatterns(rbl);
    rbl->type.state = ZERO;
    rbl->idx = 0;

    return SUCCESS;
}


/* > Instantation */
static status Roebling_SetMark(Roebling *rbl, int mark, int _idx){
    return Lookup_Add(rbl->m, rbl->marks, mark, (void *)&_idx);
}

static status Roebling_Prepare(Roebling *rbl, Span *parsers){
    Stack(bytes("Roebling_Prepare"), (Abstract *)parsers);
    int idx = 0;
    int rblIdx = 0;
    Abstract *t = NULL;
    if(rbl->capture == NULL){
        Fatal("Roebling->capture not defined", TYPE_ROEBLING);
        return ERROR;
    }
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
    Return SUCCESS;
}

status Roebling_AddParsers(MemCtx *m, Span *parsers, Lookup *marks, Span *additions, Lookup *desc){
    status r = READY;
    r |= Span_Concat(parsers, additions);
    r |= Lookup_Concat(m, marks, desc);
    return r;
}

Roebling *Roebling_Make(MemCtx *m,
        cls type,
        Span *parsers,
        Lookup *markLabels,
        String *s,
        RblCaptureFunc capture,
        Abstract *source
    ){
    Stack(bytes("Roebling_Make"), NULL);

    Roebling *rbl = (Roebling *)MemCtx_Alloc(m, sizeof(Roebling));
    rbl->type.of = TYPE_ROEBLING;
    rbl->m = m;
    rbl->source = source;
    rbl->capture = capture;
    rbl->matches = Span_MakeInline(rbl->m, TYPE_PATMATCH, (int)sizeof(Match));  
    rbl->snips = Span_Make(m, TYPE_SPAN);
    rbl->parsers_do = Span_Make(m, TYPE_SPAN);
    int markStart = 0;
    if(markLabels != NULL){
        markStart = markLabels->offset;
    }
    rbl->marks = LookupInt_Make(m, markStart, (Abstract *)rbl); 
    rbl->markLabels = markLabels;
    Cursor_Init(&rbl->cursor, s); 
    Roebling_Prepare(rbl, parsers);
    Roebling_Reset(m, rbl, s);
    Guard_Setup(m, &rbl->guard, ROEBLING_GUARD_MAX, bytes("Roebling Guard"));

    Return rbl;
}
