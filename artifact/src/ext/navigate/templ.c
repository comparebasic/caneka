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
        if(templ->type.state & DEBUG){
            Abstract *args[] = {
                (Abstract *)item,
                NULL
            };
            Out("^c.StrVec Out: &^0.\n", args);
        }
        total += ToS(sm, (Abstract *)item, 0, ZERO); 
    }else if(item->type.of == TYPE_TEMPL_JUMP){
        if(templ->type.state & DEBUG){
            Abstract *args[] = {
                (Abstract *)item,
                NULL
            };
            Out("^c.Jump: &^0.\n", args);
        }
        TemplJump *jump = (TemplJump*)item;
        if(jump->jumpType.of == FORMAT_TEMPL_VAR_FOR){
            Iter *it = (Iter *)as(data, TYPE_ITER); 
            if((Iter_Next(it) & END) == 0){;
                Iter_Add(&templ->data, (Abstract *)Iter_Get(it));
            }else{
                Iter_PrevRemove(&templ->data);
            }
        }else if(jump->jumpType.of == FORMAT_TEMPL_LOGIC_END){
            /*
            Iter_PrevRemove(&templ->data);
            data = Iter_Get(&templ->data);
            Iter *it = (Iter *)as(data, TYPE_ITER); 
            if((Iter_Next(it) & END) == 0){
                Iter_Add(&templ->data, (Abstract *)Iter_Get(it));
                Iter_GetByIdx(&templ->content, jump->destIdx);
            }
            */
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
        if(sg->objType.of == FORMAT_TEMPL_INDENT){
            if(templ->type.state & DEBUG){
                Abstract *args[] = {
                    (Abstract *)sg,
                    NULL
                };
                Out("^c.Indent: &^0.\n", args);
            }
            templ->indent = ((StrVec *)sg->val.ptr)->total;
            total += ToS(sm, (Abstract *)sg->val.ptr, 0, ZERO); 
        }else if(sg->objType.of == FORMAT_TEMPL_VAR){
            if(templ->type.state & DEBUG){
                Abstract *args[] = {
                    (Abstract *)sg,
                    NULL
                };
                Out("^c.Var: &^0.\n", args);
            }
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
            if(templ->type.state & DEBUG){
                Abstract *args[] = {
                    (Abstract *)sg,
                    NULL
                };
                Out("^c.For: &^0.\n", args);
            }
            Iter *it = From_GetIter(sm->m, data, sg->val.ptr);
            Iter_Add(&templ->data,
                (Abstract *)it);
            if((Iter_Next(it) & END) == 0){
                Iter_Add(&templ->data, (Abstract *)Iter_Get(it));
            }
            Iter_Set(&templ->content,
                TemplJump_Make(sm->m, sg->objType.of, templ->content.idx, NEGATIVE));
        }else if(sg->objType.of == FORMAT_TEMPL_VAR_BODY){
            if(templ->type.state & DEBUG){
                Abstract *args[] = {
                    (Abstract *)sg,
                    NULL
                };
                Out("^c.VarBody: &^0.\n", args);
            }
        }else if(sg->objType.of == FORMAT_TEMPL_VAR_PATH_SEP){
            if(templ->type.state & DEBUG){
                Abstract *args[] = {
                    (Abstract *)sg,
                    NULL
                };
                Out("^c.PathSep: &^0.\n", args);
            }
        }else if(sg->objType.of == FORMAT_TEMPL_VAR_ATT_SEP){
            if(templ->type.state & DEBUG){
                Abstract *args[] = {
                    (Abstract *)sg,
                    NULL
                };
                Out("^c.AttSep: &^0.\n", args);
            }
        }else if(sg->objType.of > _FORMAT_TEMPL_LOGIC_END){
            if(templ->type.state & DEBUG){
                Abstract *args[] = {
                    (Abstract *)sg,
                    NULL
                };
                Out("^c.Logic End: &^0.\n", args);
            }
            Iter _it;
            memcpy(&_it, &templ->content, sizeof(Iter));
            i32 idx = templ->content.idx;

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
