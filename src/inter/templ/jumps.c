#include <external.h>
#include <caneka.h>

static status Jump_NextSetFl =
    UFLAG_ITER_FOCUS|UFLAG_ITER_SIBLING|UFLAG_ITER_LEAF;

status Templ_HandleJump(Templ *templ){
    status r = READY;
    DebugStack_Push(templ, templ->type.of);
    MemCh *m = templ->m;

    Fetcher *fch = (Fetcher *)Iter_Get(&templ->content);
    Abstract *data = Iter_Get(&templ->data);
    i32 idx = templ->content.idx;
    if(templ->type.state & DEBUG){
        void *args[] = {
            I32_Wrapped(m, templ->content.idx),
            Type_StateVec(m, TYPE_ITER_UPPER, templ->objType.state),
            fch,
            data,
            NULL
        };
        /*
        Out("^y.HandleJump \\@$ @ - fch/data=@/@^0\n", args);
        */
        Out("^y.HandleJump \\@$ @^0\n", args);
    }

    if(templ->objType.state & (
            UFLAG_ITER_ENCLOSE|UFLAG_ITER_FINISH)){
        if(templ->objType.state & NOOP){
            templ->objType.state &= ~NOOP;
        }else{
            goto paths;
        }
    } 

    if(fch->type.state & FETCHER_END){

    }else if(fch->type.state & FETCHER_CONDITION){
        FetchTarget *tg = Span_Get(fch->val.targets, 0);
        if(tg->objType.of == FORMAT_TEMPL_INDENT){
            if((templ->objType.state & UFLAG_ITER_FOCUS) == 0 ||
               (templ->objType.state & UFLAG_ITER_LEAF)){
                templ->objType.state |= UFLAG_ITER_SKIP;
            }else{
                templ->objType.state |= UFLAG_ITER_ENCLOSE;
                TemplCrit *loop = Templ_LastJumpAt(templ, templ->content.idx,
                    UFLAG_ITER_ENCLOSE_IDX);
                TemplCrit *finish = Templ_LastJumpAt(templ, loop->contentIdx,
                    UFLAG_ITER_FINISH_IDX);
                Templ_AddJump(templ,
                    loop->contentIdx,
                    templ->content.idx,
                    UFLAG_ITER_FINISH_IDX,
                    MORE|UFLAG_ITER_NEXT);
                Templ_AddJump(templ,
                    loop->contentIdx,
                    finish->contentIdx,
                    UFLAG_ITER_FINISH_IDX,
                    MORE|UFLAG_ITER_NEXT);

                if(templ->type.state & DEBUG){
                    Jumps *js = Lookup_Get(templ->jumps, loop->contentIdx);
                    Abstract *a = (Abstract *)js->crit[UFLAG_ITER_FINISH_IDX];
                    if(a->type.of == TYPE_ITER){
                        a = (Abstract*)((Iter *)a)->p;
                    }
                    void *args[] = {
                        a,
                        loop,
                        NULL
                    };
                    Out("^b.Enclose to Enclose finish @ - loop @^0\n", args);
                }
            }
        }else if(tg->objType.of == FORMAT_TEMPL_LEVEL){
            if(templ->objType.state & UFLAG_ITER_LEAF){
                templ->objType.state |= UFLAG_ITER_SKIP;
            }
        }else if(tg->objType.of == FORMAT_TEMPL_CURRENT){
            if((templ->objType.state & (UFLAG_ITER_LEAF|UFLAG_ITER_FOCUS)) !=
                    UFLAG_ITER_LEAF){
                templ->objType.state |= UFLAG_ITER_SKIP;
            }
        }else if(tg->objType.of == FORMAT_TEMPL_ACTIVE){
            if((templ->objType.state & (UFLAG_ITER_LEAF|UFLAG_ITER_FOCUS)) !=
                    (UFLAG_ITER_LEAF|UFLAG_ITER_FOCUS)){
                templ->objType.state |= UFLAG_ITER_SKIP;
            }
        }

        if(templ->objType.state & UFLAG_ITER_FINISH){
            templ->objType.state &= ~UFLAG_ITER_SKIP;
        }
    }else if(fch->type.state & FETCHER_FOR){
        Iter *it = NULL;
        if((fch->type.state & PROCESSING) == 0){
            Iter *value = as(Fetch(m, fch, data, NULL), TYPE_ITER);
            Itin_IterAdd(&templ->data, value);
            fch->type.state |= PROCESSING;
            it = (Iter *)Itin_GetByType(&templ->data, TYPE_ITER);
        }else{
            it = (Iter *)Itin_GetByType(&templ->data, TYPE_ITER);
            if((it->type.state & END) == 0){
                Iter_Remove(&templ->data);
                Iter_Prev(&templ->data);
            }
        }

        if(it == NULL || it->type.of != TYPE_ITER){
            void *args[] = { NULL, it, Iter_Get(&templ->data), NULL };
            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                "Error ^{STACK.name}, expected Iter have @ from"
                " Iter_Get(@) instead^0\n", 
                args);
            templ->type.state |= ERROR;
            DebugStack_Pop();
            return templ->type.state;
        }

        i32 indentIdx = it->idx;
        if((it->type.state & END) || (fch->api->next(it) & END)){
            templ->objType.state &= ~UFLAG_ITER_NEXT;
            templ->objType.state |= UFLAG_ITER_FINISH;
            templ->level = 0;
        }else{
            Abstract *a = fch->api->get(it);
            Itin_IterAdd(&templ->data, a);
            templ->objType.state |= UFLAG_ITER_NEXT;

            if(it->itin != NULL){
                templ->level = it->idx;
                templ->objType.state = ((templ->objType.state & ~Jump_NextSetFl) |
                    (it->itin->objType.state & Jump_NextSetFl));

                if(templ->type.state & DEBUG){
                    void *ar[] = {
                        Type_StateVec(m, TYPE_ITER_UPPER, it->itin->objType.state),
                        a,
                        NULL
                    };
                    Out("^b.Item itin @ @^0\n", ar);
                }
            }else{
                if(templ->type.state & DEBUG){
                    void *ar[] = {
                        a,
                        Type_StateVec(m, TYPE_ITER, it->type.state),
                        NULL
                    };
                    Out("^b.Item @ @^0\n", ar);
                }
                templ->objType.state = ((templ->objType.state & ~Jump_NextSetFl) |
                    UFLAG_ITER_LEAF);
                if(it->idx > 0){
                    templ->objType.state |= UFLAG_ITER_SIBLING;
                }
                if(it->idx == it->metrics.selected){
                    templ->objType.state |= UFLAG_ITER_FOCUS;
                }
            }

            if(it->idx < indentIdx){
                templ->objType.state |= UFLAG_ITER_FINISH;
            }
        }
    }


paths:
    if(templ->objType.state & UPPER_FLAGS){
        Jumps *js = Lookup_Get(templ->jumps, templ->content.idx);
        if(js != NULL){
            status fl = 1 << 8;
            status local = NOOP;
            status clean = READY;
            Abstract *a = NULL;
            status flag = ZERO;
            i32 i = 0;

            if(js->type.state & UFLAG_ITER_INVERT){
                flag = UFLAG_ITER_SKIP;
                local |= UFLAG_ITER_SKIP;
                a = (Abstract *)js->crit[UFLAG_ITER_SKIP_IDX];
                printf("Invert set\n");
                fflush(stdout);
                exit(1);

                if(templ->type.state & DEBUG){
                    void *args[] = {
                        I32_Wrapped(m, templ->content.idx),
                        a, 
                        NULL
                    };
                    Out("^p.Skip \\@$ @^0\n", args);
                }
            }else{
                while(i < 8){
                    flag = fl << i;
                    a = (Abstract *)js->crit[i];
                    if((flag & js->type.state & templ->objType.state)){
                        Str **labels = Lookup_Get(ToSFlagLookup, TYPE_ITER_UPPER);
                        break;
                    }else{
                        if(flag != UFLAG_ITER_SKIP){
                            clean |= flag;
                        }
                        a = NULL;
                    }
                    i++;
                }
            }

            if(a != NULL){
                TemplCrit *crit = NULL;
                TemplCrit *prev = NULL;
                if(a->type.of == TYPE_ITER){
                    Iter *critIt = (Iter *)a;
                    crit = Iter_Get(critIt);
                    prev = Span_Get(critIt->p, critIt->idx-1);

                    void *args[] = {
                        critIt->p,
                        NULL
                    };
                    Out("^b.From stack @^0\n", args);

                    if(critIt->p->nvalues > 1){
                        Iter_Remove(critIt);
                        Iter_Prev(critIt);
                    }else if(flag == UFLAG_ITER_FINISH){
                        Iter *it = (Iter *)Itin_GetByType(&templ->data, TYPE_ITER);
                        if(it != NULL && (it->type.state & END)){
                            templ->objType.state &= ~UFLAG_ITER_NEXT; 
                        }
                    }
                }else{
                    crit = (TemplCrit *)a;
                }
                idx = crit->contentIdx;
                local &= ~NOOP;

                void *args[] = {
                    I32_Wrapped(templ->m, crit->dataIdx),
                    I32_Wrapped(templ->m, templ->level),
                    crit,
                    NULL
                };
                Out("^b.dataIdx @ vs level @ crit @^0\n", args);

                if((crit->type.state & MORE) && (crit->dataIdx == -1 || (crit->dataIdx >= templ->level))){
                    templ->objType.state |= (
                        crit->type.state & UPPER_FLAGS);
                }else{
                    templ->objType.state &= ~flag;
                }

                if(1 || templ->type.state & DEBUG){
                    void *args[] = {
                        I32_Wrapped(m, i), 
                        Type_StateVec(m, TYPE_ITER_UPPER, js->type.state),
                        Type_StateVec(m, TYPE_ITER_UPPER, templ->objType.state),
                        Type_StateVec(m, TYPE_ITER_UPPER, flag),
                        NULL
                    };
                    Out("^Ep. FlagFound^e. js/templ/fl@ @ @ @^0\n", args);
                }
            }

            if((local & UFLAG_ITER_INVERT) == 0){
                templ->objType.state |= (local & ~UFLAG_ITER_SKIP);
                templ->objType.state &= ~clean;
                if((templ->type.state & DEBUG) && (clean != ZERO)){
                    void *args[] = {
                        Type_StateVec(m, TYPE_ITER_UPPER, clean),
                        NULL
                    };
                    Out("^b. Removing flag @^0\n", args);
                }
            }
        }
    }

    Abstract *debug = Iter_Get(&templ->data);
    DebugStack_SetRef(debug, debug->type.of);

    if(idx != templ->content.idx){
        if(templ->type.state & DEBUG){
            void *args[] = {
                I32_Wrapped(m, idx),
                I32_Wrapped(m, templ->content.idx),
                fch,
                NULL
            };
            /*
            Out("^c.Jump to @ \\@$ - @^0\n", args);
            */
            Out("^c.Jump to @ \\@$^0\n", args);
        }
        Iter_GetByIdx(&templ->content, idx);
        DebugStack_Pop();
        return PROCESSING;
    }

    DebugStack_Pop();
    return ZERO;
}

Jumps *Jumps_Make(MemCh *m, i32 idx){
    Jumps *js = MemCh_AllocOf(m, sizeof(Jumps), TYPE_TEMPL_JUMPS);
    js->type.of = TYPE_TEMPL_JUMPS;
    js->idx = idx;
    return js;
}
