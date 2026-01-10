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
        return NULL;
    }

    void *ar[] = {it->value, NULL};
    Out("^b.NestSel_Get(@)^0\n", ar);

    return it->value;
}

status NestSel_Next(Iter *_it){
    _it->objType.state &= ~(
        UFLAG_ITER_SELECTED|UFLAG_ITER_INDENT|UFLAG_ITER_OUTDENT|UFLAG_ITER_LEAF);

    void *ar[] = {
        Type_StateVec(ErrStream->m, TYPE_ITER_UPPER, _it->objType.state), NULL};
    Out("^y.     Starting NestSelNext Fl @^0\n", ar);

    if(_it->type.state & END){
        _it->type.state &= ~(END|PROCESSING);
        printf("END WIPING PROCESSING\n");
        fflush(stdout);
    }
    Iter *it = NULL;
    if((_it->type.state & PROCESSING) == 0){
        Iter_Next(_it);
        it = Iter_Get(_it);
        _it->objType.state |= UFLAG_ITER_INDENT;
        printf("INDENT START\n");
        fflush(stdout);
    }else{
        while(1){
            it = Iter_Get(_it);
            if(it->type.state & END){
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
                printf("INDENT SELECTED\n");
                fflush(stdout);
            }

            break;
        }
    }

    Iter_Next(it);
    if(_it->type.state & LAST){
        _it->objType.state |= UFLAG_ITER_LEAF;
        if(it->metrics.selected == it->idx){
            _it->objType.state |= UFLAG_ITER_SELECTED;
        }else{
            _it->objType.state &= ~UFLAG_ITER_SELECTED;
        }
    }

    if(_it->idx == 0&& (it->type.state & END)){
        _it->type.state |= END;
        return END;
    }

    if(it->type.state & LAST){
        _it->objType.state |= UFLAG_ITER_OUTDENT;
    }

    return ZERO;
}
