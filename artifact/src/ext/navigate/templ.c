#include <external.h>
#include <caneka.h>

Templ *Templ_Make(MemCh *m, Span *content){
    Templ *templ = (Templ *)MemCh_Alloc(m, sizeof(Templ));
    templ->type.of = TYPE_TEMPL;
    Iter_Init(&templ->content, content);
    Iter_Init(&templ->it, Span_Make(m));
    return templ;
}

i64 Templ_ToSCycle(Templ *templ, Stream *sm, i64 total){
    if(Iter_Next(&templ->content) & END){
        templ->type.state |= SUCCESS;
        return total;
    }
    Abstract *content = Iter_Get(&templ->content);
    Abstract *stack = Iter_Current(&templ->it);

    if(content->type.of == TYPE_STRVEC){
        if(templ->type.state & DEBUG){
            Abstract *args[] = {
                (Abstract *)content,
                NULL
            };
            Out("^c.StrVec Out: &^0.\n", args);
        }
        total += ToS(sm, (Abstract *)content, 0, ZERO); 
    }else if(content->type.of == TYPE_TEMPL_JUMP){
        if(templ->type.state & DEBUG){
            Abstract *args[] = {
                (Abstract *)content,
                NULL
            };
            Out("^c.Jump: &^0.\n", args);
        }
        TemplJump *jump = (TemplJump*)content;
        if(jump->jumpType.of == FORMAT_CASH_VAR_FOR){
            Iter *it = (Iter *)as(stack, TYPE_ITER); 
            if((Iter_Next(it) & END) == 0){;
                Iter_Add(&templ->it, (Abstract *)Iter_Get(it));
            }else{
                Iter_PrevRemove(&templ->it);
            }
        }else if(jump->jumpType.of == FORMAT_CASH_VAR_ENDFOR){
            Iter_PrevRemove(&templ->it);
            stack = Iter_Get(&templ->it);
            Iter *it = (Iter *)as(stack, TYPE_ITER); 
            if((Iter_Next(it) & END) == 0){
                Iter_Add(&templ->it, (Abstract *)Iter_Get(it));
                Iter_GetByIdx(&templ->content, jump->destIdx);
            }
        }else{
            Abstract *args[] = {
                (Abstract *)jump,
                NULL
            };
            Out("^r.Jump not implemented @^0.\n", args);
            exit(1);
        }
    }else if(content->type.of == TYPE_WRAPPED_PTR){
        Single *sg = (Single *)content;
        if(sg->objType.of == FORMAT_CASH_INDENT){
            templ->indent = ((StrVec *)sg->val.ptr)->total;
            total += ToS(sm, (Abstract *)sg->val.ptr, 0, ZERO); 
        }else if(sg->objType.of == FORMAT_CASH_VAR_FOR){
            if(templ->type.state & DEBUG){
                Abstract *args[] = {
                    (Abstract *)sg,
                    NULL
                };
                Out("^c.For: &^0.\n", args);
            }
            Iter *it = From_GetIter(sm->m, (Abstract *)templ->data, sg->val.ptr);
            Iter_Add(&templ->it,
                (Abstract *)it);
            if((Iter_Next(it) & END) == 0){
                Iter_Add(&templ->it, (Abstract *)Iter_Get(it));
            }
            Iter_Set(&templ->content,
                TemplJump_Make(sm->m, sg->objType.of, templ->content.idx, NEGATIVE));
        }else if(sg->objType.of == FORMAT_CASH_VAR_KEYVALUE){
            if(templ->type.state & DEBUG){
                Abstract *args[] = {
                    (Abstract *)sg,
                    NULL
                };
                Out("^c.KeyValue: &^0.\n", args);
            }
            Abstract *value = From_Name((Abstract *)stack);
            total += ToS(sm, (Abstract *)value, 0, ZERO); 
        }else if(sg->objType.of == FORMAT_CASH_VAR_NAMEVALUE){
            if(templ->type.state & DEBUG){
                Abstract *args[] = {
                    (Abstract *)sg,
                    NULL
                };
                Out("^c.NameValue: &^0.\n", args);
            }
            Abstract *value = From_Value((Abstract *)stack);
            total += ToS(sm, (Abstract *)value, 0, ZERO); 
        }else if(sg->objType.of == FORMAT_CASH_VAR_KEY){
            if(templ->type.state & DEBUG){
                Abstract *args[] = {
                    (Abstract *)sg,
                    NULL
                };
                Out("^c.Key: &^0.\n", args);
            }
            Abstract *value = From_Key((Abstract *)templ->data, sg->val.ptr);
            total += ToS(sm, (Abstract *)value, 0, ZERO); 
        }else if(sg->objType.of > _FORMAT_CASH_VAR_ENDINGS && 
                content->type.of < _FORMAT_CASH_VAR_ENDINGS_END){
            if(templ->type.state & DEBUG){
                Abstract *args[] = {
                    (Abstract *)sg,
                    NULL
                };
                Out("^c.Endings: &^0.\n", args);
            }
            Iter _it;
            memcpy(&_it, &templ->content, sizeof(Iter));
            i32 idx = templ->content.idx;
            cls lookFor = sg->objType.of - (FORMAT_CASH_VAR_ENDIF - FORMAT_CASH_VAR_IF);

            TemplJump *jumpFrom = TemplJump_Make(sm->m, sg->objType.of, 
                templ->content.idx, -1);
            Iter_Set(&templ->content, (Abstract *)jumpFrom);
            while((Iter_Prev(&_it) & END) == 0){
                Abstract *prev = Iter_Get(&_it);
                if(prev->type.of == TYPE_TEMPL_JUMP){
                    TemplJump *jump = (TemplJump *)prev;
                    if(jump->destIdx == NEGATIVE){
                        jump->destIdx = idx;
                        jumpFrom->destIdx = _it.idx;
                        break;
                    }
                }
            }
            if(jumpFrom->destIdx == NEGATIVE){
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
            (Abstract *)stack,
            (Abstract *)Iter_Current(&templ->it),
            (Abstract *)content,
            NULL
        };
        Out("Templ: ^p.&^0.\nStack: ^p.@^0.\nDataItem: ^p.&^0.\nContent:^y.&^0\n\n", args);
    }

    return total;
}

i64 Stream_Templ(Stream *sm, Templ *templ, Nested *nd){
    i64 total = 0;
    i16 g = 0;
    templ->data = nd;
    while((total = Templ_ToSCycle(templ, sm, total)) && 
        (templ->type.state & OUTCOME_FLAGS) == 0){
        Guard_Incr(&g, 64, FUNCNAME, FILENAME, LINENUMBER);
    }
    return total;
}
