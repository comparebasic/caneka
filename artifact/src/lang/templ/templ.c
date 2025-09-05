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
            (Abstract *)Iter_Current(&templ->content),
            (Abstract *)Iter_Current(&templ->data),
            NULL
        };
        Out("^y.Templ_ToSCycle\n  ^E.content:^e.&\n  ^E.data:^e.&^0.\n", args);
    }

    word unregJumpFl = (FETCHER_FOR|FETCHER_COMMAND|FETCHER_END|FETCHER_WITH);
    if(item->type.of == TYPE_FETCHER && (((Fetcher *)item)->type.state & unregJumpFl)){
        Fetcher *fch = (Fetcher*)item;

        i32 idx = templ->content.idx;
        TemplJump *jump = TemplJump_Make(sm->m, idx, fch);
        Span_Set(templ->content.p, idx, (Abstract *)jump);
        Iter_GetByIdx(&templ->content, idx);

        if(templ->type.state & DEBUG){
            Abstract *args[] = {
                (Abstract *)jump,
                NULL,
            };
            Out("^c.  Making a Jump &^0.\n", args);
        }

        if(fch->type.state & FETCHER_FOR){
            jump->level = 0;
            jump->depth = Object_Depth(data);
            Iter *it = (Iter *)as(Fetch(sm->m, fch, data, NULL), TYPE_ITER);
            Iter_Add(&templ->data, (Abstract *)it);
            if((Iter_Next(it) & END) == 0){
                Iter_Add(&templ->data, (Abstract *)Iter_Get(it));
                Iter_Next(&templ->content);
            }
            /*
            if(templ->type.state & DEBUG){
                Abstract *args[] = {
                    (Abstract *)fch,
                    (Abstract *)it,
                    NULL,
                };
                Out("^c.  For\n\nFch:@\nIt:&^0.\n", args);
            }
            */
        }else if(fch->type.state & FETCHER_WITH){
            /*continue */
        }else if(fch->type.state & FETCHER_COMMAND){
            Iter it;
            memcpy(&it, &templ->content, sizeof(Iter));

            i32 targetIdx = it.p->max_idx;
            while((Iter_Next(&it) & END) == 0){
                if(it.idx > targetIdx){
                    continue;
                }

                Abstract *a = Iter_Get(&it);
                status fl = (FETCHER_WITH|FETCHER_FOR|FETCHER_IF|
                    FETCHER_IFNOT|FETCHER_END);
                if(a->type.of == TYPE_FETCHER && (a->type.state & fl)){
                    jump->skipIdx = it.idx; 
                }
            }

        }else if(fch->type.state & FETCHER_END){
            Iter it;
            memcpy(&it, &templ->content, sizeof(Iter));

            i32 targetIdx = templ->content.p->max_idx;
            while((Iter_Prev(&it) & END) == 0){
                if(it.idx > targetIdx){
                    continue;
                }

                Abstract *a = Iter_Get(&it);

                if(a->type.of == TYPE_TEMPL_JUMP){
                    TemplJump *prevJump = (TemplJump *)a;
                    if(prevJump->fch->type.state & FETCHER_END){
                        targetIdx = prevJump->destIdx;
                    }else if(prevJump->fch->type.state & 
                            (FETCHER_WITH|FETCHER_FOR|FETCHER_IF|FETCHER_IFNOT)){
                        jump->destIdx = it.idx;
                        break;
                    }
                }
            }

        }

        if((jump->fch->type.state & (FETCHER_FOR|FETCHER_WITH)) == 0 &&
                jump->destIdx == NEGATIVE && jump->skipIdx == NEGATIVE &&
                jump->tempIdx == NEGATIVE){
            Error(ErrStream->m, (Abstract *)templ, FUNCNAME, FILENAME, LINENUMBER, 
                "Unable to find source or ending for Jump in Templ content\n", NULL);
        }
        item = Iter_Get(&templ->content);
        data = Iter_Current(&templ->data);
    }

    if(item->type.of == TYPE_TEMPL_JUMP){
        TemplJump *jump = (TemplJump *)item;

        Fetcher *fch = jump->fch;
        i32 skipIdx = -1;
        if(fch->type.state & FETCHER_END){
            skipIdx = templ->content.idx;
            TemplJump *dest = (TemplJump *)Span_Get(templ->content.p, jump->destIdx);
            if(dest->fch->type.state & FETCHER_WITH){
                Iter_PrevRemove(&templ->data);
            }
            if(dest->fch->type.state & FETCHER_FOR){
                Iter_GetByIdx(&templ->content, jump->destIdx);
                jump = (TemplJump *)Iter_Get(&templ->content);
                fch = jump->fch;
            }
        }

        if(templ->type.state & DEBUG){
            Abstract *args[] = {
                (Abstract *)item,
                (Abstract *)fch,
                NULL
            };
            Out("^b.  Jump: & of &^0.\n", args);
        }

        if(fch->type.state & FETCHER_FOR){
            Iter_PrevRemove(&templ->data);
            Iter *it = (Iter *)as(Iter_Get(&templ->data), TYPE_ITER);
            if((Iter_Next(it) & END) == 0){
                Iter_Add(&templ->data, (Abstract *)Iter_Get(it));
            }else if(jump->skipIdx != NEGATIVE){
                Iter_GetByIdx(&templ->content, jump->skipIdx);
            }else if(skipIdx != NEGATIVE){
                Iter_GetByIdx(&templ->content, skipIdx);
            }
        }else if(fch->type.state & FETCHER_WITH){
            if(templ->type.state & DEBUG){
                Abstract *args[] = {
                    (Abstract *)fch,
                    (Abstract *)data,
                    NULL
                };
                Out("^c.  With: & of @^0.\n", args);
            }
            Abstract *value = Fetch(sm->m, fch, data, NULL);
            Iter_Add(&templ->data, (Abstract *)value);
        }else if(fch->type.state & FETCHER_COMMAND){
            if(templ->type.state & DEBUG){
                Abstract *args[] = {
                    (Abstract *)fch,
                    NULL
                };
                Out("^c.  Command: & of &^0.\n", args);
            }
            FetchTarget *tg = Span_Get(fch->val.targets, 0);
            if(
                (tg->idx == jump->level && tg->objType.of == FORMAT_TEMPL_LEVEL) ||
                (tg->idx == jump->depth && tg->objType.of == FORMAT_TEMPL_LEVEL_ITEM) ||
                (tg->objType.of == FORMAT_TEMPL_LEVEL_BETWEEN && jump->level < jump->depth)
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

        item = Iter_Get(&templ->content);
        data = Iter_Current(&templ->data);
    }

    if(item->type.of == TYPE_STRVEC){
        total += ToS(sm, (Abstract *)item, 0, ZERO); 
    }else if(item->type.of == TYPE_FETCHER && item->type.state & FETCHER_VAR){
        Fetcher *fch = (Fetcher *)item;
        if(templ->type.state & DEBUG){
            Abstract *args[] = {
                (Abstract *)fch,
                NULL
            };
            Out("^c.  Fetcher: &^0.\n", args);
        }

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
            Out("^c.  VarValue: &^0.\n", args);
        }
        total += ToS(sm, (Abstract *)value, 0, ZERO); 
    }

    if(templ->content.type.state & END){
        templ->type.state |= SUCCESS;
    }

    if(templ->type.state & DEBUG){
        Abstract *args[] = {
            (Abstract *)sm->dest.curs->v,
            NULL
        };
        Out("^gE.>> Out:^e. @^0.\n", args);
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
