status Iter_AddWithGaps(Iter *it, MemPage *pg){
    Abstract *value = it->value;
    i32 idx = it->idx;
    i8 dimP = it->p->dims+1;
    Iter prevIt;
    i32 prevIdx = -1;

    if((it->p->max_idx & _modulos[dimP]) == _modulos[dimP]){
        it->type.state = (it->type.state & NORMAL_FLAGS) | SPAN_OP_RESERVE;
        it->idx = it->p->max_idx+1;
        it->value = NULL;
        _Iter_QueryPage(it, pg);
        dimP = it->p->dims+1;
    }

    it->type.state = (it->type.state & NORMAL_FLAGS) | SPAN_OP_GET;
    _Iter_QueryPage(it, pg);
    i8 dim = 0;
    void *sl = NULL;
    void **last = NULL;
    void **ptr = NULL;
    i64 openSlots = -1;
    if(it->type.state & SUCCESS){
        it->type.state &= ~SUCCESS;

        if(it->p->dims == 0){
            i32 localIdx = it->stackIdx[dim];
            ptr = it->stack[dim];
            last = ptr+((_capacity[0]-1)-localIdx);
            openSlots = (it->p->max_idx+1) - it->idx;
        }else{
            while((it->type.state & SUCCESS) == 0){
                i32 localIdx = it->stackIdx[dim];
                ptr = it->stack[dim];
                last = ptr+((_capacity[0]-1)-localIdx);
                while(*last == NULL){
                    last--;
                    if((openSlots = last-ptr) == 0){
                        break;
                    }
                }

                if(openSlots > 0){
                    break;
                }
                if(dim+1 > it->p->dims){
                    break;
                }
                dim++;
            }
        }

        if(openSlots >= 0){
            memmove(ptr+1, ptr, (openSlots+1)*sizeof(void *));
            *ptr = NULL;
            it->p->max_idx += _increments[dim];
            prevIdx = it->idx + _increments[dim];
            it->type.state |= SUCCESS;
        }
    }

    it->type.state = (it->type.state & NORMAL_FLAGS) | SPAN_OP_SET;
    it->value = value;
    it->idx = idx;
    _Iter_QueryPage(it, pg);
    if(prevIdx != -1){
        Iter_Setup(&prevIt, it->p, SPAN_OP_GET, prevIdx);
        _Iter_QueryPage(&prevIt, pg);
        while(dim > 0 && (it->type.state & SUCCESS)){
            sl = *((void **)prevIt.stack[dim]);
            openSlots = it->stackIdx[dim-1];
            if(sl != NULL && openSlots > 0){
                void *destSl = *((void **)it->stack[dim]);
                memcpy(destSl, sl, (openSlots)*sizeof(void *));
                memset(sl, 0, (openSlots)*sizeof(void *));
            }
            dim--;
        }
    }
    Iter_Setup(it, it->p, SPAN_OP_SET, it->idx);
    return it->type.state;
}

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
