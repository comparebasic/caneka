#include <external.h>
#include <caneka.h>

status NestSel_Init(Iter *it, Inst *inst, Span *coords){
    status r = READY;
    if(it->p == NULL){
        Iter_Init(it, Span_Make(inst->m));
    }
    it->objType.of = inst->type.of;
    
    Table *children = Span_Get(inst, INST_PROPIDX_CHILDREN);

    Iter _it;
    Iter_Init(&_it, coords);
    while((Iter_Next(&_it) & END) == 0){
        Single *sg = Iter_Get(&_it);

        Iter *cur = Iter_GetByIdx(it, _it.idx);
        if(cur == NULL){
            cur = Iter_Make(inst->m, Table_Ordered(inst->m, children));
            Iter_Add(it, cur);
        }
        cur->metrics.selected = sg->val.i;
        Iter_First(cur);

        Hashed *h = Iter_GetByIdx(cur, sg->val.i); 
        if(h == NULL){
            Error(inst->m, FUNCNAME, FILENAME, LINENUMBER,
                "Nested item expected and not found", NULL);
            return ERROR;
        }

        children = Span_Get(h->value, INST_PROPIDX_CHILDREN);
        r |= SUCCESS;
    }

    if(r == READY){
        r |= NOOP;
    }

    Iter_First(it);
    it->objType.state |= MORE;

    return r;
}

void *NestSel_Get(Iter *_it){
    Iter *it = Iter_Get(_it);
    if(it == NULL){
        return NULL;
    }
    return it->value;
}

status NestSel_Next(Iter *_it){
    _it->objType.state &= ~MORE;
    if(_it->type.state & END){
        _it->type.state &= ~(END|PROCESSING);
    }
    Iter *it = NULL;
    if((_it->type.state & PROCESSING) == 0){
        Iter_Next(_it);
        it = Iter_Get(_it);
        if((_it->type.state & (LAST|END)) == 0){
            _it->objType.state |= FLAG_ITER_SELECTED;
        }else if(_it->type.state & LAST){
            _it->objType.state &= ~FLAG_ITER_SELECTED;
            if(it->idx == it->metrics.selected){
                it->objType.state |= FLAG_ITER_SELECTED;
            }else{
                it->objType.state &= ~SUCCESS;
            }
        }
        _it->objType.state |= MORE;
    }else{
        it = Iter_Get(_it);
        if((_it->type.state & (LAST|END)) == 0 &&
                it->metrics.selected == it->idx){
            it->metrics.selected = -1;
            Iter_Next(_it);
            it = Iter_Get(_it);
            if(_it->type.state & LAST){
                Iter_First(it);
            }
            _it->objType.state |= (FLAG_ITER_SELECTED|MORE);
        }else if(it->type.state & END){
            return END;
        }else if(it->type.state & LAST){
            _it->objType.state &= ~FLAG_ITER_SELECTED;
            _it->objType.state |= LAST;
            if(it->idx == it->metrics.selected){
                it->objType.state |= SUCCESS;
            }else{
                it->objType.state &= ~SUCCESS;
            }
        }
    }

    Iter_Next(it);
    if(_it->type.state & LAST){
        if(it->metrics.selected == it->idx){
            _it->objType.state |= FLAG_ITER_SELECTED;
        }else{
            _it->objType.state &= ~FLAG_ITER_SELECTED;
        }
        if(it->type.state & END){
            return END;
        }
    }

    _it->objType.state |= (it->type.state & LAST);
    return ZERO;
}
