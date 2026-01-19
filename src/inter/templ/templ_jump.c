#include <external.h>
#include <caneka.h>

status Templ_HandleJump(Templ *templ){
    status r = READY;
    DebugStack_Push(templ, templ->type.of);
    MemCh *m = templ->m;

    TemplJump *jump = (TemplJump *)Iter_Get(&templ->content);
    Abstract *data = Iter_Get(&templ->data);

    i32 idx = templ->content.idx;
    if(templ->objType.state & (UFLAG_ITER_INDENT|UFLAG_ITER_OUTDENT)){
        templ->objType.state &= ~(UFLAG_ITER_INDENT|UFLAG_ITER_OUTDENT);
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
        Abstract *value = Fetch(m, fch, data, NULL);
        if(value == NULL){
            templ->objType.state |= UFLAG_ITER_SKIP;
        }else{
            if(fch->type.state & (FETCHER_IF|FETCHER_WITH)){
                Itin_IterAdd(&templ->data, value);
            }
        }
    }else if((fch->type.state & (FETCHER_CONDITION|FETCHER_VAR)) ==
            (FETCHER_CONDITION|FETCHER_VAR)){
        TemplCrit *crit = TemplCrit_Make(m,
            templ->content.idx, UFLAG_ITER_OUTDENT);
        if(jump->type.state & PROCESSING){
            jump->type.state &= PROCESSING;
            TemplJump *dest = Span_Get(templ->content.p, jump->crit.skip.idx);
            dest->crit.skip.type.state |= END;
            idx = templ->content.idx + 1;
        }else{
            Iter_Add(&templ->ret, crit);
            templ->objType.state |= UFLAG_ITER_SKIP;
            jump->type.state |= PROCESSING;
        }
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
    }

    if(idx == templ->content.idx && (templ->objType.state & UFLAG_ITER_NEXT)){
        Iter *it = NULL;
        if((fch->type.state & PROCESSING) == 0){
            DebugStack_SetRef(fch, fch->type.of);
            void *value = as(Fetch(m, fch, data, NULL), TYPE_ITER);
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
            return templ->type.state;
        }

        if(fch->api->next(it) & END){
            templ->objType.state &= ~(PROCESSING|UFLAG_ITER_NEXT);
            templ->objType.state |= UFLAG_ITER_SKIP;
        }else{
            templ->objType.state = ((templ->objType.state & UFLAG_ITER_SKIP) |
                (it->itin->objType.state|PROCESSING));
            Itin_IterAdd(&templ->data, fch->api->get(it));

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

    if(templ->objType.state & (UFLAG_ITER_INDENT|UFLAG_ITER_OUTDENT)){
        TemplJump *out = Span_Get(templ->content.p, jump->crit.out.idx);
        if(out != NULL){
            out->crit.skip.incr++;
        }
    }

    if(fch->type.state & FETCHER_FOR){
        void *args[3];
        args[0] = Type_StateVec(m, TYPE_ITER_UPPER, templ->objType.state);
        args[1] = Type_StateVec(m, TYPE_ITER_UPPER, jump->crit.dest.type.state);
        args[2] = NULL;
        Out("^y.For objType.state @ - dest@^0\n", args);
    }

    TemplCrit *crit = NULL;
    if(templ->ret.p->nvalues > 0 && ((crit = Iter_Get(&templ->ret)) != NULL) &&
        ((crit->type.state & templ->objType.state) ||
            ((templ->objType.state & PROCESSING) == 0)
        )){
        if(templ->objType.state & UFLAG_ITER_INDENT){
            crit->incr++;
        }else if(--crit->incr <= 1){
            idx = crit->idx;
            Iter_Remove(&templ->ret);
            Iter_Prev(&templ->ret);
            void *args[] = {
                I32_Wrapped(m, idx),
                I32_Wrapped(m, templ->content.idx),
                jump,
                NULL
            };
            Out("^c.  Ret to @ \\@$ - @^0\n", args);
        }
    }

    if(idx == templ->content.idx){
        boolean skipMatch = ((templ->objType.state & UFLAG_ITER_SKIP) ||
                    (jump->crit.skip.type.state != ZERO && 
                        (jump->crit.skip.type.state & templ->objType.state) == 0));
        if(jump->crit.skip.idx != -1 && (jump->crit.skip.incr > 0 || skipMatch)){
            TemplJump *enclose = NULL;

            if(jump->crit.skip.incr && skipMatch){
                jump->crit.skip.incr--;
                /*
                TemplCrit *crit = TemplCrit_Make(m,
                    jump->crit.out.idx, UFLAG_ITER_OUTDENT);
                Iter_Add(&templ->ret, crit);
                */
            }

            if((jump->crit.skip.type.state & END) &&
                    ((enclose = Span_Get(templ->content.p, jump->crit.enclose.idx))
                    != NULL)
            ){
                jump->crit.skip.type.state &= ~END;
                if(enclose->crit.out.idx != -1){
                    idx = enclose->crit.out.idx;
                    templ->objType.state |= UFLAG_ITER_OUTDENT;
                }else{
                    idx = enclose->idx;
                }
                void *args[] = {
                    I32_Wrapped(m, idx),
                    I32_Wrapped(m, templ->content.idx),
                    NULL
                };
                Out("^c.  Skip to end @ \\@$^0\n", args);
            }else{
                idx = jump->crit.skip.idx;
                void *args[] = {
                    I32_Wrapped(m, idx),
                    I32_Wrapped(m, jump->crit.skip.incr),
                    I32_Wrapped(m, templ->content.idx),
                    skipMatch ? S(m, "match") : S(m, "no-match"),
                    jump,
                    NULL
                };
                Out("^c.  Skip to @/$ \\@$ - @ - @^0\n", args);
            }
            templ->objType.state &= ~UFLAG_ITER_SKIP;
        }else if(jump->crit.dest.idx != -1 && 
                ((jump->crit.dest.type.state & UFLAG_ITER_INVERT) &&
                    (jump->crit.dest.type.state & templ->objType.state) == 0) ||
                ((jump->crit.dest.type.state & UFLAG_ITER_INVERT) == 0 &&
                    (jump->crit.dest.type.state & templ->objType.state))){
            idx = jump->crit.dest.idx;
            void *args[] = {
                I32_Wrapped(m, idx),
                I32_Wrapped(m, templ->content.idx),
                NULL
            };
            Out("^c.  Dest @ \\@$^0\n", args);
        }
    }

    if(idx != templ->content.idx){
        void *args[] = {
            I32_Wrapped(m, idx),
            I32_Wrapped(m, templ->content.idx),
            jump,
            NULL
        };
        Out("^c.Jump to @ \\@$ - @^0\n", args);
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
    jump->crit.dest.idx = jump->crit.skip.idx =
        jump->crit.enclose.idx = jump->crit.out.idx = NEGATIVE;
    jump->crit.dest.type.of = jump->crit.skip.type.of =
        jump->crit.enclose.type.of = jump->crit.out.type.of =
        TYPE_TEMPL_JUMP_CRIT;
    return jump;
}
