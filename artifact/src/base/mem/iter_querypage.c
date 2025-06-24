status _Iter_QueryPage(Iter *it, MemPage *pg){
    it->type.state &= ~(SUCCESS|NOOP);
    MemCh *m = it->p->m;

    if(it->type.state & SPAN_OP_ADD){
        if(it->type.state & FLAG_ITER_CONTINUE){
            return Iter_AddWithGaps(it, pg);
        }
        it->idx = it->p->max_idx+1;
    }

    i8 dimsNeeded = 0;
    while(_increments[dimsNeeded+1] <= it->idx){
        dimsNeeded++;
    }

    Span *p = it->p;
    if(dimsNeeded > p->dims){
        if((it->type.state &
                (SPAN_OP_SET|SPAN_OP_RESERVE|SPAN_OP_ADD|SPAN_OP_RESIZE)) == 0){
            return NOOP;
        }
        slab *exp_sl = NULL;
        slab *shelf_sl = NULL;
        while(p->dims < dimsNeeded){
            slab *new_sl = NULL;
            if(pg != NULL){
                new_sl = (slab *)BytesPage_Alloc(pg, sizeof(slab));
            }else{
                new_sl = (slab *)Bytes_Alloc((m), sizeof(slab));
            }

            if(exp_sl == NULL){
                shelf_sl = it->p->root;
                it->p->root = new_sl;
            }else{
                void **ptr = (void **)exp_sl;
                *ptr = new_sl;
            }

            exp_sl = new_sl;
            p->dims++;
        }
        void **ptr = (void **)exp_sl;
        *ptr = shelf_sl;
    }

    i8 dim = p->dims;
    i32 offset = it->idx;
    void **ptr = NULL;
    while(dim >= 0){
        offset = Iter_SetStack(p->m, pg, it, dim, offset);
        if(it->type.state & NOOP){
            break;
        }
        if(dim == 0){
            if(it->type.state & (SPAN_OP_SET|SPAN_OP_REMOVE|SPAN_OP_ADD)){
                ptr = (void **)it->stack[dim];
                it->type.state |= SUCCESS;
                if(it->type.state & (SPAN_OP_SET|SPAN_OP_ADD)){
                    *ptr = it->value;
                    p->nvalues++;
                    if(it->idx > p->max_idx){
                        p->max_idx = it->idx;
                    }
                }else if(it->type.state & SPAN_OP_REMOVE){
                    *ptr = NULL;
                    p->nvalues--;
                    if(it->idx == p->max_idx){
                        p->max_idx--;
                    }
                }
            }else if(it->type.state & (SPAN_OP_GET|SPAN_OP_RESERVE)){
                ptr = (void **)it->stack[dim];
                if(ptr != NULL){
                    it->value = *ptr;
                    it->type.state |= SUCCESS;
                }else{
                    it->value = NULL;
                    it->type.state |= NOOP;
                }
            }
        }
        dim--;
    }

    if(it->idx == p->max_idx){
        it->type.state |= LAST;
    }else{
        it->type.state &= ~LAST;
    }

    return it->type.state;
}
