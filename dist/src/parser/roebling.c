#include <external.h>
#include <caneka.h>

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
    while((rbl->type.state & (ROEBLING_NEXT|END)) == 0){
        Guard_Incr(&rbl->guard);
        if(HasFlag(rbl->range.potential.type.state, END)){
            rbl->type.state |= END;
            continue;
        }
        c = Range_GetByte(&(rbl->range));
        Match *mt = NULL;
        int ranCount = 0;
        int i = 0;
        for(i = 0; i < rbl->matches->nvalues; i++){
            mt = Span_Get(rbl->matches, i);

            if(DEBUG_ROEBLING){
                printf("\x1b[%dm%d ", DEBUG_ROEBLING, i);
                Debug_Print(mt, 0, "Roebling Running Match: ", DEBUG_ROEBLING, TRUE);
            }

            if(mt != NULL){

               if(DEBUG_PATMATCH){
                    String *sec = Roebling_GetMarkDebug(rbl, rbl->idx);
                    printf("\x1b[%dmrbl:%s/match:%d - ", DEBUG_PATMATCH, String_ToChars(rbl->m, sec), i);
               }
               Match_Feed(mt, c);
               if((mt->type.state & NOOP) == 0){
                    ranCount++;
               }
               if((mt->type.state & SUCCESS) != 0){
                     rbl->type.state &= ~PROCESSING;
                     rbl->type.state = ROEBLING_NEXT;

                     rbl->matches->metrics.selected = i;
                     if(mt->jump > -1){
                        rbl->jump = mt->jump;
                     }

                     if(mt->lead+mt->count+mt->tail == 0){
                        Fatal("No increment value for successful match", TYPE_PATMATCH);
                     }

                     SCursor_Incr(&(rbl->range.start), mt->lead);
                     if(mt->count){
                         SCursor_Incr(&(rbl->range.end), mt->lead+mt->count);
                     }
                     rbl->tail = mt->tail;

                     if(DEBUG_ROEBLING_COMPLETE){
                         printf("\x1b[%dm#%d ", DEBUG_ROEBLING_COMPLETE, i);
                         Debug_Print((void *)mt, 0, "Match Found: ", DEBUG_ROEBLING_COMPLETE, TRUE);
                         String *s = Range_Copy(rbl->m, &(rbl->range));
                         printf(" \x1b[1;%dm(", DEBUG_ROEBLING_COMPLETE);
                         Debug_Print((void *)s, 0, "", DEBUG_ROEBLING_COMPLETE, TRUE);
                         printf("\x1b[1;%dmjumpTo:%d)\n\x1b[0m", DEBUG_ROEBLING_COMPLETE, mt->jump);
                     }

                     break;
               }
            }
        }
        if(ranCount == 0){
            rbl->type.state |= (NOOP|END);
        }
        if(HasFlag(mt->type.state, MATCH_INVERTED)){
            break;
        }
        SCursor_Incr(&(rbl->range.potential), 1);
    }


    Return rbl->type.state;
}

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

status Roebling_SetPattern(Roebling *rbl, PatCharDef *def, word captureKey, int jump){
    Match *mt = Span_ReserveNext(rbl->matches);
    status r = READY;
    r |= Match_SetPattern(mt, def);
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
        Match_SetString(rbl->m, mt, s);
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

status Roebling_SetMark(Roebling *rbl, int mark, int _idx){
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

status Roebling_Run(Roebling *rbl){
    status r = READY;
    while((r & (SUCCESS|ERROR|END)) == 0){
        r = Roebling_RunCycle(rbl);
    }
    rbl->type.state &= ~END;
    return rbl->type.state;
}

status Roebling_JumpTo(Roebling *rbl, int mark){
    rbl->jump = Roebling_GetMarkIdx(rbl, mark);
    rbl->type.state &= ~PROCESSING;
    return rbl->type.state;
}

status Roebling_RunCycle(Roebling *rbl){
    if(DEBUG_ROEBLING_MARK){
        printf("\x1b[%dmRblIdx:%d %s\x1b[0m\n", DEBUG_ROEBLING_MARK, rbl->idx, State_ToChars(rbl->type.state));
    }
    rbl->type.state &= ~END;
    if(HasFlag(rbl->type.state, ROEBLING_NEXT)){
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
            rbl->type.state &= ~ROEBLING_NEXT;
            Range_Sync(&(rbl->range), &(rbl->range.end));
        }
    }
    rbl->type.state &= ~(ROEBLING_NEXT|NOOP); 

    Single *wdof = Span_Get(rbl->parsers_do, rbl->idx);
    if(wdof == NULL){
        printf("wdof is null\n");
        if((rbl->type.state & ROEBLING_REPEAT) != 0){
            printf("repeating\n");
            rbl->type.state |= ROEBLING_NEXT; 
            rbl->jump = 0;
        }else{
            rbl->type.state = SUCCESS;
            if(DEBUG_ROEBLING_COMPLETE){
                printf("\x1b[%dmRbl DONE wof == NULL\x1b[0m\n", DEBUG_ROEBLING_COMPLETE);
            }

            if(DEBUG_ROEBLING){
                printf("\x1b[%dmRbl DONE wof == NULL\x1b[0m\n", DEBUG_ROEBLING);
            }
        }
    }else{
        if((rbl->type.state & PROCESSING) == 0){
            rbl->type.state |= PROCESSING;
            wdof = as(wdof, TYPE_WRAPPED_DO);
            ((RblFunc)(wdof->val.dof))(rbl->m, rbl);

            if(DEBUG_ROEBLING){
                printf("\x1b[%dmRbl Run 0x%lx idx:\x1b[1;%dm%d\x1b[0;%dm ", DEBUG_ROEBLING, (util)wdof, DEBUG_ROEBLING, rbl->idx, DEBUG_ROEBLING);
                Debug_Print((void *)rbl, 0, "", DEBUG_ROEBLING, TRUE);
                printf("\n");
            }

        }
        Roebling_RunMatches(rbl);
    }

    if(HasFlag(rbl->type.state, ROEBLING_NEXT)){
        Match *mt = Roebling_GetValueMatch(rbl);
        if(mt != NULL){
            rbl->type.state |= (mt->type.state & NOOP);
            if(HasFlag(mt->type.state, SUCCESS) && mt->jump > -1){
                rbl->jump = mt->jump; 
            }
        }
        String *s = Range_Copy(rbl->m, &(rbl->range));
        int matchIdx = Roebling_GetMatchIdx(rbl);
        rbl->capture(mt->captureKey, matchIdx, s, rbl->source);
    }
        
    return rbl->type.state;
}

status Roebling_ResetPatterns(Roebling *rbl){
    Span_ReInit(rbl->matches);
    rbl->jump = -1;
    rbl->jumpMiss = -1;
    rbl->guard.count = 0;
    return READY;
}

status roebling_AddReset(Roebling *rbl){
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
    return SUCCESS;
}

status Roebling_Add(Roebling *rbl, String *s){
    String_Add(rbl->m, rbl->range.search, s);
    return roebling_AddReset(rbl);
}

status Roebling_AddBytes(Roebling *rbl, byte bytes[], int length){
    status r = String_AddBytes(rbl->m, rbl->range.search, bytes, length);
    return roebling_AddReset(rbl);
}

status Roebling_Reset(MemCtx *m, Roebling *rbl, String *s){
    if(s != NULL){
        Range_Set(&rbl->range, s);
    }
    /*
    rbl->type.state &= ~(SUCCESS|ERROR|END);
    */
    Roebling_ResetPatterns(rbl);
    rbl->type.state = ZERO;
    rbl->idx = 0;

    return SUCCESS;
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

    Roebling *rbl = (Roebling *)MemCtx_Alloc(m, sizeof(Roebling));
    rbl->type.of = TYPE_ROEBLING;
    rbl->m = m;
    rbl->source = source;
    rbl->capture = capture;
    rbl->matches = Span_MakeInline(rbl->m, TYPE_PATMATCH, (int)sizeof(Match));  
    rbl->parsers_do = Span_Make(m, TYPE_SPAN);
    int markStart = 0;
    if(markLabels != NULL){
        markStart = markLabels->offset;
    }
    rbl->marks = LookupInt_Make(m, markStart, (Abstract *)rbl); 
    rbl->markLabels = markLabels;
    Roebling_Prepare(rbl, parsers);
    Roebling_Reset(m, rbl, s);
    Guard_Setup(m, &rbl->guard, ROEBLING_GUARD_MAX, bytes("Roebling Guard"));

    return rbl;
}
