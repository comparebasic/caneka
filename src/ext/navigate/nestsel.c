#include <external.h>
#include <caneka.h>

status NestSel_Init(Iter *it, Inst *inst, Span *coords){
    status r = READY;
    if(it->p == NULL){
        Iter_Init(it, Span_Make(inst->m));
    }

    if(it->itin == NULL){
        it->itin = Itin_Make(inst->m, inst->type.of);
    }
    
    Table *children = Span_Get(inst, INST_PROPIDX_CHILDREN);

    Iter _it;
    Iter_Init(&_it, coords);
    while((Iter_Next(&_it) & END) == 0){
        Single *sg = Iter_Get(&_it);

        Iter *cur = Iter_GetByIdx(it, _it.idx);
        if(cur == NULL){
            cur = ItinIt_Make(inst->m, Table_Ordered(inst->m, children));
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
        printf("it null\n");
        fflush(stdout);
        return NULL;
    }
    return it->value;
}

status NestSel_Next(Iter *_it){
    _it->itin->objType.state &= ~(UFLAG_ITER_FOCUS|UFLAG_ITER_LEAF);

    if(_it->type.state & END){
        _it->type.state &= ~(END|PROCESSING);
    }
    Iter *it = NULL;
    if((_it->type.state & PROCESSING) == 0){
        Iter_Next(_it);
        it = Iter_Get(_it);
    }else{
        while(1){
            it = Iter_Get(_it);
            if(it->type.state & END){
                if(Iter_Prev(_it) & END){
                    _it->type.state |= END;
                    return END;
                }
                continue;
            }

            if(it->metrics.selected == it->idx &&
                    (_it->type.state & (LAST|END)) == 0){
                it->metrics.selected = -1;
                Iter_Next(_it);
                it = Iter_Get(_it);
                if(_it->type.state & LAST){
                    Iter_First(it);
                }
            }

            break;
        }
    }

    if((it->type.state & END) == 0){
        Iter_Next(it);
    }

    if(it->type.state & END){
        if(_it->idx <= 0){
            _it->type.state |= END;
            return END;
        }
        return NestSel_Next(_it);
    }

    if(_it->type.state & LAST){
        _it->itin->objType.state |= UFLAG_ITER_LEAF;
        if(it->metrics.selected == it->idx){
            _it->itin->objType.state |= UFLAG_ITER_FOCUS;
        }else{
            _it->itin->objType.state &= ~UFLAG_ITER_FOCUS;
        }
    }

    return ZERO;
}
