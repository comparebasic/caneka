#include <external.h>
#include <caneka.h>

static i32 Templ_FindStart(Templ *templ, word flags){
    DebugStack_Push(templ, templ->type.of);
    Iter it;
    memcpy(&it, &templ->content, sizeof(Iter));

    if(flags == ZERO){
        flags = (
            FETCHER_WITH|FETCHER_FOR|FETCHER_IF|FETCHER_IFNOT|FETCHER_CONDITION);
    }

    Abstract *self = Iter_Get(&it);
    status latest = ((TemplJump *)self)->fch->type.state;

    i32 targetCount = 1;
    while((Iter_Prev(&it) & END) == 0){
        Abstract *a = Iter_Get(&it);
        if(a->type.of == TYPE_TEMPL_JUMP){
            TemplJump *prevJump = (TemplJump *)a;
            if(prevJump->fch->type.state & FETCHER_VAR){
                continue;
            }else if(prevJump->fch->type.state & FETCHER_END){
                targetCount++;
                latest = a->type.state;
            }else if((prevJump->fch->type.state & flags) == 0){
                if(prevJump->fch->type.state != latest && targetCount > 0){
                    --targetCount;
                }
                latest = prevJump->fch->type.state;
            }else if(prevJump->fch->type.state & flags){
                if(prevJump->fch->type.state != latest && targetCount > 0){
                    --targetCount;
                }
                latest = prevJump->fch->type.state;
                if(targetCount == 0){
                    DebugStack_Pop();
                    return it.idx;
                }
            }
        }
    }
    DebugStack_Pop();
    return -1;
}

static i32 Templ_FindNext(Templ *templ, status flags){
    DebugStack_Push(templ, templ->type.of);
    Iter it;
    memcpy(&it, &templ->content, sizeof(Iter));

    while((Iter_Next(&it) & END) == 0){
        Abstract *a = Iter_Get(&it);
        if(a->type.of == TYPE_FETCHER){
            if(a->type.state & FETCHER_VAR){
                continue;
            }else if(a->type.state & flags){
                return it.idx; 
            }
        }
    }
    DebugStack_Pop();
    return -1;
}


static i32 Templ_FindEnd(Templ *templ){
    DebugStack_Push(templ, templ->type.of);
    Iter it;
    memcpy(&it, &templ->content, sizeof(Iter));


    Abstract *self = Iter_Get(&it);
    status latest = ((TemplJump *)self)->fch->type.state;
    void *args[] = {self, NULL};

    i32 targetCount = 1;
    while((Iter_Next(&it) & END) == 0){
        Abstract *a = Iter_Get(&it);

        if(a->type.of == TYPE_FETCHER){
            if(a->type.state & FETCHER_VAR){
                continue;
            }else if((a->type.state & FETCHER_END) == 0){
                void *args[] = {a, I32_Wrapped(templ->m, targetCount), NULL};
                if(a->type.state != latest){
                    targetCount++;
                }
                latest = a->type.state;
            }else if((a->type.state & FETCHER_END) && --targetCount == 0){
                DebugStack_Pop();
                return it.idx; 
            }
        }
    }
    DebugStack_Pop();
    return -1;
}

status Templ_PrepareCycle(Templ *templ){
    DebugStack_Push(templ, templ->type.of);
    void *args[5];
    MemCh *m = templ->m;
    if(Iter_Next(&templ->content) & END){
        templ->type.state |= PROCESSING;
        DebugStack_Pop();
        return templ->type.state;
    }

    Abstract *item = Iter_Get(&templ->content);
    if(templ->type.state & DEBUG){
        args[0] = NULL;
        args[1] = templ;
        args[2] = item;
        args[3] = NULL;
        Out("^c.^{STACK.name}: Templ:&\n    item:&^0\n", args);
    }

    word unregJumpFl = (FETCHER_FOR|FETCHER_CONDITION|FETCHER_END|FETCHER_WITH|FETCHER_IF);
    if(item->type.of == TYPE_FETCHER && (((Fetcher *)item)->type.state & unregJumpFl)){
        Fetcher *fch = (Fetcher*)item;

        i32 idx = templ->content.idx;
        TemplJump *jump = TemplJump_Make(m, idx, fch);
        Span_Set(templ->content.p, idx, jump);
        Iter_GetByIdx(&templ->content, idx);

        if(fch->type.state & FETCHER_FOR){
            jump->crit.skip.idx = Templ_FindEnd(templ);
            jump->crit.dest.idx = Templ_FindNext(templ, FETCHER_CONDITION|FETCHER_END);
        }else if(fch->type.state & (FETCHER_WITH|FETCHER_IF)){
            jump->crit.skip.idx = Templ_FindEnd(templ); 
        }else if((fch->type.state & (FETCHER_CONDITION|FETCHER_TEMPL)) == 
                (FETCHER_CONDITION|FETCHER_TEMPL)){
            jump->crit.skip.idx = Templ_FindEnd(templ);
            jump->crit.dest.idx = Templ_FindStart(templ, ZERO);
        }else if(fch->type.state & (FETCHER_CONDITION)){
            FetchTarget *tg = Span_Get(jump->fch->val.targets, 0); 
            jump->crit.dest.idx = Templ_FindNext(templ, FETCHER_END);
            jump->crit.skip.idx = Templ_FindNext(templ, (FETCHER_CONDITION|FETCHER_END));
            i32 enclosingIdx = Templ_FindStart(templ, FETCHER_FOR);
            if(enclosingIdx != -1){
                TemplJump *enclosing = Span_Get(templ->content.p, enclosingIdx);
                if(enclosing != NULL){
                    enclosing->crit.dest.type.state = UFLAG_ITER_INDENT;
                }
            }
        }else if(fch->type.state & FETCHER_END){
            i32 destIdx = Templ_FindStart(templ, ZERO);
            if(destIdx > -1){
                TemplJump *dest = Span_Get(templ->content.p, destIdx);
                jump->sourceType.state = dest->fch->type.state;
                if(dest != NULL){
                    if(dest->fch->type.state & (FETCHER_WITH|FETCHER_FOR)){
                        jump->crit.dest.idx = destIdx;
                    }else if(dest->fch->type.state & (FETCHER_CONDITION)){
                        jump->crit.skip.idx = Templ_FindEnd(templ);
                        jump->crit.skip.type.state = 
                            (UFLAG_ITER_OUTDENT|UFLAG_ITER_INVERT);
                    }else{
                        jump->fch->type.state |= NOOP;
                    }
                }
            }
        }

        if((jump->fch->type.state & (NOOP|FETCHER_FOR|FETCHER_WITH)) == 0 &&
                jump->crit.dest.idx == NEGATIVE && jump->crit.skip.idx == NEGATIVE
            ){
            args[0] = jump;
            args[1] = I32_Wrapped(m, templ->content.idx);
            args[2] = NULL;
            Error(m, FUNCNAME, FILENAME, LINENUMBER, 
                "Unable to find source or ending for Jump in Templ content: @ \\@$", 
                args);

            templ->type.state |= ERROR;
            DebugStack_Pop();
            return templ->type.state;
        }
    }

    DebugStack_Pop();
    return templ->type.state;
}

status Templ_Prepare(Templ *templ){
    DebugStack_Push(templ, templ->type.of);

    if((templ->type.state & PROCESSING) == 0){
        i16 g = 0;
        templ->type.state &= ~(ERROR|SUCCESS);
        while((Templ_PrepareCycle(templ) & (ERROR|PROCESSING)) == 0){
            Guard_Incr(templ->m, &g, 64, FUNCNAME, FILENAME, LINENUMBER);
        }
        if((templ->type.state & PROCESSING) == 0){
            void *args[] = {
                templ,
                NULL
            };
            Error(templ->m, FUNCNAME, FILENAME, LINENUMBER,
                "Error preparing Templ, &", args);
            DebugStack_Pop();
            return ERROR;
        }
        DebugStack_Pop();
        return (templ->type.state|SUCCESS);
    }
    DebugStack_Pop();
    return NOOP;
}

