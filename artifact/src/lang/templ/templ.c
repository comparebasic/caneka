#include <external.h>
#include <caneka.h>

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
    }else if(item->type.of == TYPE_FETCHER){
        Fetcher *fch = (Fetcher *)item;
        if(fch->type.state & FETCHER_FOR){
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
            total += ToS(sm, (Abstract *)value, 0, ZERO); 
        }else if(fch->type.state & FETCHER_FOR){
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
            Abstract *a = Span_Get(fch->val.targets, 0);
            if(a->type.of == TYPE_TEMPL_JUMP){
                TemplJump *jump = (TemplJump*)a;
                if(jump->type.state & FETCHER_FOR){
                    Iter_PrevRemove(&templ->data);
                    Iter *it = (Iter *)as(Iter_Get(&templ->data), TYPE_ITER);
                    if((Iter_Next(it) & END) == 0){
                        Iter_Add(&templ->data, (Abstract *)Iter_Get(it));
                        Iter_GetByIdx(&templ->content, jump->destIdx);
                    }
                }
            }
        }else if(fch->type.state & FETCHER_END){
            TemplJump *jump = TemplJump_Make(sm->m, templ->content.idx, -1);
            fch->type.state = (fch->type.state & NORMAL_FLAGS) | FETCHER_COMMAND;
            Span_Add(fch->val.targets, (Abstract *)jump);

            Iter it;
            memcpy(&it, &templ->content, sizeof(Iter));
            i32 idx = templ->content.idx;

            i32 targetIdx = templ->content.p->max_idx;
            while((Iter_Prev(&it) & END) == 0){
                if(it.idx > targetIdx){
                    continue;
                }
                Abstract *prev = Iter_Get(&it);
                if(prev->type.of == TYPE_FETCHER){
                    Fetcher *item = (Fetcher *)prev;
                    Abstract *a = Span_Get(item->val.targets, 0);
                    if(item->type.state & FETCHER_COMMAND &&
                            a->type.of == TYPE_TEMPL_JUMP){
                        targetIdx = ((TemplJump *)a)->destIdx; 
                    }else if(item->type.state & 
                            (FETCHER_WITH|FETCHER_FOR|FETCHER_IF|FETCHER_IFNOT)){
                        jump->destIdx = it.idx;
                        jump->type.state =  item->type.state & UPPER_FLAGS;
                        break;
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

    if(templ->content.type.state & END){
        templ->type.state |= SUCCESS;
    }

    if(templ->type.state & DEBUG){
        Abstract *args[] = {
            (Abstract *)templ,
            (Abstract *)item,
            NULL
        };
        Out("Templ: ^p.@ \\@@^0.\n", args);
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
