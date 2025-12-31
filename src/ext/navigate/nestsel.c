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
            cur = Iter_Make(inst->m, children);
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
        return NULL;
    }
    return it->value;
}

status NestSel_Next(Iter *_it){
    if(_it->type.state & END){
        _it->type.state &= ~(END|PROCESSING);
    }
    if((_it->type.state & PROCESSING) == 0){
        Iter_Next(_it);
    }

    void *args[] = {
        Type_StateVec(_it->p->m, _it->type.of, _it->type.state),
        I32_Wrapped(_it->p->m, _it->idx),
        NULL
    };
    Out("^y.State @ Idx @^0\n", args);

    Iter *it = Iter_Get(_it);
    if(it->type.state & END){
        Iter_Prev(_it);
        it = Iter_Get(_it);
    }else if(it->metrics.selected == it->idx){
        if(_it->type.state & END){
            return END;
        }
        Iter_Next(_it);
        it = Iter_Get(_it);
    }

    return Iter_Next(it);
}
