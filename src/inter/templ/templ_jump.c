#include <external.h>
#include <caneka.h>

status Templ_HandleJump(Templ *templ){
    DebugStack_Push(templ, templ->type.of);
    status r = READY;
    void *args[5];
    TemplJump *jump = (TemplJump *)Iter_Get(&templ->content);
    Abstract *data = Iter_Get(&templ->data);

    Fetcher *fch = jump->fch;
    i32 skipIdx = -1;
    if(fch->type.state & FETCHER_END){
        skipIdx = templ->content.idx;
        TemplJump *dest = (TemplJump *)Span_Get(templ->content.p, jump->destIdx);
        if(dest->fch->type.state & FETCHER_WITH){
            Iter_Remove(&templ->data);
            Iter_Prev(&templ->data);
        }
        if(dest->fch->type.state & FETCHER_FOR){
            Iter_GetByIdx(&templ->content, jump->destIdx);
            jump = (TemplJump *)Iter_Get(&templ->content);
            fch = jump->fch;
        }
    }

    if(1 || templ->type.state & DEBUG){
        void *args[] = {
            jump,
            fch,
            NULL
        };
        Out("^b.  Jump: & \n  of &^0.\n", args);
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

        while((fch->api->next(it) & END) == 0){
            void *a = fch->api->get(it);
            if(a != NULL){
                Iter_Add(&templ->data, a);
                break;
            }
        }

        if(it->type.state & END){
            i32 idx = jump->skipIdx != NEGATIVE ? jump->skipIdx : skipIdx;
            if(idx != NEGATIVE){
                Iter_GetByIdx(&templ->content, idx);
                Iter_Remove(&templ->data);
                Iter_Prev(&templ->data);
            }
        }
        if((it->objType.state & MORE) == 0){
            Iter_GetByIdx(&templ->content, jump->destIdx);
        }
        r |= PROCESSING;
    }else if(fch->type.state & FETCHER_IF){
        if(templ->type.state & DEBUG){
            void *args[] = {
                fch,
                data,
                NULL
            };
            Out("^c.  If: & of @^0.\n", args);
        }
        fch->type.state |= PROCESSING;
        DebugStack_SetRef(fch, fch->type.of);
        Abstract *value = Fetch(templ->m, fch, data, NULL);
        if(value == NULL){
            if(templ->type.state & DEBUG){
                void *args[] = {
                    fch,
                    data,
                    NULL
                };
                Out("^c.  If is null: & of @^0.\n", args);
            }
            Iter_GetByIdx(&templ->content, jump->skipIdx);
            r |= PROCESSING;
        }else{
            Iter_Add(&templ->data, value);
            if(templ->type.state & DEBUG){
                void *args[] = {
                    fch,
                    Iter_Get(&templ->data),
                    NULL
                };
                Out("^c.  If nesting/found: & of @^0.\n", args);
            }
        }
        r |= PROCESSING;
    }else if(fch->type.state & FETCHER_WITH){
        if(templ->type.state & DEBUG){
            void *args[] = {
                fch,
                data,
                NULL
            };
            Out("^c.  With: & of @^0.\n", args);
        }
        DebugStack_SetRef(fch, fch->type.of);
        Abstract *value = Fetch(templ->m, fch, data, NULL);
        Iter_Add(&templ->data, value);
        r |= PROCESSING;
    }else if(fch->type.state & FETCHER_CONDITION){
        if(fch->val.targets->nvalues == 0){
            Iter_GetByIdx(&templ->content, jump->destIdx);
            r |= PROCESSING;
        }else{
            FetchTarget *tg = Span_Get(fch->val.targets, 0);
            Iter *it = Span_Get(templ->data.p, templ->data.idx-1);
            void *args[] = {
                fch,
                Type_ToStr(templ->m, tg->objType.of),
                data,
                NULL
            };
            Out("^c.  Command: & ^E.@^e. @^0.\n", args);
            if(tg == NULL){
                Iter_GetByIdx(&templ->content, jump->skipIdx);
                r |= PROCESSING;
            }else if(tg->objType.of == FORMAT_TEMPL_LEVEL  &&
                it != NULL && (it->objType.state & LAST) == 0
            ){
                void *args[] = {
                    fch,
                    Type_ToStr(templ->m, tg->objType.of),
                    NULL
                };
                Out("^0.  Running Command: & @^0.\n", args);
            }else if(tg->objType.of == FORMAT_TEMPL_INDENT &&
                it != NULL &&
                (it->objType.state & FLAG_ITER_SELECTED)
            ){
                void *args[] = {
                    fch,
                    Type_ToStr(templ->m, tg->objType.of),
                    I32_Wrapped(templ->m, jump->destIdx),
                    NULL
                };
                Out("^0.  Running Command: & @ to @^0.\n", args);
                Iter_GetByIdx(&templ->content, jump->destIdx);
                r |= PROCESSING;
            }else if(tg->objType.of == FORMAT_TEMPL_CURRENT  &&
                it != NULL && (it->objType.state & LAST)
            ){
                void *args[] = {
                    fch,
                    Type_ToStr(templ->m, tg->objType.of),
                    NULL
                };
                Out("^0.  Running Command: & @^0.\n", args);
            }else if(tg->objType.of == FORMAT_TEMPL_ACTIVE  &&
                it != NULL &&
                ((it->objType.state & (LAST|FLAG_ITER_SELECTED)) ==
                    (LAST|FLAG_ITER_SELECTED))
            ){
                void *args[] = {
                    fch,
                    Type_ToStr(templ->m, tg->objType.of),
                    NULL
                };
                Out("^0.  Running Command: & @^0.\n", args);
            }else{
                Iter_GetByIdx(&templ->content, jump->skipIdx);
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
    jump->destIdx = jump->skipIdx = jump->tempIdx = NEGATIVE;
    jump->fch = fch;
    return jump;
}
