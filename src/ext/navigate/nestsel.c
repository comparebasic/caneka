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
    it->objType.state |= UFLAG_ITER_INDENT;

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
    _it->objType.state &= ~(
        UFLAG_ITER_SELECTED|UFLAG_ITER_INDENT|UFLAG_ITER_OUTDENT|UFLAG_ITER_LEAF);

    if(_it->type.state & END){
        _it->type.state &= ~(END|PROCESSING);
    }
    Iter *it = NULL;
    if((_it->type.state & PROCESSING) == 0){
        Iter_Next(_it);
        it = Iter_Get(_it);
        _it->objType.state |= UFLAG_ITER_INDENT;
    }else{
        while(1){
            it = Iter_Get(_it);
            if(it->type.state & END){
                printf("Prev\n");
                fflush(stdout);
                if(Iter_Prev(_it) & END){
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
                if((_it->objType.state & UFLAG_ITER_OUTDENT) == 0){
                    _it->objType.state |= (UFLAG_ITER_SELECTED|UFLAG_ITER_INDENT);
                }
            }

            break;
        }
    }

    Iter_Next(it);
    if(_it->type.state & DEBUG){
        void *ar[] = {it, Iter_Get(it), NULL};
        Out("^c.Next @ @^0\n", ar);
    }
    if(it->type.state & END){
        return NestSel_Next(_it);
    }

    if(_it->type.state & LAST){
        _it->objType.state |= UFLAG_ITER_LEAF;
        if(it->metrics.selected == it->idx){
            _it->objType.state |= UFLAG_ITER_SELECTED;
        }else{
            _it->objType.state &= ~UFLAG_ITER_SELECTED;
        }
    }

    if(_it->idx == 0 && (it->type.state & END)){
        _it->type.state |= END;
        return END;
    }

    if(it->type.state & LAST){
        _it->objType.state |= UFLAG_ITER_OUTDENT;
    }

    return ZERO;
}
