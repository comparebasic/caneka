#include <external.h>
#include <caneka.h>

status Templ_HandleJump(Templ *templ){
    DebugStack_Push(templ, templ->type.of);

    status r = READY;
    void *args[5];
    TemplJump *jump = (TemplJump *)Iter_Get(&templ->content);
    Abstract *data = Iter_Get(&templ->data);

    Fetcher *fch = jump->fch;
    i32 curIdx = -1;
    if(fch->type.state & FETCHER_END){
        curIdx = templ->content.idx;
        Iter *it = Span_Get(templ->data.p, templ->data.idx-1);
        if(jump->sourceType.state & FETCHER_WITH){
            Iter_Remove(&templ->data);
            Iter_Prev(&templ->data);
        }else if(jump->sourceType.state & FETCHER_FOR){
            Iter_GetByIdx(&templ->content, jump->crit.dest.idx);
            jump = (TemplJump *)Iter_Get(&templ->content);
            fch = jump->fch;
        }else if(jump->sourceType.state & FETCHER_CONDITION){
            if(it != NULL){
                if(jump->crit.skip.type.state != ZERO){
                    status fl = IterUpper_FlagCombine(jump->crit.skip.type.state, 
                            it->objType.state);
                    if((fl & NOOP) && jump->crit.skip.idx != -1){
                        if(it->objType.state & UFLAG_ITER_INDENT){
                            jump->crit.skip.incr++;
                        }
                        Iter_GetByIdx(&templ->content, jump->crit.skip.idx);
                        r |= PROCESSING;
                    }
                }
            }

            return r;
        }
    }

    if(fch->type.state & FETCHER_FOR){
        if((fch->type.state & PROCESSING) == 0){
            DebugStack_SetRef(fch, fch->type.of);
            void *value = as(Fetch(templ->m, fch, data, NULL), TYPE_ITER);
            Iter_Add(&templ->data, value);
            fch->type.state |= PROCESSING;
        }else{
            Iter_Remove(&templ->data);
            Iter_Prev(&templ->data);
        }

        Iter *it = (Iter *)Iter_Get(&templ->data);
        if(it->type.of != TYPE_ITER){
            void *args[] = { NULL, it, Iter_Get(&templ->data), NULL };
            Error(templ->m, FUNCNAME, FILENAME, LINENUMBER,
                "Error ^{STACK.name}, expected Iter have @ from Iter_Get(@) instead^0\n", 
            args);
            templ->type.state |= ERROR;
            return templ->type.state;
        }

        status upperFlags = it->objType.state;
        if(upperFlags & UFLAG_ITER_OUTDENT){
            printf("OUTDENT! go to ret!\n");
            exit(1);
        }

        while((fch->api->next(it) & END) == 0){
            void *a = fch->api->get(it);
            if(a != NULL){
                Iter_Add(&templ->data, a);
                break;
            }
        }

        if(it->type.state & END){
            i32 idx = jump->crit.skip.idx != NEGATIVE ? jump->crit.skip.idx : curIdx;
            if(idx != NEGATIVE){
                Iter_GetByIdx(&templ->content, idx);
                Iter_Remove(&templ->data);
                Iter_Prev(&templ->data);
            }
        }else if(jump->crit.dest.idx != -1 && 
                    IterUpper_FlagCombine(jump->crit.dest.type.state, upperFlags) & SUCCESS){
            Iter_GetByIdx(&templ->content, jump->crit.dest.idx);
            r |= PROCESSING;
        }
    }else if(fch->type.state & FETCHER_IF){
        fch->type.state |= PROCESSING;
        DebugStack_SetRef(fch, fch->type.of);
        Abstract *value = Fetch(templ->m, fch, data, NULL);
        if(value == NULL){
            Iter_GetByIdx(&templ->content, jump->crit.skip.idx);
            r |= PROCESSING;
        }else{
            Iter_Add(&templ->data, value);
        }
    }else if(fch->type.state & FETCHER_WITH){
        DebugStack_SetRef(fch, fch->type.of);
        Abstract *value = Fetch(templ->m, fch, data, NULL);
        Iter_Add(&templ->data, value);
    }else if(fch->type.state & FETCHER_CONDITION){
        if(fch->val.targets->nvalues == 0){
            Iter_GetByIdx(&templ->content, jump->crit.dest.idx);
            r |= PROCESSING;
        }else{
            FetchTarget *tg = Span_Get(fch->val.targets, 0);
            Iter *it = Span_Get(templ->data.p, templ->data.idx-1);
            if(tg == NULL){
                Iter_GetByIdx(&templ->content, jump->crit.skip.idx);
                r |= PROCESSING;
            }else if(tg->objType.of == FORMAT_TEMPL_LEVEL  &&
                it != NULL && (it->type.state & LAST) == 0
            ){
                /* proceed w content */
            }else if(tg->objType.of == FORMAT_TEMPL_INDENT &&
                it != NULL &&
                (it->objType.state & UFLAG_ITER_INDENT)
            ){
                TemplJump *dest = Span_Get(templ->content.p, jump->crit.dest.idx);
                if(dest != NULL){
                    dest->crit.ret.type.state = UFLAG_ITER_OUTDENT;
                    dest->crit.ret.incr = 2;
                    dest->crit.ret.idx = templ->content.idx+1;
                }

                i32 idx = jump->crit.dest.idx;
                jump->crit.dest.idx = -1;
                Iter_GetByIdx(&templ->content, idx);
                r |= PROCESSING;
            }else if(tg->objType.of == FORMAT_TEMPL_CURRENT  &&
                it != NULL &&
                    (it->objType.state & UFLAG_ITER_SELECTED) == 0 &&
                    (it->type.state & LAST)
            ){
                /* proceed with content */
            }else if(tg->objType.of == FORMAT_TEMPL_ACTIVE  &&
                it != NULL &&
                (it->objType.state & UFLAG_ITER_SELECTED) &&
                (it->type.state & LAST)
            ){
                /* proceed with content */
            }else{
                Iter_GetByIdx(&templ->content, jump->crit.skip.idx);
                r |= PROCESSING;
            }
        }
    }

    if(r == READY){
        r |= NOOP;
    }

    DebugStack_Pop();
    return r;
}


TemplJump *TemplJump_Make(MemCh *m, i32 idx, Fetcher *fch){
    TemplJump *jump = (TemplJump *)MemCh_Alloc(m, sizeof(TemplJump));
    jump->type.of = TYPE_TEMPL_JUMP;
    jump->idx = idx;
    jump->fch = fch;
    jump->crit.dest.idx = jump->crit.skip.idx = jump->crit.ret.idx = NEGATIVE;
    jump->crit.dest.type.of =
        jump->crit.skip.type.of =
        jump->crit.ret.type.of =
        TYPE_TEMPL_JUMP_CRIT
        ;
    return jump;
}
