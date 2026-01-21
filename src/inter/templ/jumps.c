#include <external.h>
#include <caneka.h>

status Templ_HandleJump(Templ *templ){
    status r = READY;
    DebugStack_Push(templ, templ->type.of);
    MemCh *m = templ->m;

    Fetcher *fch = (Fetcher *)Iter_Get(&templ->content);
    Abstract *data = Iter_Get(&templ->data);
    i32 idx = templ->content.idx;
    void *args[] = {
        I32_Wrapped(m, templ->content.idx),
        Type_StateVec(m, TYPE_ITER_UPPER, templ->objType.state),
        fch,
        data,
        NULL
    };
    Out("^y.HandleJump \\@$ @ - fch/data=@/@^0\n", args);


    if(templ->objType.state & (
            UFLAG_ITER_ENCLOSE|UFLAG_ITER_JUMPIN|UFLAG_ITER_FINISH)){
        if(templ->objType.state & NOOP){
            templ->objType.state &= ~NOOP;
        }else{
            printf("skip next on flags\n");
            fflush(stdout);
            goto paths;
        }
    } 

    if(fch->type.state & FETCHER_FOR){
        templ->objType.state |= UFLAG_ITER_NEXT;
    }else if(fch->type.state & FETCHER_END){

    }else if(fch->type.state & FETCHER_CONDITION){
        printf("condition\n");
        fflush(stdout);
        FetchTarget *tg = Span_Get(fch->val.targets, 0);
        if(tg->objType.of == FORMAT_TEMPL_INDENT){
            if((templ->objType.state & UFLAG_ITER_FOCUS) == 0 ||
               (templ->objType.state & UFLAG_ITER_LEAF)){
                templ->objType.state |= UFLAG_ITER_SKIP;
            }else{
                templ->objType.state |= UFLAG_ITER_ENCLOSE;
                Jumps *js = Lookup_Get(templ->jumps, templ->content.idx);
                TemplCrit *crit = js->crit[UFLAG_ITER_ENCLOSE_IDX];
                js = Lookup_Get(templ->jumps, crit->contentIdx);
                crit = js->crit[UFLAG_ITER_FINISH_IDX];
                void *args[] = {
                    crit,
                    NULL
                };
                Out("^b.Enclose to Enclose crit @^0\n", args);
            }
        }else if(tg->objType.of == FORMAT_TEMPL_LEVEL){
            if(templ->objType.state & UFLAG_ITER_LEAF){
                templ->objType.state |= UFLAG_ITER_SKIP;
            }
        }else if(tg->objType.of == FORMAT_TEMPL_CURRENT){
            void *args[] = {
                Type_StateVec(m, TYPE_ITER_UPPER,
                    templ->objType.state & (UFLAG_ITER_LEAF|UFLAG_ITER_FOCUS)),
                    NULL
                };
            Out("^b.Current @^0\n", args);
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
    }

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
    */

    if(idx == templ->content.idx && (templ->objType.state & UFLAG_ITER_NEXT)){
        templ->objType.state &= ~UFLAG_ITER_NEXT;
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
        if(fch->api->next(it) & END){
            templ->objType.state &= UFLAG_ITER_NEXT;
            templ->objType.state |= UFLAG_ITER_FINISH;
            printf("end\n");
            fflush(stdout);
            exit(1);
        }else{
            Abstract *a = fch->api->get(it);
            Itin_IterAdd(&templ->data, a);
            templ->objType.state |= (it->itin->objType.state & (
               UFLAG_ITER_FOCUS|UFLAG_ITER_SIBLING|UFLAG_ITER_LEAF
            ));
            if(it->idx < indentIdx){
                templ->objType.state |= UFLAG_ITER_FINISH;
            }
            void *args[] = {
                Type_StateVec(m, TYPE_ITER_UPPER, templ->objType.state),
                a,
                NULL,
            };
            Out("^p.Item: @ templ->objType.state=@^0\n", args);
        }
    }


paths:
    if(templ->objType.state & UPPER_FLAGS){
        Jumps *js = Lookup_Get(templ->jumps, templ->content.idx);
        if(js != NULL){
            status fl = 1 << 8;
            i32 i = 0;
            status noop = NOOP;
            Abstract *a = NULL;
            while(i < 8){
                status flag = fl << i;
                void *args[] = {
                    I32_Wrapped(m, i), 
                    Type_StateVec(m, TYPE_ITER_UPPER, js->type.state),
                    Type_StateVec(m, TYPE_ITER_UPPER, templ->objType.state),
                    Type_StateVec(m, TYPE_ITER_UPPER, flag),
                    NULL
                };
                Out("^Ep. Stuff^e. js/templ/fl@ @ @ @^0\n", args);
                a = (Abstract *)js->crit[i];
                TemplCrit *crit = NULL;
                if((flag & js->type.state & templ->objType.state)){
                    Str **labels = Lookup_Get(ToSFlagLookup, TYPE_ITER_UPPER);
                    void *args[] = {
                        labels[i+9],
                        I32_Wrapped(m, templ->content.idx),
                        js->crit[i], 
                        NULL
                    };
                    Out("^p.Jump Found @/\\@$ @^0\n", args);
take:
                    if(a != NULL){
                        if(a->type.of == TYPE_ITER){
                            Iter *critIt = (Iter *)a;
                            crit = Iter_Get(critIt);
                            if(critIt->idx > 0){
                                Iter_Remove(critIt);
                                Iter_Prev(critIt);
                            }
                        }else{
                            crit = (TemplCrit *)a;
                        }
                        idx = crit->contentIdx;
                        if((templ->objType.state & MORE) == 0){
                            templ->objType.state &= ~flag;
                        }
                        noop &= ~NOOP;
                        break;
                    }

                }
                i++;
            }
            if((noop & NOOP & js->type.state)){
                a = (Abstract *)js->crit[UFLAG_ITER_SKIP_IDX];
                goto take;
            }else{
                templ->objType.state |= noop;
            }
        }
    }

    Abstract *debug = Iter_Get(&templ->data);
    DebugStack_SetRef(debug, debug->type.of);

    if(idx != templ->content.idx){
        void *args[] = {
            I32_Wrapped(m, idx),
            I32_Wrapped(m, templ->content.idx),
            fch,
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
