#include <external.h>
#include <caneka.h>

status NestSel_Init(Iter *it, Inst *inst, Span *coords){
    status r = READY;
    if(it->p == NULL){
        Iter_Init(it, Span_Make(inst->m));
    }
    
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

    return r;
}

void *NestSel_Get(Iter *_it){
    Iter *it = Iter_Get(_it);
    if(it == NULL){
        printf("it is null\n");
        fflush(stdout);
        return NULL;
    }
    return it->value;
}

status NestSel_Next(Iter *_it){
    if(_it->type.state & END){
        _it->type.state &= ~(END|PROCESSING);
    }
    Iter *it = NULL;
    if((_it->type.state & PROCESSING) == 0){
        Iter_Next(_it);
        it = Iter_Get(_it);
    }else{
        it = Iter_Get(_it);
        if((_it->type.state & (LAST|END)) == 0 && it->metrics.selected == it->idx){
            it->metrics.selected = -1;
            Iter_Next(_it);
            it = Iter_Get(_it);
            if(_it->type.state & LAST){
                Iter_First(it);
            }
        }else if(it->type.state & END){
            return END;
        }
    }

    Iter_Next(it);
    if(_it->type.state & LAST){
        if(it->metrics.selected == it->idx){
            _it->type.state |= MORE;
        }else{
            _it->type.state &= ~MORE;
        }
        if(it->type.state & END){
            return END;
        }
    }

    return ZERO;
}
