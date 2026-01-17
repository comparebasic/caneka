#include <external.h>
#include <caneka.h>

status Templ_HandleJump(Templ *templ){
    status r = READY;
    DebugStack_Push(templ, templ->type.of);

    TemplJump *jump = (TemplJump *)Iter_Get(&templ->content);
    Abstract *data = Iter_Get(&templ->data);

    i32 idx = templ->content.idx;
    if(templ->objType.state & (UFLAG_ITER_INDENT|UFLAG_ITER_OUTDENT)){
        templ->objType.state &= ~(UFLAG_ITER_INDENT|UFLAG_ITER_OUTDENT);
        /*
        if(templ->objType.state & UFLAG_ITER_INDENT){
            Iter *itp = Span_Get(templ->indent.p, templ->indent.idx);
            if(itp != NULL && itp->p->nvalues > 0){
                Single *sg = Iter_Get(itp);
                Iter_Remove(itp);
                Iter_Prev(itp);
                if(itp->idx < 0){
                    templ->objType.state &= ~UFLAG_ITER_INDENT;
                }
                Iter_GetByIdx(&templ->content, sg->val.i);
                r |= PROCESSING;
            }
        }else if(templ->objType.state & UFLAG_ITER_OUTDENT){
            Iter *itp = Span_Get(templ->indent.p, templ->outdent.idx);
            if(itp != NULL && itp->p->nvalues > 0){
                Single *sg = Iter_Get(itp);
                Iter_Remove(itp);
                Iter_Prev(itp);
                if(itp->idx < 0){
                    templ->objType.state &= ~UFLAG_ITER_OUTDENT;
                }
                Iter_GetByIdx(&templ->content, sg->val.i);
                r |= PROCESSING;
            }
        }
        */
    }

    if(idx != templ->content.idx){
        Iter_GetByIdx(&templ->content, idx);
        DebugStack_Pop();
        return PROCESSING;
    }

    Fetcher *fch = jump->fch;
    if(fch->type.state & FETCHER_END){
        if(jump->sourceType.state & (FETCHER_WITH)){
            Iter_Remove(&templ->data);
            Iter_Prev(&templ->data);
        }
    }else if(fch->type.state & FETCHER_FOR){
        if((templ->objType.state & (UFLAG_ITER_INDENT|UFLAG_ITER_OUTDENT)) == 0){
            templ->objType.state |= UFLAG_ITER_NEXT;
        }
    }else if(fch->type.state & (FETCHER_IF|FETCHER_WITH)){
        Abstract *value = Fetch(templ->m, fch, data, NULL);
        if(value == NULL){
            templ->objType.state |= UFLAG_ITER_SKIP;
        }else{
            if(fch->type.state & (FETCHER_IF|FETCHER_WITH)){
                Itin_IterAdd(&templ->data, value);
            }
        }
    }else if((fch->type.state & (FETCHER_CONDITION|FETCHER_VAR)) ==
        (FETCHER_CONDITION|FETCHER_VAR)){
        Single *ret = I32_Wrapped(templ->m, templ->content.idx);
        ret->objType.of = TYPE_TEMPL_JUMP_RET;
        /* handle value remove before this somehow */
        Itin_IterAdd(&templ->data, ret);
        printf("IndentCondition\n");
        exit(1);
    }else if(fch->type.state & FETCHER_CONDITION){
        Iter *it = (Iter *)Itin_GetByType(&templ->data, TYPE_ITER);
        FetchTarget *tg = Span_Get(fch->val.targets, 0);
        if(it != NULL){
            if(it->type.state & LAST){
                if(
                    (tg->objType.of == FORMAT_TEMPL_CURRENT && 
                        (templ->objType.state & UFLAG_ITER_SELECTED)) ||
                    (tg->objType.of == FORMAT_TEMPL_LEVEL)
                ){
                    templ->objType.state |= UFLAG_ITER_SKIP;
                }
            }else if(tg->objType.of != FORMAT_TEMPL_LEVEL){
                templ->objType.state |= UFLAG_ITER_SKIP;
            }
        }

        void *args[] = {fch, Type_StateVec(templ->m, TYPE_ITER_UPPER, templ->objType.state), NULL};
        Out("Condition @ @^0\n", args);
    }

    if(templ->objType.state & UFLAG_ITER_NEXT){
        if((fch->type.state & PROCESSING) == 0){
            DebugStack_SetRef(fch, fch->type.of);
            void *value = as(Fetch(templ->m, fch, data, NULL), TYPE_ITER);
            Itin_IterAdd(&templ->data, value);
            fch->type.state |= PROCESSING;
            printf("Fetch Iter\n");
            fflush(stdout);
        }else{
            void *args[] = {Iter_Get(&templ->data), NULL};
            Out("^y.Removing @^0\n", args);
            Iter_Remove(&templ->data);
            Iter_Prev(&templ->data);
        }

        Iter *it = (Iter *)Itin_GetByType(&templ->data, TYPE_ITER);
        if(it == NULL || it->type.of != TYPE_ITER){
            void *args[] = { NULL, it, Iter_Get(&templ->data), NULL };
            Error(templ->m, FUNCNAME, FILENAME, LINENUMBER,
                "Error ^{STACK.name}, expected Iter have @ from"
                " Iter_Get(@) instead^0\n", 
                args);
            templ->type.state |= ERROR;
            return templ->type.state;
        }

        if(fch->api->next(it) & END){
            void *args[] = {Iter_Get(&templ->data), I32_Wrapped(templ->m, templ->content.idx), jump, NULL};
            Out("^p.End Removing @ \\@$ - @^0\n", args);

            Iter_Remove(&templ->data);
            Iter_Prev(&templ->data);
            templ->objType.state &= ~(PROCESSING|UFLAG_ITER_NEXT);
            templ->objType.state |= UFLAG_ITER_SKIP;
        }else{
            templ->objType.state = it->itin->objType.state|PROCESSING;
            Itin_IterAdd(&templ->data, fch->api->get(it));

            void *args[] = {Iter_Get(&templ->data), NULL};
            Out("^p.NextItem @^0\n", args);

            if(it->idx > idx){
                templ->indent.idx = it->idx;
                templ->indent.incr++;
                templ->objType.state |= UFLAG_ITER_INDENT;
            } else if(it->idx < idx){
                templ->outdent.idx = it->idx;
                templ->outdent.incr++;
                templ->objType.state |= UFLAG_ITER_OUTDENT;
            }
        }
    }

    if(jump->crit.ret.idx != -1 && (jump->crit.ret.type.state & templ->objType.state)){
        idx = jump->crit.ret.idx;
    }else if(jump->crit.skip.idx != -1 && ((templ->objType.state & UFLAG_ITER_SKIP) ||
            (jump->crit.skip.type.state != ZERO && 
                (jump->crit.skip.type.state & templ->objType.state) == 0))){
        templ->objType.state &= ~UFLAG_ITER_SKIP;
        idx = jump->crit.skip.idx;
        printf("skip to %d\n", idx);
        fflush(stdout);
    }else if(jump->crit.dest.idx != -1 && (jump->crit.dest.type.state & templ->objType.state)){
        idx = jump->crit.dest.idx;
        void *args[] = {I32_Wrapped(templ->m, idx), jump, NULL};
        Out("dest @ jump @\n", args);
    }

    if(idx != templ->content.idx){
        printf("jump %d\n", idx);
        fflush(stdout);
        Iter_GetByIdx(&templ->content, idx);
        DebugStack_Pop();
        return PROCESSING;
    }

    DebugStack_Pop();
    return ZERO;
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
