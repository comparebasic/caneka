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
            Out("^gU. With Remove^0\n", NULL);

            Iter_Remove(&templ->data);
            Iter_Prev(&templ->data);
        }else if(jump->sourceType.state & FETCHER_FOR){
            void *ar[] = {I32_Wrapped(templ->m, jump->destIdx), NULL};
            Out("^gU. -> Jump To Dest $ ->^0\n", ar);

            Iter_GetByIdx(&templ->content, jump->destIdx);
            jump = (TemplJump *)Iter_Get(&templ->content);
            fch = jump->fch;
        }else if(jump->sourceType.state & FETCHER_CONDITION){
            if(jump->skipIdx != -1 && it != NULL &&
                    (it->objType.state & LAST) == 0){
                    
                void *ar[] = {I32_Wrapped(templ->m, jump->skipIdx), NULL};
                Out("^gU. -> Jump To Skip $ ->^0\n", ar);

                Iter_GetByIdx(&templ->content, jump->skipIdx);
                jump = (TemplJump *)Iter_Get(&templ->content);
                fch = jump->fch;
            }
        }
    }

    if(fch->type.state & FETCHER_FOR){
        if((fch->type.state & PROCESSING) == 0){
            DebugStack_SetRef(fch, fch->type.of);
            void *value = as(Fetch(templ->m, fch, data, NULL), TYPE_ITER);
            Iter_Add(&templ->data, value);
            fch->type.state |= PROCESSING;
        }else{
            void *ar[] = {I32_Wrapped(templ->m, jump->skipIdx), NULL};
            Out("   ^gU. -> For Remove $ ->^0\n", ar);
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
            Out("^gU. -> End ->^0\n", NULL);

            i32 idx = jump->skipIdx != NEGATIVE ? jump->skipIdx : curIdx;
            if(idx != NEGATIVE){

                void *ar[] = {I32_Wrapped(templ->m, idx), NULL};
                Out("^gU. -> Jump To Skip $/End Remove ->^0\n", ar);

                Iter_GetByIdx(&templ->content, idx);
                Iter_Remove(&templ->data);
                Iter_Prev(&templ->data);
            }
        }else if((it->objType.state & MORE) == 0){
            void *ar[] = {I32_Wrapped(templ->m, jump->destIdx), NULL};
            Out("^gU. -> Jump To Skip $ ->^0\n", ar);

            Iter_GetByIdx(&templ->content, jump->destIdx);
        }
        r |= PROCESSING;
    }else if(fch->type.state & FETCHER_IF){
        fch->type.state |= PROCESSING;
        DebugStack_SetRef(fch, fch->type.of);
        Abstract *value = Fetch(templ->m, fch, data, NULL);
        if(value == NULL){

            void *ar[] = {I32_Wrapped(templ->m, jump->skipIdx), NULL};
            Out("^gU. -> Jump To Skip $ ->^0\n", ar);

            Iter_GetByIdx(&templ->content, jump->skipIdx);
            r |= PROCESSING;
        }else{
            Iter_Add(&templ->data, value);
        }
        r |= PROCESSING;
    }else if(fch->type.state & FETCHER_WITH){
        DebugStack_SetRef(fch, fch->type.of);
        Abstract *value = Fetch(templ->m, fch, data, NULL);
        Iter_Add(&templ->data, value);
        r |= PROCESSING;
    }else if(fch->type.state & FETCHER_CONDITION){
        if(fch->val.targets->nvalues == 0){

            void *ar[] = {I32_Wrapped(templ->m, jump->destIdx), NULL};
            Out("^gU. -> Jump To Skip $ ->^0\n", ar);

            Iter_GetByIdx(&templ->content, jump->destIdx);
            r |= PROCESSING;
        }else{
            FetchTarget *tg = Span_Get(fch->val.targets, 0);
            Iter *it = Span_Get(templ->data.p, templ->data.idx-1);
            void *args[] = {
                fch,
                Type_ToStr(templ->m, tg->objType.of),
                NULL
            };
            Out("^c.  Command: & ^E.@^e.^0.\n", args);
            if(tg == NULL){

                void *ar[] = {I32_Wrapped(templ->m, jump->skipIdx), NULL};
                Out("^gU. -> Jump To Skip $ ->^0\n", ar);

                Iter_GetByIdx(&templ->content, jump->skipIdx);
                r |= PROCESSING;
            }else if(tg->objType.of == FORMAT_TEMPL_LEVEL  &&
                it != NULL && (it->type.state & LAST) == 0
            ){
                void *args[] = {
                    fch,
                    Type_ToStr(templ->m, tg->objType.of),
                    NULL
                };
                Out("^0.    Running Command: & @^0.\n", args);
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
                Out("^0.    Running Command: & @ to @^0.\n", args);
                void *ar[] = {I32_Wrapped(templ->m, jump->destIdx), NULL};
                Out("^gU. -> Jump To Dest $ ->^0\n", ar);

                Iter_GetByIdx(&templ->content, jump->destIdx);
                r |= PROCESSING;
            }else if(tg->objType.of == FORMAT_TEMPL_CURRENT  &&
                it != NULL &&
                    (it->objType.state & FLAG_ITER_SELECTED) == 0 &&
                    (it->type.state & LAST)
            ){
                void *args[] = {
                    fch,
                    Type_ToStr(templ->m, tg->objType.of),
                    NULL
                };
                Out("^0.    Running Command: & @^0.\n", args);
            }else if(tg->objType.of == FORMAT_TEMPL_ACTIVE  &&
                it != NULL &&
                (it->objType.state & FLAG_ITER_SELECTED) &&
                (it->type.state & LAST)
            ){
                void *args[] = {
                    fch,
                    Type_ToStr(templ->m, tg->objType.of),
                    NULL
                };
                Out("^0.    Running Command: & @^0.\n", args);
            }else{
                void *ar[] = {I32_Wrapped(templ->m, jump->skipIdx), NULL};
                Out("^gU. -> Jump To Skip $ ->^0\n", ar);
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
