#include <external.h>
#include <caneka.h>

status Templ_HandleJump(Templ *templ){
    status r = READY;
    DebugStack_Push(templ, templ->type.of);
    MemCh *m = templ->m;

    Fetcher *fch = (Fetcher *)Iter_Get(&templ->content);
    Abstract *data = Iter_Get(&templ->data);
    i32 idx = templ->content.idx;
    TemplFunc *fs = Span_Get(templ->funcs, idx);
    if(templ->type.state & DEBUG){
        if(fs != NULL){
            void *ar[] = {fs, NULL};
            Out("^y.FS @^0\n", ar);
        }
    }
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
            if(fs == NULL || fs->func == NULL){
                void *ar[] = {templ, NULL};
                Error(m, FUNCNAME, FILENAME, LINENUMBER,
                    "Error expected TemplFunc for For INDENT &", ar);
                return ERROR;
            }
            fs->func(templ, fs);
            templ->objType.state &= ~fs->dflag.negative;
            templ->objType.state |= fs->dflag.positive;
        }
    }else if(fch->type.state & FETCHER_FOR){
        if(fs == NULL || fs->func == NULL){
            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                "Error expected TemplFunc for For Loop", NULL);
            return ERROR;
        }
        fs->func(templ, fs);
        templ->objType.state &= ~fs->dflag.negative;
        templ->objType.state |= fs->dflag.positive;
    }

    if((templ->type.state & DEBUG) && fs != NULL){
        void *args[] = {
            Type_StateVec(m, TYPE_ITER_UPPER, fs->dflag.negative),
            Type_StateVec(m, TYPE_ITER_UPPER, fs->dflag.positive),
            Type_StateVec(m, TYPE_ITER_UPPER, templ->objType.state),
            NULL
        };
        Out("^p.For func Flags\n    negative: @\n    posative: @\n"
            "    templ->objType: @\n", args);
    }

paths:
    if(templ->objType.state & UPPER_FLAGS){
        Jumps *js = Span_Get(templ->jumps, templ->content.idx);
        if(js != NULL){
            status fl = 1 << 8;
            status local = NOOP;
            status clean = READY;
            Abstract *a = NULL;
            status flag = ZERO;
            i32 i = 0;

            if(js->type.state & UFLAG_ITER_SKIP){
                TemplCrit *crit = Templ_LastJumpAt(templ,
                    templ->content.idx, UFLAG_ITER_SKIP_IDX);

                if(crit->dflag.positive != ZERO && 
                        (crit->dflag.positive & templ->objType.state) ==
                            crit->dflag.positive){

                    void *ar[] = {
                        I32_Wrapped(m, templ->content.idx),
                        Type_StateVec(m, TYPE_ITER_UPPER, crit->dflag.positive),
                        NULL
                    };
                    Out("new skip TRUE \\@$ dflag.positive@^0\n", ar);

                    flag = UFLAG_ITER_SKIP;
                    local |= UFLAG_ITER_SKIP;
                    a = (Abstract *)js->crit[UFLAG_ITER_SKIP_IDX];
                }else if(crit->dflag.negative != ZERO &&
                        (crit->dflag.negative & templ->objType.state) !=
                            crit->dflag.negative){

                    void *ar[] = {
                        I32_Wrapped(m, templ->content.idx),
                        Type_StateVec(m, TYPE_ITER_UPPER, crit->dflag.negative),
                        NULL
                    };
                    Out("new skip TRUE \\@$ dflag.negative@^0\n", ar);

                    flag = UFLAG_ITER_SKIP;
                    local |= UFLAG_ITER_SKIP;
                    a = (Abstract *)js->crit[UFLAG_ITER_SKIP_IDX];
                }
            }

            if(a == NULL){
                while(i < 8){
                    flag = fl << i;
                    a = (Abstract *)js->crit[i];
                    if((flag & js->type.state & templ->objType.state)){
                        Str **labels = Lookup_Get(ToSFlagLookup, TYPE_ITER_UPPER);
                        break;
                    }else{
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

                    if(templ->type.state & DEBUG){
                        void *args[] = {
                            critIt->p,
                            NULL
                        };
                        Out("^b.From stack @^0\n", args);
                    }

                    if(critIt->p->nvalues > 1){
                        Iter_Remove(critIt);
                        Iter_Prev(critIt);
                    }else if(flag == UFLAG_ITER_FINISH){
                        Iter *it = (Iter *)Itin_GetByType(&templ->data, TYPE_ITER);
                        if(it != NULL && (it->type.state & END)){
                            templ->objType.state &= ~UFLAG_ITER_ACTION; 
                            printf("compareing finish - removing flag\n");
                            fflush(stdout);
                        }else{
                            printf("compareing finish - not yet\n");
                            fflush(stdout);
                        }
                    }
                }else{
                    crit = (TemplCrit *)a;
                }
                idx = crit->contentIdx;
                local &= ~NOOP;

                if(templ->type.state & DEBUG){
                    void *args[] = {
                        I32_Wrapped(templ->m, crit->dataIdx),
                        I32_Wrapped(templ->m, templ->level),
                        crit,
                        NULL
                    };
                    Out("^b.dataIdx @ vs level @ crit @^0\n", args);
                }

                if((crit->type.state & MORE) && (crit->dataIdx == -1 || (crit->dataIdx >= templ->level))){
                    templ->objType.state |= (
                        crit->dflag.positive & UPPER_FLAGS);
                }else{
                    templ->objType.state &= ~flag;
                }

                if(templ->type.state & DEBUG){
                    void *args[] = {
                        I32_Wrapped(m, i), 
                        Type_StateVec(m, TYPE_ITER_UPPER, js->type.state),
                        Type_StateVec(m, TYPE_ITER_UPPER, templ->objType.state),
                        Type_StateVec(m, TYPE_ITER_UPPER, flag),
                        NULL
                    };
                    Out("^Ep. FlagFound^e. js/templ/fl@ @ @ @^0\n", args);
                }
            }else{
                if(flag != UFLAG_ITER_SKIP){
                    clean |= flag;
                }
            }

            if((local & NOOP) == 0 && (js->type.state & UFLAG_ITER_REQUIRED) == 0){
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
