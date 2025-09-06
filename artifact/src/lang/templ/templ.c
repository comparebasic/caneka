#include <external.h>
#include <caneka.h>

static i32 Templ_FindStart(Templ *templ, word flags){
    Iter it;
    memcpy(&it, &templ->content, sizeof(Iter));

    i32 targetIdx = templ->content.p->max_idx;
    while((Iter_Prev(&it) & END) == 0){
        if(it.idx > targetIdx){
            continue;
        }

        Abstract *a = Iter_Get(&it);

        if(a->type.of == TYPE_TEMPL_JUMP){
            TemplJump *prevJump = (TemplJump *)a;
            if(prevJump->fch->type.state & FETCHER_END){
                targetIdx = prevJump->destIdx;
            }else if(prevJump->fch->type.state & 
                    (FETCHER_WITH|FETCHER_FOR|FETCHER_IF|FETCHER_IFNOT) &&
                    flags == ZERO || (prevJump->fch->type.state & flags)){
                return it.idx;
            }
        }
    }
    return -1;
}

static i32 Templ_FindEnd(Templ *templ){
    Iter it;
    memcpy(&it, &templ->content, sizeof(Iter));

    i32 targetCount = 1;
    while((Iter_Next(&it) & END) == 0){
        Abstract *a = Iter_Get(&it);


        if(a->type.of == TYPE_FETCHER){
            if((a->type.state & FETCHER_END|FETCHER_VAR) == 0){
                targetCount++;
            }else if((a->type.state & FETCHER_END) && --targetCount == 0){
                return it.idx; 
            }
        }

        /*
        Abstract *args[] = {
            (Abstract *)a,
            (Abstract *)I32_Wrapped(OutStream->m, targetCount),
            (Abstract *)I32_Wrapped(OutStream->m, it.idx),
            NULL
        };

        Out("^c.  SearchEnd @ target:$\\@$^0.\n", args);
        */
    }
    return -1;
}

static status Templ_handleJump(Templ *templ){
    status r = READY;
    TemplJump *jump = (TemplJump *)Iter_Current(&templ->content);
    Abstract *data = Iter_Current(&templ->data);

    Fetcher *fch = jump->fch;
    i32 skipIdx = -1;
    if(fch->type.state & FETCHER_END){
        skipIdx = templ->content.idx;
        TemplJump *dest = (TemplJump *)Span_Get(templ->content.p, jump->destIdx);
        if(dest->fch->type.state & FETCHER_WITH){
            Iter_PrevRemove(&templ->data);
        }
        if(dest->fch->type.state & FETCHER_FOR){
            Iter_GetByIdx(&templ->content, jump->destIdx);
            jump = (TemplJump *)Iter_Get(&templ->content);
            fch = jump->fch;
        }
    }

    if(templ->type.state & DEBUG){
        Abstract *args[] = {
            (Abstract *)jump,
            (Abstract *)fch,
            NULL
        };
        Out("^b.  Jump: & of &^0.\n", args);
    }

    if(fch->type.state & FETCHER_FOR){
        Iter_PrevRemove(&templ->data);
        Iter *it = (Iter *)as(Iter_Get(&templ->data), TYPE_ITER);
        if((Iter_Next(it) & END) == 0){
            Iter_Add(&templ->data, (Abstract *)Iter_Get(it));
        }else if(jump->skipIdx != NEGATIVE){
            Iter_GetByIdx(&templ->content, jump->skipIdx);
        }else if(skipIdx != NEGATIVE){
            Iter_GetByIdx(&templ->content, skipIdx);
        }
        r |= PROCESSING;
    }else if(fch->type.state & FETCHER_WITH){
        if(templ->type.state & DEBUG){
            Abstract *args[] = {
                (Abstract *)fch,
                (Abstract *)data,
                NULL
            };
            Out("^c.  With: & of @^0.\n", args);
        }
        Abstract *value = Fetch(templ->m, fch, data, NULL);
        Iter_Add(&templ->data, (Abstract *)value);
        r |= PROCESSING;
    }else if(fch->type.state & FETCHER_COMMAND){
        if(templ->type.state & DEBUG){
            Abstract *args[] = {
                (Abstract *)fch,
                NULL
            };
            Out("^c.  Command: &^0.\n", args);
        }
        if(fch->val.targets->nvalues == 0){
            Iter_GetByIdx(&templ->content, jump->destIdx);
            r |= PROCESSING;
        }else{
            FetchTarget *tg = Span_Get(fch->val.targets, 0);
            if(
                (tg->idx == jump->level && tg->objType.of == FORMAT_TEMPL_LEVEL) ||
                (tg->idx == jump->depth && tg->objType.of == FORMAT_TEMPL_LEVEL_ITEM) ||
                (tg->objType.of == FORMAT_TEMPL_LEVEL_BETWEEN && jump->level < jump->depth)
                ){
                /* proceed */ 
                Abstract *args[] = {
                    (Abstract *)tg,
                    (Abstract *)I32_Wrapped(OutStream->m, tg->idx),
                    (Abstract *)I32_Wrapped(OutStream->m, jump->level),
                    (Abstract *)I32_Wrapped(OutStream->m, jump->depth),
                    NULL
                };
                Out("^c.      Proceed: & tg$ level$ depth$^0.\n", args);
            }else if(tg->objType.of == FORMAT_TEMPL_LEVEL_NEST){
                Iter *it = (Iter *)Iter_Get(&templ->data);
                if(it != NULL && it->idx < it->p->max_idx){
                    Abstract *args[] = {
                        (Abstract *)tg,
                        NULL
                    };
                    Out("^c.      Nest: &^0.\n", args);
                    Iter_Add(&templ->data, (Abstract *)Iter_Get(it));
                    r |= PROCESSING;
                }else{
                    Abstract *args[] = {
                        (Abstract *)tg,
                        NULL
                    };
                    Out("^c.      SkipNest: &^0.\n", args);
                    Iter_GetByIdx(&templ->content, jump->skipIdx);
                    r |= PROCESSING;
                }
            }else{
                Abstract *args[] = {
                    (Abstract *)tg,
                    NULL
                };
                Out("^c.      Skipping: &^0.\n", args);

                Iter_GetByIdx(&templ->content, jump->skipIdx);
                r |= PROCESSING;
            }
        }
    }

    if(r == READY){
        r |= NOOP;
    }

    return r;
}

static status Templ_PrepareCycle(Templ *templ){
    MemCh *m = templ->m;
    if(Iter_Next(&templ->content) & END){
        templ->type.state |= PROCESSING;
        return templ->type.state;
    }

    Abstract *item = Iter_Get(&templ->content);

    word unregJumpFl = (FETCHER_FOR|FETCHER_COMMAND|FETCHER_END|FETCHER_WITH);
    if(item->type.of == TYPE_FETCHER && (((Fetcher *)item)->type.state & unregJumpFl)){
        Fetcher *fch = (Fetcher*)item;

        i32 idx = templ->content.idx;
        TemplJump *jump = TemplJump_Make(m, idx, fch);
        Span_Set(templ->content.p, idx, (Abstract *)jump);
        Iter_GetByIdx(&templ->content, idx);

        if(fch->type.state & FETCHER_FOR){
            jump->level = 0;
            jump->skipIdx = Templ_FindEnd(templ);
        }else if(fch->type.state & FETCHER_WITH){
            jump->skipIdx = Templ_FindEnd(templ); 
        }else if((fch->type.state & (FETCHER_COMMAND|FETCHER_TEMPL)) == 
                (FETCHER_COMMAND|FETCHER_TEMPL)){
            jump->skipIdx = Templ_FindEnd(templ);
        }else if(fch->type.state & (FETCHER_COMMAND)){
            jump->destIdx = Templ_FindStart(templ, FETCHER_FOR); 
            i32 idx = templ->content.idx;
            Iter_GetByIdx(&templ->content, jump->destIdx);
            jump->skipIdx = Templ_FindEnd(templ); 
            Iter_GetByIdx(&templ->content, idx);
        }else if(fch->type.state & FETCHER_END){
            jump->destIdx = Templ_FindStart(templ, ZERO);
        }

        if((jump->fch->type.state & (FETCHER_FOR|FETCHER_WITH)) == 0 &&
                jump->destIdx == NEGATIVE && jump->skipIdx == NEGATIVE &&
                jump->tempIdx == NEGATIVE){
            Error(ErrStream->m, (Abstract *)templ, FUNCNAME, FILENAME, LINENUMBER, 
                "Unable to find source or ending for Jump in Templ content\n", NULL);

            templ->type.state |= ERROR;
            return templ->type.state;
        }

        if(templ->type.state & DEBUG){
            Abstract *args[] = {
                (Abstract *)jump,
                NULL,
            };
            Out("^c.  Making a Jump &^0.\n", args);
        }
    }


    if(templ->type.state & DEBUG){
        Abstract *args[] = {
            (Abstract *)Iter_Current(&templ->content),
            NULL
        };
        Out("^y.Templ_Prepare\n  ^E.content:^e.&^0.\n", args);
    }

    return templ->type.state;
}

i64 Templ_ToSCycle(Templ *templ, Stream *sm, i64 total, Abstract *source){
    if(Iter_Next(&templ->content) & END){
        templ->type.state |= SUCCESS;
        return total;
    }

    Abstract *item = Iter_Get(&templ->content);

    if(item->type.of == TYPE_TEMPL_JUMP){
        if(Templ_handleJump(templ) & (PROCESSING|SUCCESS)){
            item = Iter_Get(&templ->content);
        }
    }

    Abstract *data = Iter_Current(&templ->data);

    if(item->type.of == TYPE_STRVEC){
        total += ToS(sm, (Abstract *)item, 0, ZERO); 
    }else if(item->type.of == TYPE_FETCHER && item->type.state & FETCHER_VAR){
        Fetcher *fch = (Fetcher *)item;
        if(templ->type.state & DEBUG){
            Abstract *args[] = {
                (Abstract *)fch,
                NULL
            };
            Out("^c.  Fetcher: &^0.\n", args);
        }

        Abstract *value = Fetch(templ->m, fch, data, NULL);
        if(value == NULL){
            Abstract *args[] = {
                (Abstract *)fch,
                (Abstract *)data,
                NULL,
            };
            Error(sm->m, (Abstract *)templ, FUNCNAME, FILENAME, LINENUMBER,
                "Error finding value using @ in data @\n",args);
            return total;
        }
        if(templ->type.state & DEBUG){
            Abstract *args[] = {
                (Abstract *)value,
                NULL
            };
            Out("^c.  VarValue: &^0.\n", args);
        }
        total += ToS(sm, (Abstract *)value, 0, ZERO); 
    }

    if(templ->content.type.state & END){
        templ->type.state |= SUCCESS;
    }

    if(templ->type.state & DEBUG){
        Abstract *args[] = {
            (Abstract *)sm->dest.curs->v,
            NULL
        };
        Out("^gE.>> Out:^e. @^0.\n", args);
    }

    return total;
}

status Templ_Prepare(Templ *templ){
    if((templ->type.state & PROCESSING) == 0){
        i16 g = 0;
        templ->type.state &= ~(ERROR|SUCCESS);
        while((Templ_PrepareCycle(templ) & (ERROR|PROCESSING)) == 0){
            Guard_Incr(&g, 64, FUNCNAME, FILENAME, LINENUMBER);
        }
        if((templ->type.state & PROCESSING) == 0){
            Abstract *args[] = {
                (Abstract *)templ,
                NULL
            };
            Error(ErrStream->m, (Abstract *)templ, FUNCNAME, FILENAME, LINENUMBER,
                "Error preparing Templ, &", args);
            return ERROR;
        }
        return (templ->type.state|SUCCESS);
    }
    return NOOP;
}

i64 Templ_ToS(Templ *templ, Stream *sm, Abstract *data, Abstract *source){
    i64 total = 0;
    i16 g = 0;

    if(Templ_Prepare(templ) & ERROR){
        return 0;
    }

    Span *p = Span_Make(sm->m);
    Span_Add(p, data);
    Iter_Init(&templ->data, p);
    Iter_Next(&templ->data);
    while((total = Templ_ToSCycle(templ, sm, total, source)) && 
        (templ->type.state & OUTCOME_FLAGS) == 0){
        Guard_Incr(&g, 64, FUNCNAME, FILENAME, LINENUMBER);
    }
    return total;
}

Templ *Templ_Make(MemCh *m, Span *content){
    Templ *templ = (Templ *)MemCh_Alloc(m, sizeof(Templ));
    templ->type.of = TYPE_TEMPL;
    templ->m = m;
    Iter_Init(&templ->content, content);
    return templ;
}
