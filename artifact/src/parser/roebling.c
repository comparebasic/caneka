#include <external.h>
#include <caneka.h>

static inline status Roebling_RunMatches(Roebling *rbl){
    DebugStack_Push(rbl, rbl->type.of);
    while((Cursor_NextByte(rbl->curs) & END) == 0){
        Guard_Incr(&rbl->guard, RBL_GUARD_MAX, FUNCNAME, FILENAME, LINENUMBER);

        Type_SetFlag((Abstract *)&rbl->matchIt, SPAN_OP_GET);
        Iter_Reset(&rbl->matchIt);
        if(rbl->curs->type.state & CURSOR_STR_BOUNDRY){
            while((Iter_Next(&rbl->matchIt) & END) == 0){
                Match *mt = rbl->matchIt.value;
                if(mt->type.state & PROCESSING){
                    Match_AddBoundrySnip(rbl->m, (Match *)rbl->matchIt.value);
                }
            }
            Iter_Reset(&rbl->matchIt);
        }

        byte c = *(rbl->curs->ptr);
        i32 noopCount = 0;
        while((Iter_Next(&rbl->matchIt) & END) == 0){
            Match *mt = (Match *)rbl->matchIt.value;
            DebugStack_SetRef(mt, mt->type.of);

            if((Match_Feed(rbl->m, mt, c) & SUCCESS) != 0){
                rbl->type.state |= ROEBLING_NEXT;
                rbl->type.state &= ~PROCESSING;

                if((mt->type.state & MATCH_SEARCH)){
                    Snip *sn = Span_Get(mt->backlog, 0);
                    if(sn != NULL && sn->length > 0 && sn->type.state & SNIP_SKIPPED){
                        i32 idx = rbl->matchIt.idx;
                        Iter_Reset(&rbl->matchIt);
                        while((Iter_Next(&rbl->matchIt) & END) == 0){
                            Match *omt = (Match *)rbl->matchIt.value;
                            if(omt != mt && (omt->type.state & MATCH_LAST_TERM)){
                                SnipSpan_Add(omt->backlog, &omt->snip);
                                i64 total = SnipSpan_Total(mt->backlog, ~SNIP_SKIPPED);
                                SnipSpan_Set(rbl->m, omt->backlog, total, SNIP_GAP);
                                StrVec *v = StrVec_Snip(rbl->m, omt->backlog, rbl->curs);
                                rbl->capture(rbl, omt->captureKey, v);
                                break;
                            }
                        }
                        Iter_Setup(&rbl->matchIt, rbl->matchIt.p, SPAN_OP_GET, idx);
                        Iter_Query(&rbl->matchIt);
                    }
                }

                StrVec *v = StrVec_Snip(rbl->m, mt->backlog, rbl->curs);
                rbl->capture(rbl, mt->captureKey, v);
                Snip *sn = Span_Get(mt->backlog, mt->backlog->max_idx);
                if((sn->type.state & SNIP_UNCLAIMED) != 0 && sn->length > 0){
                    Cursor_Decr(rbl->curs, sn->length);
                }

                Guard_Reset(&rbl->guard);
                DebugStack_Pop();
                return rbl->type.state;
            }

            if((mt->type.state & NOOP) != 0){
                if(++noopCount == rbl->matchIt.p->nvalues){
                    rbl->type.state |= (NOOP|END|ERROR);
                    Guard_Reset(&rbl->guard);
                    DebugStack_Pop();
                    return rbl->type.state;
                }
            }
            if(rbl->type.state & DEBUG){
                Abstract *args[] = {
                    (Abstract *)Str_Ref(OutStream->m, &c, 1, 1, DEBUG),
                    (Abstract *)rbl,
                    NULL
                };
                Out("^p.Matche Run(^D.$^d.): @^0.\n", args);
            }
        }
    }

    rbl->type.state |= (rbl->curs->type.state & END);
    Guard_Reset(&rbl->guard);
    DebugStack_Pop();
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
        Match *mt = rbl->matchIt.value;
        rbl->tail = 0;
        if(mt != NULL && (mt->type.state & MATCH_JUMP)){
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
    rbl->parseIt.idx = sg->val.w;
    return rbl->type.state;
}

status Roebling_Run(Roebling *rbl){
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

status Roebling_SetPattern(Roebling *rbl, PatCharDef *def, word captureKey, i16 jump){
    Match *mt = Match_Make(rbl->m, def, NULL);
    mt->captureKey = captureKey;
    if(jump != -1){
        mt->jump = Roebling_GetMarkIdx(rbl, jump);
        mt->type.state |= MATCH_JUMP;
        if(mt->jump == -1){
            Abstract *args[] = {
                (Abstract *)I16_Wrapped(rbl->m, jump),
                (Abstract *)rbl,
                NULL,
            };
            Fatal(FUNCNAME, FILENAME, LINENUMBER, "Jump not found in marks, jump $ not found in @\n", args); 
            return ERROR;
        }
    }

    Type_SetFlag((Abstract *)&rbl->matchIt, SPAN_OP_ADD);
    rbl->matchIt.value = mt;
    return Iter_Query(&rbl->matchIt);
}

i64 Roebling_GetMarkIdx(Roebling *rbl, i32 mark){
    Single *sg = (Single *)Lookup_Get(rbl->marks, mark); 
    if(sg != NULL){
        return sg->val.w;
    }
    return -1; 
}

status Roebling_Reset(MemCh *m, Roebling *rbl, StrVec *v){
    DebugStack_Push("Roebling_Reset", TYPE_CSTR); 
    Roebling_ResetPatterns(rbl);
    if(v != NULL){
        Cursor_Setup(rbl->curs, v);
    }

    rbl->type.state = (rbl->type.state & DEBUG);
    DebugStack_Pop();
    return SUCCESS;
}

status Roebling_AddMark(Roebling *rbl, Single *sg){
    i64 mark = sg->val.w;
    sg->val.w = rbl->parseIt.idx;
    return Lookup_Add(rbl->m, rbl->marks, mark, (Abstract *)sg);
}

status Roebling_AddStep(Roebling *rbl, Abstract *step){
    word fl = SPAN_OP_ADD;
    if(step->type.of == TYPE_WRAPPED_DO){
        Single *sg = rbl->parseIt.value;
        if(sg != NULL && sg->type.of == TYPE_WRAPPED_I16){
            Roebling_AddMark(rbl, sg);
            fl = SPAN_OP_SET;
        }
    }else if(step->type.of != TYPE_WRAPPED_I16){
        Abstract *args[] = {
            (Abstract *)step,
            NULL
        };
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Expected I16 or Do, have $", args);
        return ERROR;
    }

    Type_SetFlag((Abstract *)&rbl->parseIt, fl);
    rbl->parseIt.value = step;
    return Iter_Query(&rbl->parseIt);
}

status Roebling_Start(Roebling *rbl){
    Single *sg = Span_Get(rbl->parseIt.p, rbl->parseIt.p->max_idx);
    if(sg != NULL && sg->type.of == TYPE_WRAPPED_I16){
        Roebling_AddMark(rbl, sg);
        Span_Set(rbl->parseIt.p, rbl->parseIt.p->max_idx, NULL);
    }
    Iter_Reset(&rbl->parseIt);
    if(rbl->type.state & DEBUG){
        Abstract *args[] = {
            (Abstract *)rbl,
            NULL
        };
        Out("^p.Start @^0.\n", args);
    }
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
