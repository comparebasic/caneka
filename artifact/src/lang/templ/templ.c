#include <external.h>
#include <caneka.h>

static TemplJump *swapJump(Templ *templ){
    i32 idx = templ->content.idx;
    TemplJump *jump = TemplJump_Make(sm->m, idx, fch);
    Span_Set(templ->content, idx, (Abstract *)jump);
    Iter_GetByIdx(&templ->content, idx);
    return jump;
}

i64 Templ_ToSCycle(Templ *templ, Stream *sm, i64 total, Abstract *source){
    if(Iter_Next(&templ->content) & END){
        templ->type.state |= SUCCESS;
        return total;
    }
    Abstract *item = Iter_Get(&templ->content);
    Abstract *data = Iter_Current(&templ->data);

    if(templ->type.state & DEBUG){
        Abstract *args[] = {
            (Abstract *)templ,
            NULL
        };
        Out("^y.Templ_ToSCycle &^0.\n", args);
    }

    if(item->type.of == TYPE_STRVEC){
        total += ToS(sm, (Abstract *)item, 0, ZERO); 
        /*
        if((it->type.state & END) == 0){
            templ->level++;
            goto nest then
            goto for target
        }
        */
    }else if(item->type.of == TYPE_TEMPL_JUMP){
        TemplJump *jump = (TemplJump *)item;
        if(templ->type.state & DEBUG){
            Abstract *args[] = {
                (Abstract *)item,
                NULL
            };
            Out("^c.Jump: &^0.\n", args);
        }

        Fetcher *fch = jump->fch;
        if(fch->type.state & FETCHER_END){
            fch = Span_Get(templ->content.p, jump->destIdx);
        }

        if(fch & FETCHER_FOR){
            Iter_PrevRemove(&templ->data);
            Iter *it = (Iter *)as(Iter_Get(&templ->data), TYPE_ITER);
            if((Iter_Next(it) & END) == 0){
                Iter_Add(&templ->data, (Abstract *)Iter_Get(it));
                Iter_GetByIdx(&templ->content, jump->destIdx);
            }
        }else if(fch & FETCHER_COMMAND){
            Iter *it = (Iter *)as(Iter_Get(&templ->data), TYPE_ITER);
            i32 maxDepth = Object_Depth(Iter_Current(&temp->data));
            FetchTarget *tg = Span_Get(fch->val.targets, 0);
            if(
                (tg->idx == jump->level && tg->objType.of == FORMAT_TEMPL_LEVEL) ||
                (tg->idx == maxDepth && tg->objType.of == FORMAT_TEMPL_LEVEL_ITEM) ||
                (tg->objType.of == FORMAT_TEMPL_LEVEL_BETWEEN && jump->level < maxDepth)
                ){
                   /* proceed */ 
                }else if(tg->objType.of == FORMAT_TEMPL_LEVEL_NEST){
                    Iter *it = (Iter *)Iter_Get(&templ->data), TYPE_ITER;
                    if(it != NULL){
                        Iter_Add(&templ->data, (Abstract *)Iter_Get(it));
                    }else{
                        Iter_GetByIdx(&templ->content, jump->skipIdx);
                    }
                }else{
                    Iter_GetByIdx(&templ->content, jump->skipIdx);
                }
            }
        }
    }else if(item->type.of == TYPE_FETCHER){
        Fetcher *fch = (Fetcher *)item;
        if(templ->type.state & DEBUG){
            Abstract *args[] = {
                (Abstract *)fch,
                NULL
            };
            Out("^c.Fetcher: &^0.\n", args);
        }
        if(fch->type.state & FETCHER_VAR){
            Abstract *value = Fetch(sm->m, fch, data, NULL);
            if(value == NULL){
                Abstract *args[] = {
                    (Abstract *)fch,
                    (Abstract *)data,
                    NULL,
                };
                Error(sm->m, (Abstract *)templ, FUNCNAME, FILENAME, LINENUMBER,
                    "Error finding value using @ in data @\n",args);
                return total;
            }
            if(templ->type.state & DEBUG){
                Abstract *args[] = {
                    (Abstract *)value,
                    NULL
                };
                Out("^c.VarValue: &^0.\n", args);
            }
            total += ToS(sm, (Abstract *)value, 0, ZERO); 
        }else if(fch->type.state & (FETCHER_FOR|FETCHER_COMMAND|FETCHER_END)){
            TemplJump *jump = swapJump(templ);

            if(fch->type.state & FETCHER_FOR){

                templ->level = 0;
                Iter *it = (Iter *)Fetch(sm->m, fch, data, NULL);
                Iter_Add(&templ->data, (Abstract *)it);
                if((Iter_Next(it) & END) == 0){
                    Iter_Add(&templ->data, (Abstract *)Iter_Get(it));
                }
                if(templ->type.state & DEBUG){
                    Abstract *args[] = {
                        (Abstract *)Iter_Get(it),
                        (Abstract *)it,
                        NULL,
                    };
                    Out("^y.For @ of &^y.\n", args);
                }
            }else if(fch->type.state & FETCHER_COMMAND){
                Iter it;
                memcpy(&it, &templ->content, sizeof(Iter));

                while((Iter_Next(&it) & END) == 0){
                    if(it.idx > targetIdx){
                        continue;
                    }
                    if(prev->type.of == TYPE_TEMPL_JUMP){
                        Abstract *a = Iter_Get(&it);
                        if(a->type.of == TYPE_TEMPL_JUMP){
                            Abstract *prevJump = (TemplJump *)a;
                            }else if(item->type.state & 
                                    (FETCHER_WITH|FETCHER_FOR|FETCHER_IF|FETCHER_IFNOT)){
                                jump->destIdx = it.idx;
                                break;
                            }
                        }else if(a->type.of == TYPE_FETCHER && 
                                (a->type.state & (FETCHER_WITH|FETCHER_FOR|FETCHER_IF|FETCHER_IFNOT|FETCHER_END))){
                            jump->skipIdx = it.idx; 
                        }
                    }
                }

                if(jump->skipIdx == NEGATIVE){
                    Error(ErrStream->m, (Abstract *)templ, FUNCNAME, FILENAME, LINENUMBER, 
                        "Unable to find skip ending in Templ content\n", NULL);
                }else{
                    Iter_GetByIdx(&templ->content, idx);
                    Iter_Prev(&templ->content;
                }
            }else if(fch->type.state & FETCHER_END){
                Iter it;
                memcpy(&it, &templ->content, sizeof(Iter));

                i32 targetIdx = templ->content.p->max_idx;
                while((Iter_Prev(&it) & END) == 0){
                    if(it.idx > targetIdx){
                        continue;
                    }
                    if(prev->type.of == TYPE_TEMPL_JUMP){
                        Abstract *a = Iter_Get(&it);
                        if(a->type.of == TYPE_TEMPL_JUMP){
                            Abstract *prevJump = (TemplJump *)a;
                            if(prevJump->fch->type.state & FETCHER_END){
                                targetIdx = prevJump->destIdx;
                            }else if(item->type.state & 
                                    (FETCHER_WITH|FETCHER_FOR|FETCHER_IF|FETCHER_IFNOT)){
                                jump->destIdx = it.idx;
                                break;
                            }
                        }
                    }
                }

                if(jump->destIdx == NEGATIVE){
                    Error(ErrStream->m, (Abstract *)templ, FUNCNAME, FILENAME, LINENUMBER, 
                        "Unable to find source of ending in Templ content\n", NULL);
                }else{
                    Iter_Prev(&templ->content);
                }
            }
        }
    }

    if(templ->content.type.state & END){
        templ->type.state |= SUCCESS;
    }

    if(templ->type.state & DEBUG){
        Abstract *args[] = {
            (Abstract *)templ,
            (Abstract *)item,
            (Abstract *)sm->dest.curs->v,
            NULL
        };
        Out("Templ: ^p.& \\@@ \n  ^E.out^e.:@^0.\n", args);
    }

    return total;
}

i64 Templ_ToS(Templ *templ, Stream *sm, Abstract *data, Abstract *source){
    i64 total = 0;
    i16 g = 0;
    Span *p = Span_Make(sm->m);
    Span_Add(p, data);
    Iter_Init(&templ->data, p);
    Iter_Next(&templ->data);
    while((total = Templ_ToSCycle(templ, sm, total, source)) && 
        (templ->type.state & OUTCOME_FLAGS) == 0){
        Guard_Incr(&g, 64, FUNCNAME, FILENAME, LINENUMBER);
    }
    return total;
}

Templ *Templ_Make(MemCh *m, Span *content){
    Templ *templ = (Templ *)MemCh_Alloc(m, sizeof(Templ));
    templ->type.of = TYPE_TEMPL;
    Iter_Init(&templ->content, content);
    return templ;
}
