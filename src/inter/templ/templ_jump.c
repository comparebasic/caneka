#include <external.h>
#include <caneka.h>

static status Templ_handleEndJump(Templ *templ, TemplJump **jumpPtr){
    status r = READY;
    TemplJump *jump = *jumpPtr;
    i32 curIdx = -1;
    Iter *it = Span_Get(templ->data.p, templ->data.idx-1);
    if(jump->sourceType.state & FETCHER_WITH){
        Iter_Remove(&templ->data);
        Iter_Prev(&templ->data);
    }else if(jump->sourceType.state & FETCHER_FOR){
        Iter_GetByIdx(&templ->content, jump->crit.dest.idx);
        *jumpPtr = (TemplJump *)Iter_Get(&templ->content);
    }else if(jump->sourceType.state & FETCHER_CONDITION){
        status fl = IterUpper_FlagCombine(
            (UFLAG_ITER_OUTDENT|UFLAG_ITER_STRICT),
            templ->objType.state);
        if(fl & SUCCESS){
            if(jump->crit.skip.idx != -1 && jump->crit.skip.incr > 0 &&
                    jump->crit.skip.incr-- <= 0){
                Iter_GetByIdx(&templ->content, jump->crit.skip.idx);
                r |= PROCESSING;
            }else{
                DebugStack_Pop();
                return ZERO;
            }
        }else{
            Iter_GetByIdx(&templ->content, jump->crit.skip.idx);
            r |= PROCESSING;
        }
    }
    return r;
}

static status Templ_handleForJump(Templ *templ, TemplJump *jump, Abstract *data){
    i32 curIdx = templ->content.idx;
    Fetcher *fch = jump->fch;
    status r = READY;
    if((fch->type.state & PROCESSING) == 0){
        DebugStack_SetRef(fch, fch->type.of);
        void *value = as(Fetch(templ->m, fch, data, NULL), TYPE_ITER);
        Iter_Add(&templ->data, value);
        fch->type.state |= PROCESSING;
    }else{
        if((fch->type.state & MORE) == 0){
            Iter_Remove(&templ->data);
            Iter_Prev(&templ->data);
        }
    }

    Iter *it = (Iter *)Iter_Get(&templ->data);
    if(it != NULL && it->type.of != TYPE_ITER){
        void *args[] = { NULL, it, Iter_Get(&templ->data), NULL };
        Error(templ->m, FUNCNAME, FILENAME, LINENUMBER,
            "Error ^{STACK.name}, expected Iter have @ from Iter_Get(@) instead^0\n", 
        args);
        templ->type.state |= ERROR;
        return templ->type.state;
    }

    /*
    void *ar[] = {Type_StateVec(templ->m, TYPE_ITER_UPPER, it->objType.state), NULL};
    Out("^p.PrevState @^0\n", ar);
    */

    if((it->objType.state & (UFLAG_ITER_OUTDENT|UFLAG_ITER_INDENT)) ==
            (UFLAG_ITER_OUTDENT|UFLAG_ITER_INDENT)){
        TemplJump *close = Span_Get(templ->content.p, jump->crit.ret.idx);
        close->crit.skip.incr++;
    }else if(it->objType.state & UFLAG_ITER_OUTDENT){
        if(fch->type.state & MORE){
            fch->type.state &= ~MORE;
        }else{
            i32 idx = templ->content.idx;
            TemplJump *close = Iter_GetByIdx(&templ->content,
                jump->crit.ret.idx);
            close->crit.skip.incr++;
            fch->type.state |= MORE;
            DebugStack_Pop();
            return r;
        }
    }

    if((fch->type.state & MORE) == 0){
        while((fch->api->next(it) & END) == 0){
            void *a = fch->api->get(it);
            if(a != NULL){
                templ->objType.state = it->objType.state;
                /*
                void *ar[] = {a, NULL};
                Out("^p.Item @^0\n", ar);
                */
                Iter_Add(&templ->data, a);
                break;
            }
        }
    }

    if(it->type.state & END){
        TemplJump *close = Span_Get(templ->content.p, jump->crit.ret.idx);
        if(close != NULL && close->crit.skip.incr > 0){
            if(close->sourceType.state & FETCHER_CONDITION){
                templ->objType.state = UFLAG_ITER_OUTDENT;
            }
            Iter_GetByIdx(&templ->content, jump->crit.ret.idx);
            r |= PROCESSING;
            fch->type.state |= MORE;
        }else{
            i32 idx = jump->crit.skip.idx != NEGATIVE ? jump->crit.skip.idx : curIdx;
            if(idx != NEGATIVE){
                Iter_GetByIdx(&templ->content, idx);
                Iter_Remove(&templ->data);
                Iter_Prev(&templ->data);
            }
        }
    }
    return r;
}

static status Templ_handleConditionJump(Templ *templ, TemplJump *jump, Abstract *data){
    status r = READY;
    Fetcher *fch = jump->fch;
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
    return r;
}

status Templ_HandleJump(Templ *templ){
    DebugStack_Push(templ, templ->type.of);

    status r = READY;
    void *args[5];
    TemplJump *jump = (TemplJump *)Iter_Get(&templ->content);
    Abstract *data = Iter_Get(&templ->data);

    i32 curIdx = -1;
    Fetcher *fch = jump->fch;
    if(fch->type.state & FETCHER_END){
        r |= Templ_handleEndJump(templ, &jump);
    }
    fch = jump->fch;

    if(fch->type.state & FETCHER_FOR){
        r |= Templ_handleForJump(templ, jump, data);
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
        r |= Templ_handleConditionJump(templ, jump, data);
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
