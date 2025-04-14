#include <external.h>
#include <caneka.h>

static status Roebling_RunMatches(Roebling *rbl, Str *s){
    DebugStack_Push(rbl, rbl->type.of);
    /*

    byte c = 0;
    rbl->type.state &= ~(ROEBLING_NEXT|END|SUCCESS);
    if(rbl->matches->nvalues == 0){
        rbl->type.state |= ROEBLING_NEXT;
    }

    byte *ptr = s->bytes;
    byte *end = s->bytes+s->length-1;
    int noopCount = 0;
    rbl->type.state |= (rbl->cursor.type.state & END);
    while(start <= end && (rbl->type.state & (ROEBLING_NEXT|END)) == 0){
        Guard_Incr(&rbl->guard);

        c = *ptr++;

        Match *mt = NULL;
        Iter it;
        Iter_Init(&it, rbl->matches);
        boolean incr = TRUE;
        noopCount = 0;
        while((Iter_Next(&it) & END) == 0){
            mt = (Match *)Iter_Get(&it);
            DebugStack_SetRef(mt, mt->type.of);
            incr = TRUE;
            if(DEBUG_PATMATCH){
                String *sec = Roebling_GetMarkDebug(rbl, rbl->idx);
                printf("\x1b[1;%dmmrbl:%s/match:%d - \x1b[0m", DEBUG_PATMATCH, String_ToChars(rbl->m, sec), it.idx);
            }

            if((Match_Feed(rbl->m, mt, c) & SUCCESS) != 0){
                 if(StrSnipStr_Total(mt->backlog, (STRSNIP_CONTENT)) == 0
                    && StrSnipStr_Total(mt->backlog, (STRSNIP_GAP)) == 0 && (mt->type.state & MATCH_ACCEPT_EMPTY) == 0){
                    Fatal("No increment value for successful match", TYPE_PATMATCH);
                 }

                 rbl->type.state = ROEBLING_NEXT;
                 rbl->type.state &= ~PROCESSING;
                 rbl->matches->metrics.selected = it.idx;

                 if(mt->jump > -1){
                    rbl->jump = mt->jump;
                 }

                 String *s = StrSnipStr_ToString(rbl->m, mt->backlog, rbl->cursor.s);
                 rbl->capture(mt->captureKey, it.idx, s, rbl->source);
                 if((mt->snip.type.state & STRSNIP_UNCLAIMED) != 0){
                    incr = FALSE;
                    if(mt->snip.length > 1){
                        Cursor_Decr(&(rbl->cursor), mt->snip.length-1);
                    }
                 }
                 break;
            }
            if((mt->type.state & NOOP) != 0){
                noopCount++;
            }
        }

        if((mt->type.state & MATCH_INVERTED) != 0){
            break;
        }

        if(incr){
            Cursor_Incr(&(rbl->cursor), 1);
        }
        rbl->type.state |= (rbl->cursor.type.state & END);

        if(DEBUG_PATMATCH && rbl->type.state & DEBUG){
            Stepper(rbl->m, (Abstract *)rbl);
        }
        if(noopCount == rbl->matches->nvalues){
            if(DEBUG_PATMATCH){
                printf("\x1b[%dnoopCount rached adding NOOP|END flags\x1b[0m\n",
                    DEBUG_PATMATCH);
            }
            rbl->type.state |= (NOOP|END|ERROR);
        }
    }

    */
    DebugStack_Pop();
    return rbl->type.state;
}

status Roebling_RunCycle(Roebling *rbl){
    DebugStack_Push(rbl, rbl->type.of);
    /*
    if(rbl->parsers->nvalues == 0){
        Fatal("Roebling parsers not set", TYPE_ROEBLING);
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
        }
    }
    rbl->type.state &= ~(ROEBLING_NEXT); 

    Single *wdof = Span_Get(rbl->parsers, rbl->idx);
    if(wdof == NULL){
        if((rbl->type.state & ROEBLING_REPEAT) != 0){
            rbl->type.state |= (ROEBLING_NEXT|ROEBLING_LOAD_MATCHES); 
            rbl->jump = 0;
        }else{
            rbl->type.state = SUCCESS;
        }
    }else{
        if((rbl->type.state & (PROCESSING|ROEBLING_LOAD_MATCHES)) == 0){
            wdof = as(wdof, TYPE_WRAPPED_DO);
            ((RblFunc)(wdof->val.dof))(rbl->m, rbl);
            rbl->type.state &= ~ROEBLING_LOAD_MATCHES;
            rbl->type.state |= PROCESSING;
        }
        Roebling_RunMatches(rbl);
    }

    */
    DebugStack_Pop();
    return rbl->type.state;
}

status Roebling_JumpTo(Roebling *rbl, i32 mark){
    Single *sg = Lookup_Get(rbl->marks, mark);
    rbl->jump = sg->val.value;
    return rbl->type.state;
}

status Roebling_Run(Roebling *rbl){
    status r = READY;
    while((r & (SUCCESS|ERROR|END|NOOP)) == 0){
        r = Roebling_RunCycle(rbl);
    }
    rbl->type.state &= ~END;
    return rbl->type.state;
}

Match *Roebling_GetMatch(Roebling *rbl){
    return rbl->matchIt.value;
}

i32 Roebling_GetMatchIdx(Roebling *rbl){
    return rbl->matchIt.metrics.selected;
}

status Roebling_ResetPatterns(Roebling *rbl){
    MemCh_Free(rbl->m);
    Iter_Init(&rbl->matchIt, Span_Make(rbl->m));
    rbl->snips = Span_Make(rbl->m);
    rbl->jump = -1;
    rbl->jumpMiss = -1;
    rbl->guard.count = 0;
    return READY;
}

status Roebling_SetPattern(Roebling *rbl, PatCharDef *def, word captureKey, i32 jump){

    Span *sns = (Span *)Span_Get(rbl->snips, rbl->matches->max_idx);
    if(sns == NULL){
        sns = Span_Make(rbl->m);
        Span_Add(rbl->snips, (Abstract *)sns);
    }

    Match *mt = Match_Make(rbl->m, def, sns);
    mt->captureKey = captureKey;
    if(jump != -1){
        mt->jump = Roebling_GetMarkIdx(rbl, jump);
    }

    return SUCCESS;
}

i64 Roebling_GetMarkIdx(Roebling *rbl, i32 mark){
    Single *sg = (Single *)Lookup_Get(rbl->marks, mark); 
    if(sg != NULL){
        return sg->val.value;
    }
    return -1; 
}

status Roebling_Reset(MemCh *m, Roebling *rbl, String *s){
    DebugStack_Push("Roebling_Reset", TYPE_CSTR); 
    if(s != NULL){
        Cursor_Init(&rbl->cursor, s);
    }

    if(rbl->type.of != TYPE_ROEBLING_BLANK){
        Roebling_ResetPatterns(rbl);
        rbl->idx = 0;
    }

    rbl->type.state = (rbl->type.state & DEBUG);
    DebugStack_Pop();
    return SUCCESS;
}

status Roebling_AddStep(Roebling *rbl, Abstract *step){
    if(step->type.of == TYPE_WRAPPED_DO){
        Single *sg = Span_Get(rbl->parsers, rbl->parsers->max_idx);
        if(sg->type.of == TYPE_WRAPPED_I64){
            i64 mark = sg->val.value;
            sg->val.value = rbl->parsers->max_idx;
            Span_Set(rbl->marks, sg->val.value, (Abstract *)sg);
            return Span_Set(rbl->parsers, rbl->parsers->max_idx, step);
        }
    }

    return Span_Add(rbl->parsers, rbl->parsers->max_idx, step);
}

Roebling *Roebling_Make(MemCh *m,
        cls type,
        Lookup *markLabels,
        Cursor *curs,
        RblCaptureFunc capture,
        Abstract *source
    ){

    Roebling *rbl = (Roebling *)MemCh_Alloc(m, sizeof(Roebling));
    rbl->type.of = TYPE_ROEBLING;
    rbl->m = m;
    rbl->source = source;
    rbl->capture = capture;
    rbl->curs = curs;
    rbl->marks = Lookup_Make(m, _TYPE_CORE_END, (Abstract *)rbl); 
    Roebling_Reset(m, rbl, s);
    Guard_Setup(m, &rbl->guard, ROEBLING_GUARD_MAX, (byte *)"Roebling Guard");

    return rbl;
}
