#include <external.h>
#include <caneka.h>

status Templ_HandleJump(Templ *templ){
    status r = READY;
    DebugStack_Push(templ, templ->type.of);
    MemCh *m = templ->m;

    Fetcher *fch = (Fetcher *)Iter_Get(&templ->content);
    Abstract *data = Iter_Get(&templ->data);
    void *args[] = {
       fch,
       data,
       NULL
    };
    Out("^y.HandleJump @/@^0\n", args);

    if(templ->objType.state & UPPER_FLAGS){
        Jumps *js = Lookup_Get(templ->jumps, templ->content.idx);
        if(js != NULL){
            status fl = 1 << 8;
            i32 i = 0;
            while(TRUE){
                if((fl & js->type.state & templ->objType.state)){
                    void *args[] = {
                        I32_Wrapped(m, templ->content.idx),
                        js->crit[i], 
                        NULL
                    };
                    Out("^p.Jump Found \\@$ @^0\n", args);
                }
                fl << 1;
                i++;
            }
        }
    }

    i32 idx = templ->content.idx;

    Fetcher *fch = jump->fch;
    if(fch->type.state & FETCHER_END){
        
    }else if(fch->type.state & FETCHER_FOR){
        templ->objType.state |= UFLAG_ITER_NEXT;
    /*
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
    */

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

        i32 indentIdx = it->idx;
        if(fch->api->next(it) & END){
            templ->objType.state &= UFLAG_ITER_NEXT;
            templ->objType.state |= UFLAG_ITER_OUTDENT;
        }else{
            Itin_IterAdd(&templ->data, fch->api->get(it));
            templ->objType.state |= (it->objType.state & (
               UFLAG_ITER_FOCUS|UFLAG_ITER_SIBLING 
            ));
            if(it->idx < indentIdx){
                templ->objType.state |= UFLAG_ITER_FINISH;
            }
            templ->indent.idx = it->idx;
            return PROCESSING;
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

Jumps *Jumps_Make(MemCh *m, i32 idx){
    Jumps *js = MemCh_AllocOf(m, sizeof(Jumps), TYPE_TEMPL_JUMPS);
    js->type.of = TYPE_TEMPL_JUMPS;
    js->idx = idx;
    return js;
}
