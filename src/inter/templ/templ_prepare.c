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
    status latest = ((Fetcher *)self)->type.state;

    i32 targetCount = 1;
    while((Iter_Prev(&it) & END) == 0){
        Abstract *a = Iter_Get(&it);
        if(a->type.of == TYPE_FETCHER){
            Fetcher *prev = (Fetcher *)a;
            if(prev->type.state & (FETCHER_VAR|FETCHER_TEMPL)){
                continue;
            }else if(prev->type.state & FETCHER_END){
                targetCount++;
                latest = a->type.state;
            }else if((prev->type.state & flags) == 0){
                if(prev->type.state != latest && targetCount > 0){
                    --targetCount;
                }
                latest = prev->type.state;
            }else if(prev->type.state & flags){
                if(prev->type.state != latest && targetCount > 0){
                    --targetCount;
                }
                latest = prev->type.state;
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
            if(a->type.state & (FETCHER_VAR|FETCHER_TEMPL)){
                continue;
            }else if(a->type.state & flags){
                DebugStack_Pop();
                return it.idx; 
            }
        }
    }
    DebugStack_Pop();
    return -1;
}

static i32 Templ_FindPrev(Templ *templ, status flags){
    DebugStack_Push(templ, templ->type.of);
    Iter it;
    memcpy(&it, &templ->content, sizeof(Iter));
    while((Iter_Prev(&it) & END) == 0){
        Abstract *a = Iter_Get(&it);
        if(a->type.of == TYPE_FETCHER){
            Fetcher *fch = (Fetcher *)a;
            if(fch->type.state & flags){
                DebugStack_Pop();
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
    status latest = ((Fetcher *)self)->type.state;
    void *args[] = {self, NULL};

    i32 targetCount = 1;
    while((Iter_Next(&it) & END) == 0){
        Abstract *a = Iter_Get(&it);
        if(a->type.of == TYPE_FETCHER){
            if(a->type.state & (FETCHER_VAR|FETCHER_TEMPL)){
                continue;
            }else if((a->type.state & FETCHER_END) == 0){
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

TemplCrit *Templ_LastJumpAt(Templ *templ, i32 idx, i32 flagIdx){
    Jumps *js = (Jumps *)Span_Get(templ->jumps, idx);
    Abstract *a = NULL;
    if(js != NULL){
        a = (Abstract *)js->crit[flagIdx];
        if(a != NULL && a->type.of == TYPE_ITER){
            Iter *it = (Iter *)a;
            a = Span_Get(it->p, it->p->max_idx);
        }
    }
    if(a == NULL){
        Error(templ->m, FUNCNAME, FILENAME, LINENUMBER,
            "Last Jump is NULL", NULL);
        return NULL;
    }

    return (TemplCrit *)a;
}

status Templ_AddFunc(Templ *templ, i32 idx, TFunc func, status flags){
    MemCh *m = templ->m;
    TemplFunc *tfunc = TemplFunc_Make(m, func, flags);
    Span_Set(templ->funcs, idx, tfunc);
    return SUCCESS;
}

status Templ_AddJump(Templ *templ,
        i32 idx, i32 destIdx, i32 flagIdx, status flags){
    MemCh *m = templ->m;
    Jumps *js = Span_Get(templ->jumps, idx);
    if(js == NULL){
        js = Jumps_Make(m, idx);
        Span_Set(templ->jumps, idx, js);
    }
    if(destIdx != -1){
        TemplCrit *crit = TemplCrit_Make(m, destIdx, flags);
        Iter *it = (Iter *)Itin_GetByType(&templ->data, TYPE_ITER);
        if(it != NULL){
            crit->dataIdx = it->idx;
        }
        void *existing = js->crit[flagIdx];
        js->crit[flagIdx] = IterUpper_Combine(m, existing, crit);
    }
    word fl = ZERO;
    if(flagIdx > -1){
        word one = 1;
        fl |= (one << (flagIdx+8));
    }
    js->type.state |= (fl | (flags & (NOOP|MORE|UFLAG_ITER_REQUIRED)));
    return SUCCESS;
}

status Templ_PrepareCycle(Templ *templ){
    DebugStack_Push(templ, templ->type.of);
    status r = READY;
    MemCh *m = templ->m;
    if(Iter_Next(&templ->content) & END){
        templ->type.state |= PROCESSING;
        DebugStack_Pop();
        return templ->type.state;
    }

    Abstract *item = Iter_Get(&templ->content);
    if(templ->type.state & DEBUG){
        void *args[4];
        args[0] = NULL;
        args[1] = templ;
        args[2] = item;
        args[3] = NULL;
        Out("^c.^{STACK.name}: Templ:&\n    item:&^0\n", args);
    }

    if(item->type.of == TYPE_FETCHER){
        Fetcher *fch = (Fetcher*)item;
        i32 idx = templ->content.idx;

        if(fch->type.state & FETCHER_FOR){
            i32 skipIdx = -1;
            if((skipIdx = Templ_FindEnd(templ)) != -1){
                TFunc func = Lookup_Get(TemplFuncLookup, TYPE_ITER);
                r |= Templ_AddFunc(templ, idx, func, ZERO);
                r |= Templ_AddJump(templ, idx, skipIdx, UFLAG_ITER_SKIP_IDX, ZERO);

                Iter_GetByIdx(&templ->content, skipIdx);
                i32 finIdx = -1;
                if((finIdx = Templ_FindPrev(templ, FETCHER_END)) != -1){
                    if(finIdx != idx){ 
                        r |= Templ_AddJump(templ,
                            idx, finIdx, UFLAG_ITER_FINISH_IDX, MORE);
                        r |= Templ_AddJump(templ,
                            finIdx, finIdx+1, UFLAG_ITER_FINISH_IDX, MORE);
                        r |= Templ_AddJump(templ,
                            finIdx,
                            skipIdx,
                            UFLAG_ITER_SKIP_IDX,
                            UFLAG_ITER_REQUIRED);
                    }
                }
                Fetcher *end = Iter_Get(&templ->content);
                r |= Templ_AddJump(templ,
                    templ->content.idx, idx, UFLAG_ITER_ACTION_IDX, ZERO);

                Iter_GetByIdx(&templ->content, idx);
            }

            cls typeOfs = (FETCHER_CONDITION|FETCHER_FOR|FETCHER_WITH);
            i32 sibIdx = -1;
            if((sibIdx = Templ_FindNext(templ, typeOfs)) != -1){
                r |= Templ_AddJump(templ, idx, sibIdx, UFLAG_ITER_SIBLING_IDX, ZERO);
            }
        }else if(fch->type.state & (FETCHER_WITH|FETCHER_IF)){
            i32 skipIdx = -1;
            if((skipIdx = Templ_FindEnd(templ)) != -1){
                r |= Templ_AddJump(templ, idx, skipIdx, UFLAG_ITER_SKIP_IDX, ZERO);
            }
        }else if(fch->type.state & (FETCHER_CONDITION)){
            status skipFlags = ZERO;
            FetchTarget *tg = Span_Get(fch->val.targets, 0);
            i32 encloseIdx = -1;
            if((encloseIdx =
                    Templ_FindStart(templ, FETCHER_FOR|FETCHER_WITH)) != -1){
                r |= Templ_AddJump(templ,
                    idx, encloseIdx, UFLAG_ITER_ENCLOSE_IDX, ZERO);

                if(tg->objType.of == FORMAT_TEMPL_INDENT){
                    TFunc func = Lookup_Get(TemplFuncLookup, FORMAT_TEMPL_INDENT);
                    r |= Templ_AddFunc(templ, idx, func, ZERO);
                    skipFlags = UFLAG_ITER_FOCUS;
                }else{
                    TemplCrit *finish = Span_Get(templ->jumps, encloseIdx);
                    if(finish != NULL){
                        r |= Templ_AddJump(templ,
                            idx, finish->contentIdx, UFLAG_ITER_FINISH_IDX, MORE);
                    }
                }
            }

            if(tg->objType.of == FORMAT_TEMPL_LEVEL){
                /*
                skipFlags = (UFLAG_ITER_SKIP|UFLAG_ITER_LEAF);
                */
            }else if(tg->objType.of == FORMAT_TEMPL_CURRENT){
                /*
                skipFlags = (UFLAG_ITER_SKIP|UFLAG_ITER_FOCUS);
                */
            }else if(tg->objType.of == FORMAT_TEMPL_ACTIVE){
                /*
                skipFlags = (UFLAG_ITER_REQUIRED|UFLAG_ITER_LEAF);
                */
            }

            i32 skipIdx = -1;
            if((skipIdx =
                    Templ_FindNext(templ,
                        (FETCHER_CONDITION|FETCHER_END))) != -1){
                r |= Templ_AddJump(templ,
                    idx, skipIdx, UFLAG_ITER_SKIP_IDX, skipFlags);
            }
        }else if(fch->type.state & FETCHER_END){
            i32 sibIdx = Templ_FindStart(templ, ZERO);
            if(sibIdx > -1){
                Fetcher *dest = Span_Get(templ->content.p, sibIdx);
                if(dest != NULL){
                    if(dest->type.state & (FETCHER_CONDITION)){
                        r |= Templ_AddJump(templ,
                            idx, sibIdx, UFLAG_ITER_ENCLOSE_IDX, ZERO);
                        i32 outIdx = Templ_FindNext(templ, FETCHER_END);
                    }else{
                        fch->type.state |= NOOP;
                    }
                }
            }
        }

        if(r == READY && 
                (fch->type.state & (
                    NOOP|FETCHER_FOR|FETCHER_VAR|FETCHER_WITH)) == 0){
            void *args[3];
            args[0] = fch;
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
