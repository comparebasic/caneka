#include <external.h>
#include <caneka.h>

i64 Templ_ToSCycle(Templ *templ, Stream *sm, i64 total){
    if(Iter_Next(&templ->content) & END){
        templ->type.state |= SUCCESS;
        return total;
    }
    Abstract *item = Iter_Get(&templ->content);
    Abstract *data = Iter_Current(&templ->data);

    if(item->type.of == TYPE_STRVEC){
        total += ToS(sm, (Abstract *)item, 0, ZERO); 
    }else if(item->type.of == TYPE_WRAPPED_PTR){
        Single *sg = (Single *)item;
        if(sg->objType.of == FORMAT_TEMPL_INDENT){
            templ->indent = ((StrVec *)sg->val.ptr)->total;
            total += ToS(sm, (Abstract *)sg->val.ptr, 0, ZERO); 
        }else if(sg->objType.of == FORMAT_TEMPL_VAR){
            Abstract *value = From_PathKey(sm->m, data, (StrVec *)sg->val.ptr);
            if(value == NULL){
                Abstract *args[] = {
                    (Abstract *)sg->val.ptr,
                    NULL,
                };
                Error(sm->m, (Abstract *)templ, FUNCNAME, FILENAME, LINENUMBER,
                    "Error finding value @\n",args);
                return total;
            }
            total += ToS(sm, (Abstract *)value, 0, ZERO); 
        }else if(sg->objType.of == FORMAT_TEMPL_VAR_FOR){
            Iter *it = From_GetIter(sm->m, data, sg->val.ptr);
            Iter_Add(&templ->data,
                (Abstract *)it);
            if((Iter_Next(it) & END) == 0){
                Iter_Add(&templ->data, (Abstract *)Iter_Get(it));
            }
        }else if(sg->objType.of == TYPE_TEMPL_JUMP){
            TemplJump *jump = (TemplJump*)sg->val.ptr;
            if(jump->jumpType.of == FORMAT_TEMPL_VAR_FOR){
                Iter_PrevRemove(&templ->data);
                Iter *it = (Iter *)as(Iter_Get(&templ->data), TYPE_ITER);
                if((Iter_Next(it) & END) == 0){
                    Iter_Add(&templ->data, (Abstract *)Iter_Get(it));
                    TemplJump *jump = (TemplJump*)sg->val.ptr;
                    Iter_GetByIdx(&templ->content, jump->destIdx);
                }
            }
        }else if(sg->objType.of == FORMAT_TEMPL_LOGIC_END){
            TemplJump *jump = TemplJump_Make(sm->m, sg->objType.of, 
                templ->content.idx, -1);
            sg->objType.of = TYPE_TEMPL_JUMP;
            sg->val.ptr = jump;

            Iter it;
            memcpy(&it, &templ->content, sizeof(Iter));
            i32 idx = templ->content.idx;

            i32 targetIdx = templ->content.p->max_idx;
            while((Iter_Prev(&it) & END) == 0){
                if(it.idx > targetIdx){
                    continue;
                }
                Abstract *prev = Iter_Get(&it);
                if(prev->type.of == TYPE_WRAPPED_PTR){
                    Single *item = (Single *)prev;
                    if(item->objType.of == TYPE_TEMPL_JUMP){
                       targetIdx = ((TemplJump *)item->val.ptr)->destIdx; 
                    }else if(item->objType.of > _FORMAT_TEMPL_LOGIC_START &&
                        item->objType.of < _FORMAT_TEMPL_LOGIC_END){
                        jump->destIdx = it.idx;
                        jump->jumpType.of = item->objType.of;
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
            NULL
        };
        Out("Templ: ^p.&^0.\n", args);
    }

    return total;
}

i64 Templ_ToS(Templ *templ, Stream *sm, OrdTable *data){
    i64 total = 0;
    i16 g = 0;
    Span *p = Span_Make(sm->m);
    Span_Add(p, (Abstract *)data);
    Iter_Init(&templ->data, p);
    Iter_Next(&templ->data);
    while((total = Templ_ToSCycle(templ, sm, total)) && 
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
