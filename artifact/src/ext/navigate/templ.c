#include <external.h>
#include <caneka.h>

Templ *Templ_Make(MemCh *m, Span *content, OrdTable *data){
    Templ *templ = (Templ *)MemCh_Alloc(m, sizeof(Templ));
    templ->type.of = TYPE_TEMPL;
    Iter_Init(&templ->content, content);
    Span *p = Span_Make(m);
    Span_Add(p, (Abstract *)data);
    Iter_Init(&templ->data, p);
    Iter_Next(&templ->data);
    return templ;
}

i64 Templ_ToSCycle(Templ *templ, Stream *sm, i64 total){
    if(Iter_Next(&templ->content) & END){
        templ->type.state |= SUCCESS;
        return total;
    }
    Abstract *item = Iter_Get(&templ->content);
    Abstract *data = Iter_Current(&templ->data);

    if(item->type.of == TYPE_STRVEC){
        total += ToS(sm, (Abstract *)item, 0, ZERO); 
    }else if(item->type.of == TYPE_TEMPL_JUMP){
        TemplJump *jump = (TemplJump*)item;
        if(jump->jumpType.of == FORMAT_CASH_VAR_FOR){
            Iter *it = (Iter *)as(data, TYPE_ITER); 
            if((Iter_Next(it) & END) == 0){;
                Iter_Add(&templ->data, (Abstract *)Iter_Get(it));
            }else{
                Iter_PrevRemove(&templ->data);
            }
        }else if(jump->jumpType.of == FORMAT_CASH_VAR_ENDFOR){
            Iter_PrevRemove(&templ->data);
            data = Iter_Get(&templ->data);
            Iter *it = (Iter *)as(data, TYPE_ITER); 
            if((Iter_Next(it) & END) == 0){
                Iter_Add(&templ->data, (Abstract *)Iter_Get(it));
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
    }else if(item->type.of == TYPE_WRAPPED_PTR){
        Single *sg = (Single *)item;
        if(sg->objType.of == FORMAT_CASH_VAR_FOR){
            Iter *it = From_GetIter(sm->m, data, sg->val.ptr);
            Iter_Add(&templ->data,
                (Abstract *)it);
            if((Iter_Next(it) & END) == 0){
                Iter_Add(&templ->data, (Abstract *)Iter_Get(it));
            }
            Iter_Set(&templ->content,
                TemplJump_Make(sm->m, sg->objType.of, templ->content.idx, NEGATIVE));
        }else if(sg->objType.of == FORMAT_CASH_VAR_KEYVALUE){
            Abstract *value = From_Name((Abstract *)data);
            total += ToS(sm, (Abstract *)value, 0, ZERO); 
        }else if(sg->objType.of == FORMAT_CASH_VAR_NAMEVALUE){
            Abstract *value = From_Value((Abstract *)data);
            total += ToS(sm, (Abstract *)value, 0, ZERO); 
        }else if(sg->objType.of == FORMAT_CASH_VAR_KEY){
            Abstract *value = From_Key(data, sg->val.ptr);
            total += ToS(sm, (Abstract *)value, 0, ZERO); 
        }else if(sg->objType.of > _FORMAT_CASH_VAR_ENDINGS && 
            item->type.of < _FORMAT_CASH_VAR_ENDINGS_END){
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
            (Abstract *)data,
            (Abstract *)Iter_Current(&templ->data),
            (Abstract *)item,
            NULL
        };
        Out("Templ: ^p.&^0.\nData: ^p.@^0.\nDataItem: ^p.&^0.\nItem:^y.&^0\n\n", args);
    }

    return total;
}

i64 Templ_ToS(Templ *templ, Stream *sm){
    i64 total = 0;
    i16 g = 0;
    while((total = Templ_ToSCycle(templ, sm, total)) && 
        (templ->type.state & OUTCOME_FLAGS) == 0){
        Guard_Incr(&g, 64, FUNCNAME, FILENAME, LINENUMBER);
    }
    return total;
}
