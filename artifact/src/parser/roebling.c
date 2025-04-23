#include <external.h>
#include <caneka.h>

static inline status Roebling_RunMatches(Roebling *rbl){
    DebugStack_Push(rbl, rbl->type.of);

    rbl->type.state |= (rbl->curs->type.state & END);
    Cursor_NextByte(rbl->curs);
    while((rbl->type.state & END) == 0){
        Guard_Incr(&rbl->guard, RBL_GUARD_MAX, FUNCNAME, FILENAME, LINENUMBER);

        byte c = *(rbl->curs->ptr);
        i32 noopCount = 0;

        Type_SetFlag((Abstract *)&rbl->matchIt, SPAN_OP_GET);
        Iter_Reset(&rbl->matchIt);
        while((Iter_Next(&rbl->matchIt) & END) == 0){
            Match *mt = (Match *)rbl->matchIt.value;
            DebugStack_SetRef(mt, mt->type.of);

            if((Match_Feed(rbl->m, mt, c) & SUCCESS) != 0){
                rbl->type.state = ROEBLING_NEXT;
                rbl->type.state &= ~PROCESSING;

                StrVec *v = StrVec_Snip(rbl->m, mt->backlog, rbl->curs);
                rbl->capture(rbl, mt->captureKey, v);

                if((mt->snip.type.state & SNIP_UNCLAIMED) != 0 &&
                        mt->snip.length > 1){
                    Cursor_Decr(rbl->curs, mt->snip.length-1);
                }

                rbl->type.state |= (rbl->curs->type.state & END);
                DebugStack_Pop();
                Guard_Reset(&rbl->guard);
                return rbl->type.state;
            }

            if((mt->type.state & NOOP) != 0){
                if(++noopCount == rbl->matchIt.span->nvalues){
                    rbl->type.state |= (NOOP|END|ERROR);
                    Guard_Reset(&rbl->guard);
                    return rbl->type.state;
                }
            }
        }

        if(Cursor_NextByte(rbl->curs) & CURSOR_STR_BOUNDRY){
            Iter_Reset(&rbl->matchIt);
            while((Iter_Next(&rbl->matchIt) & END) == 0){
                Match_AddBoundrySnip(rbl->m, (Match *)rbl->matchIt.value);
            }
        }
        rbl->type.state |= (rbl->curs->type.state & END);
    }

    DebugStack_Pop();
    Guard_Reset(&rbl->guard);
    return rbl->type.state;
}

status Roebling_RunCycle(Roebling *rbl){
    DebugStack_Push(rbl, rbl->type.of);
    if(rbl->parseIt.span->nvalues == 0){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER,
            "Roebling parsers not set", NULL);
    }
    rbl->type.state &= ~END;
    if((rbl->type.state & ROEBLING_NEXT) != 0){
        Match *mt = rbl->matchIt.value;
        rbl->tail = 0;
        if(mt != NULL && mt->jump > -1){
            rbl->parseIt.idx = mt->jump;
        }else{
            rbl->parseIt.idx++;
        }
        rbl->type.state &= ~ROEBLING_NEXT;
    }

    Type_SetFlag((Abstract *)&rbl->parseIt, SPAN_OP_GET);
    Iter_Query(&rbl->parseIt);
    Single *wdof = rbl->parseIt.value;
    if(wdof == NULL){
        if((rbl->type.state & ROEBLING_REPEAT) != 0){
            rbl->type.state |= ROEBLING_NEXT;
        }else{
            rbl->type.state = SUCCESS;
        }
    }else{
        if((rbl->type.state & PROCESSING) == 0){
            wdof = (Single *)as((Abstract *)wdof, TYPE_WRAPPED_DO);
            ((RblFunc)(wdof->val.dof))(rbl->m, rbl);
            rbl->type.state |= PROCESSING;
        }
        Roebling_RunMatches(rbl);
    }

    DebugStack_Pop();
    return rbl->type.state;
}

status Roebling_JumpTo(Roebling *rbl, i32 mark){
    Single *sg = Lookup_Get(rbl->marks, mark);
    rbl->parseIt.idx = sg->val.value;
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
    if(rbl->m->type.range > 0){
        MemCh_Free(rbl->m);
        Iter_Init(&rbl->matchIt, Span_Make(rbl->m));
    }

    Span *p = Span_Make(rbl->m);
    Iter_Init(&rbl->matchIt, p);

    Guard_Reset(&rbl->guard);
    return READY;
}

status Roebling_SetPattern(Roebling *rbl, PatCharDef *def, word captureKey, i32 jump){
    Match *mt = Match_Make(rbl->m, def, NULL);
    mt->captureKey = captureKey;
    if(jump != -1){
        mt->jump = Roebling_GetMarkIdx(rbl, jump);
    }

    Type_SetFlag((Abstract *)&rbl->matchIt, SPAN_OP_ADD);
    rbl->matchIt.value = mt;
    return Iter_Query(&rbl->matchIt);
}

i64 Roebling_GetMarkIdx(Roebling *rbl, i32 mark){
    Single *sg = (Single *)Lookup_Get(rbl->marks, mark); 
    if(sg != NULL){
        return sg->val.value;
    }
    return -1; 
}

status Roebling_Reset(MemCh *m, Roebling *rbl, StrVec *v){
    DebugStack_Push("Roebling_Reset", TYPE_CSTR); 
    Roebling_ResetPatterns(rbl);
    if(v != NULL){
        Cursor_Setup(rbl->curs, v);
    }

    if(rbl->type.of != TYPE_ROEBLING_BLANK){
        Roebling_ResetPatterns(rbl);
        rbl->parseIt.idx = 0;
    }

    rbl->type.state = (rbl->type.state & DEBUG);
    DebugStack_Pop();
    return SUCCESS;
}

status Roebling_AddStep(Roebling *rbl, Abstract *step){
    if(step->type.of == TYPE_WRAPPED_DO){
        Single *sg = rbl->parseIt.value;
        if(sg != NULL && sg->type.of == TYPE_WRAPPED_I64){
            i64 mark = sg->val.value;
            sg->val.value = rbl->parseIt.idx;
            Lookup_Add(rbl->m, rbl->marks, (word)sg->val.value, (Abstract *)sg);
            Type_SetFlag((Abstract *)&rbl->parseIt, SPAN_OP_SET);
            rbl->parseIt.value = step;
            return Iter_Query(&rbl->parseIt);
        }
    }

    Type_SetFlag((Abstract *)&rbl->parseIt, SPAN_OP_ADD);
    rbl->parseIt.value = step;
    void *args[] = {step, NULL};
    return Iter_Query(&rbl->parseIt);
}

status Roebling_Start(Roebling *rbl){
    Iter_Reset(&rbl->parseIt);
    return SUCCESS;
}

Roebling *Roebling_Make(MemCh *m,
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
    Span *p = Span_Make(m);
    Iter_Init(&rbl->parseIt, p);
    rbl->marks = Lookup_Make(m, _TYPE_CORE_END, NULL, (Abstract *)rbl); 
    Roebling_Reset(m, rbl, NULL);

    return rbl;
}
