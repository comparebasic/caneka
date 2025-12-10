#include <external.h>
#include <caneka.h>

static inline status Roebling_RunMatches(Roebling *rbl){
    DebugStack_Push(rbl, rbl->type.of);
    while((Cursor_NextByte(rbl->curs) & END) == 0){
        Guard_Incr(rbl->m, &rbl->guard, RBL_GUARD_MAX, FUNCNAME, FILENAME, LINENUMBER);

        Type_SetFlag((Abstract *)&rbl->matchIt, SPAN_OP_GET);
        Iter_Reset(&rbl->matchIt);
        if(rbl->curs->type.state & CURSOR_STR_BOUNDRY){
            while((Iter_Next(&rbl->matchIt) & END) == 0){
                Match *mt = Iter_Current(&rbl->matchIt);
                if(mt->type.state & PROCESSING){
                    Match_AddBoundrySnip(rbl->m, (Match *)Iter_Current(&rbl->matchIt));
                }
            }
            Iter_Reset(&rbl->matchIt);
        }

        byte c = *(rbl->curs->ptr);
        /*
        if(rbl->type.state & DEBUG){
            void *args[] = {
                Str_Ref(rbl->m, &c, 1, 1, DEBUG),
                NULL
            };
            Out("^pE. $ ^0.\n", args);
        }
        */
        i32 noopCount = 0;
        while((Iter_Next(&rbl->matchIt) & END) == 0){
            Match *mt = (Match *)Iter_Current(&rbl->matchIt);
            DebugStack_SetRef(mt, mt->type.of);

            status success = Match_Feed(rbl->m, mt, c) & SUCCESS;
            /*
            if(rbl->type.state & DEBUG){
                void *args[] = {
                    Str_Ref(rbl->m, &c, 1, 1, DEBUG),
                    mt,
                    NULL
                };
                Out("^p.$ - &^0.\n", args);
            }
            */
            if(success){
                DebugStack_Pop();
                return Roebling_Dispatch(rbl, mt);
            }

            if((mt->type.state & NOOP) != 0){
                if(++noopCount == rbl->matchIt.p->nvalues){
                    rbl->type.state |= (NOOP|END|ERROR);
                    Guard_Reset(&rbl->guard);

                    DebugStack_Pop();
                    if(mt->type.state & DEBUG){
                        void *args[] = {
                            Str_Ref(rbl->m, &c, 1, 1, DEBUG),
                            rbl,
                            NULL
                        };
                        Out("^p.Match Run -> ALL NOOPs (^D.$^d.): @^0.\n", args);
                    }
                    return rbl->type.state;
                }
            }
            if(mt->type.state & DEBUG){
                void *args[] = {
                    Str_Ref(rbl->m, &c, 1, 1, DEBUG),
                    rbl,
                    NULL
                };
                Out("^p.Match Run(^D.$^d.): @^0.\n", args);
            }
        }
    }

    rbl->type.state |= (rbl->curs->type.state & END);
    Guard_Reset(&rbl->guard);

    DebugStack_Pop();
    return rbl->type.state;
}

status Roebling_Finalize(Roebling *rbl, Match *mt, i64 total){
    i32 idx = rbl->matchIt.idx;
    Iter_Reset(&rbl->matchIt);
    while((Iter_Next(&rbl->matchIt) & END) == 0){
        Match *omt = (Match *)Iter_Current(&rbl->matchIt);
        if(omt != mt && (omt->type.state & MATCH_LAST_TERM)){
            SnipSpan_Add(omt->backlog, &omt->snip);
            if(total > 0){
                SnipSpan_Set(rbl->m, omt->backlog, total, SNIP_GAP);
            }
            StrVec *v = StrVec_Snip(rbl->m, omt->backlog, rbl->curs);

            if(rbl->type.state & DEBUG){
                void *args[] = {
                    Type_ToStr(rbl->m,
                        omt->captureKey),
                    v,
                    NULL
                };
                Out("^c.RblCapture(^D.$^d.): &^0.\n", args);
            }

            rbl->capture(rbl, omt->captureKey, v);
            omt->type.state &= ~MATCH_LAST_TERM;
            break;
        }
    }
    Iter_GetByIdx(&rbl->matchIt, idx);
    return SUCCESS;
}

status Roebling_Dispatch(Roebling *rbl, Match *mt){
    rbl->type.state |= ROEBLING_NEXT;
    rbl->type.state &= ~PROCESSING;

    if(mt->type.state & DEBUG){
        void *args[] = {
            mt,
            NULL
        };
        Out("^y.Match successful &^0.\n", args);
    }

    if((mt->type.state & MATCH_SEARCH)){
        Snip *sn = Span_Get(mt->backlog, 0);
        if(sn->length > 0 && sn->type.state & SNIP_SKIPPED){
            i64 total = SnipSpan_Total(mt->backlog, ~SNIP_SKIPPED);
            Roebling_Finalize(rbl, mt, total);
        }
    }

    StrVec *v = StrVec_Snip(rbl->m, mt->backlog, rbl->curs);
    if(rbl->type.state & DEBUG){
        byte c = *(rbl->curs->ptr);
        void *args[] = {
            Str_Ref(rbl->m, &c, 1, 1, DEBUG),
            Type_ToStr(rbl->m,
                mt->captureKey),
            v,
            mt,
            NULL
        };
        Out("^c.RblCapture on (^D.$^d.) (^D.$^d.): &\n&^0.\n", args);
    }

    rbl->capture(rbl, mt->captureKey, v);
    Snip *sn = Span_Get(mt->backlog, mt->backlog->max_idx);
    if((sn->type.state & SNIP_UNCLAIMED) != 0 && sn->length > 0){
        Cursor_Decr(rbl->curs, sn->length);
    }

    Guard_Reset(&rbl->guard);

    return rbl->type.state;
}

status Roebling_RunCycle(Roebling *rbl){
    DebugStack_Push(rbl, rbl->type.of);
    if(rbl->parseIt.p->nvalues == 0){
        Fatal(FUNCNAME, FILENAME, LINENUMBER,
            "Roebling parsers not set", NULL);
    }
    rbl->type.state &= ~END;
    if((rbl->type.state & ROEBLING_NEXT) != 0){
        Match *mt = Iter_Current(&rbl->matchIt);
        rbl->tail = 0;
        if(mt != NULL && (mt->type.state & MATCH_JUMP)){
            rbl->parseIt.idx = mt->jump;
        }else{
            rbl->parseIt.idx++;
        }
        rbl->type.state &= ~ROEBLING_NEXT;
    }

    Single *wdof = Iter_GetByIdx(&rbl->parseIt, rbl->parseIt.idx);
    if(wdof == NULL){
        if((rbl->type.state & ROEBLING_REPEAT) != 0){
            rbl->type.state |= ROEBLING_NEXT;
        }else{
            rbl->type.state = SUCCESS;
        }
    }else{
        if((rbl->type.state & PROCESSING) == 0){
            wdof = (Single *)as(wdof, TYPE_WRAPPED_DO);
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
    rbl->parseIt.idx = sg->val.w;
    return rbl->type.state;
}

status Roebling_Run(Roebling *rbl){
    if(rbl->type.state & DEBUG){
        void *args[2] = {
            rbl,
            NULL
        };
        Out("^p.Roebling_Run &\n", args);
    }
    if((rbl->curs->type.state & END) != 0){
        rbl->type.state |= END;
    }
    status r = rbl->type.state;
    while((r & (SUCCESS|ERROR|END|NOOP)) == 0){
        r = Roebling_RunCycle(rbl);
    }
    rbl->type.state &= ~END;
    return rbl->type.state;
}

Match *Roebling_GetMatch(Roebling *rbl){
    return Iter_Current(&rbl->matchIt);
}

i32 Roebling_GetMatchIdx(Roebling *rbl){
    return rbl->matchIt.metrics.selected;
}

status Roebling_ResetPatterns(Roebling *rbl){
    MemCh_FreeTemp(rbl->m);

    Span *p = Span_Make(rbl->m);
    Iter_Init(&rbl->matchIt, p);

    Guard_Reset(&rbl->guard);
    return READY;
}

status Roebling_SetPattern(Roebling *rbl, PatCharDef *def, word captureKey, i16 jump){
    Match *mt = Match_Make(rbl->m, def, NULL);
    mt->captureKey = captureKey;
    if(jump != -1){
        mt->jump = Roebling_GetMarkIdx(rbl, jump);
        mt->type.state |= MATCH_JUMP;
        if(mt->jump == -1){
            void *args[] = {
                I16_Wrapped(rbl->m, jump),
                rbl,
                NULL,
            };
            Fatal(FUNCNAME, FILENAME, LINENUMBER, "Jump not found in marks, jump $ not found in @\n", args); 
            return ERROR;
        }
    }

    return Iter_Add(&rbl->matchIt, mt);
}

i64 Roebling_GetMarkIdx(Roebling *rbl, i32 mark){
    Single *sg = (Single *)Lookup_Get(rbl->marks, mark); 
    if(sg != NULL){
        return sg->val.w;
    }
    return -1; 
}

status Roebling_Reset(MemCh *m, Roebling *rbl, StrVec *v){
    Roebling_ResetPatterns(rbl);
    if(v != NULL){
        if(rbl->curs == NULL){
            rbl->curs = Cursor_Make(m, v);
        }else{
            Cursor_Setup(rbl->curs, v);
        }
    }
    rbl->type.state = (rbl->type.state & DEBUG);
    return SUCCESS;
}

status Roebling_AddMark(Roebling *rbl, Single *sg){
    i64 mark = sg->val.w;
    sg->val.w = rbl->parseIt.idx;
    return Lookup_Add(rbl->m, rbl->marks, mark, sg);
}

status Roebling_AddStep(Roebling *rbl, void *_step){
    Abstract *step = (Abstract *)_step;
    word fl = SPAN_OP_ADD;
    if(step->type.of == TYPE_WRAPPED_DO){
        Single *sg = (Single *)Iter_Current(&rbl->parseIt);
        if(sg != NULL && sg->type.of == TYPE_WRAPPED_I16){
            Roebling_AddMark(rbl, sg);
            fl = SPAN_OP_SET;
        }
    }else if(step->type.of != TYPE_WRAPPED_I16){
        void *args[] = {
            step,
            NULL
        };
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Expected I16 or Do, have $", args);
        return ERROR;
    }

    if(fl == SPAN_OP_ADD){
        return Iter_Add(&rbl->parseIt, step);
    }else{
        return Iter_SetByIdx(&rbl->parseIt, rbl->parseIt.idx, step);
    }
}

status Roebling_Start(Roebling *rbl){
    Single *sg = Span_Get(rbl->parseIt.p, rbl->parseIt.p->max_idx);
    if(sg != NULL && sg->type.of == TYPE_WRAPPED_I16){
        Roebling_AddMark(rbl, sg);
        Span_Set(rbl->parseIt.p, rbl->parseIt.p->max_idx, NULL);
    }
    Iter_Reset(&rbl->parseIt);
    if(rbl->type.state & DEBUG){
        void *args[] = {
            rbl,
            NULL
        };
        Out("^p.Start @^0.\n", args);
    }
    return SUCCESS;
}

Roebling *Roebling_Make(MemCh *m, Cursor *curs, RblCaptureFunc capture, void *source){

    Roebling *rbl = (Roebling *)MemCh_Alloc(m, sizeof(Roebling));
    rbl->type.of = TYPE_ROEBLING;
    rbl->m = m;
    rbl->source = source;
    rbl->capture = capture;
    rbl->curs = curs;
    Span *p = Span_Make(m);
    Iter_Init(&rbl->parseIt, p);
    rbl->marks = Lookup_Make(m, _TYPE_CORE_END); 
    if(curs != NULL && (curs->type.state & PROCESSING) == 0){
        Cursor_Setup(rbl->curs, curs->v);
    }

    return rbl;
}
