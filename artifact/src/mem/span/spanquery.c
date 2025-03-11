void SpanQuery_Setup(SpanQuery *sr, Span *p, byte op, int idx){
    memset(sr, 0, sizeof(SpanQuery));

    sr->type.of = TYPE_SPAN_QUERY;
    sr->op = op;
    sr->m = p->m;
    sr->span = p;
    sr->dims = p->dims;
    sr->idx = idx;
    sr->dimsNeeded = Span_GetDimNeeded(idx);

    return;
}

SpanState *SpanQuery_SetStack(SpanQuery *sq, i8 dim){
    Span *p = sq->span; 
    slab *sl = NULL;
    word localIdx = 0;
    SpanState *st = sq->stack+dim;
    i32 increment = _increments[dim];
    if(dim == p->dims){
        sl = p->root;
        localIdx = sq->idx / increment;
        if(dim == 0){
            st->offset = 0;
        }else{
            st->offset = localIdx * increment;
        }
    }else{
        SpanState *prev = sq->stack+(dim+1);
        localIdx = ((sq->idx - prev->offset) / increment);
        word localMax = SPAN_STRIDE;
        st->offset = prev->offset + increment*localIdx;
        sl = (slab *)Slab_GetSlot(prev->slab, prev->localIdx);
    }
    st->slab = sl;
    st->localIdx = localIdx;
    st->dim = dim;
    st->increment = increment;

    return st;
}

slab *Slab_WhileExpanding(MemSlab **_sl){
    MemSlab *sl = *_sl;
    if(sl == NULL){
        void *bytes = MemChapter_GetBytes();
        if(bytes == NULL){
            return NULL;
        }
        MemSlab inl = {
            .type = {TYPE_MEMSLAB, 0},
            .level = 0,
            .remaining = MEM_SLAB_SIZE,
            .bytes = bytes,
        };
        *_sl = sl = MemSlab_Alloc(&inl, sizeof(MemSlab));
        memcpy(sl, &inl, sizeof(MemSlab));
    }
    return MemSlab_Alloc(sl, sizeof(void *)*SPAN_STRIDE);
}

status Span_Query(SpanQuery *sr){
    MemCtx *m = sr->span->m;
    i32 idx = sr->idx;
    Span *p = sr->span;
    /* increase the dims with blank slabs if necessary */
    if(sr->dimsNeeded > sr->dims){
        if(sr->op != SPAN_OP_SET && sr->op != SPAN_OP_RESERVE){
            return NOOP;
        }
        slab *exp_sl = NULL;
        slab *shelf_sl = NULL;
        MemSlab *sl = NULL;
        while(p->dims < sr->dimsNeeded){
            slab *new_sl = NULL;
            if(p == &p->m->p){
                new_sl = Slab_WhileExpanding(&sl);
            }else{
                new_sl = Slab_Make(p->m);
            }

            if(exp_sl == NULL){
                shelf_sl = sr->span->root;
                sr->span->root = new_sl;
            }else{
                void **ptr = (void *)exp_sl;
                *ptr = new_sl;
            }

            exp_sl = new_sl;
            p->dims++;
        }
        void **ptr = (void *)exp_sl;
        *ptr = shelf_sl;
    }
    sr->dims = p->dims;

    /* resize the span by adding dimensions and slabs as needed */
    byte dims = p->dims;
    SpanState *st = NULL;
    while(TRUE){
        /* make new if not exists */
        st = SpanQuery_SetStack(sr, dims);
        if(st->slab == NULL){
            if(sr->op != SPAN_OP_SET && sr->op != SPAN_OP_RESERVE){
                return NOOP;
            }
            slab *new_sl = Slab_Make(p->m); 
            SpanState *prev = sr->stack+(dims+1);
            void **ptr = (void **)prev->slab;
            ptr += prev->localIdx;
            *ptr = new_sl;
            st->slab = new_sl;
        }else{
            sr->queryDim = dims;
        }

        /* find or allocate a space for the new span */
        if(dims == 0){
            break;
        }

        dims--;
    }

    if(st == NULL){
        Fatal("Slab not found, SpanState is null\n", TYPE_SPAN);
    }

    if(st->localIdx >= SPAN_STRIDE){
        SpanState_Print(st, 0, 31);
        printf("\n");
        Fatal("localIdx greater than stride", p->type.of);
    }

    sr->type.state |= SUCCESS;
    return sr->type.state;
}
