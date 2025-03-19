#include <external.h>
#include <caneka.h>

static i32 _increments[SPAN_MAX_DIMS] = {1, 16, 256, 4096, 65536};

static inline SpanState *SpanQuery_SetStack(SpanQuery *sq, i8 dim){
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
        st->offset = prev->offset;
        if(dim > 0){
            st->offset += increment*localIdx;
        }
        void **ptr = (void **)prev->slab;
        ptr += localIdx;
        if(ptr != NULL){
            sl = *ptr;
        }else{
            sl = NULL;
        }
    }
    st->slab = sl;
    st->localIdx = localIdx;
    st->dim = dim;
    st->increment = increment;

    return st;
}

void SpanQuery_Setup(SpanQuery *sr, Span *p, status op, i32 idx){
    memset(sr, 0, sizeof(SpanQuery));
    sr->type.of = TYPE_SPAN_QUERY;
    sr->type.state = op;
    sr->span = p;
    sr->idx = idx;
    return;
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
    sr->type.state &= ~SUCCESS;
    MemCtx *m = sr->span->m;
    i32 idx = sr->idx;

    i8 dimsNeeded = 0;
    while(_increments[dimsNeeded+1] <= idx){
        dimsNeeded++;
    }

    Span *p = sr->span;
    MemSlab *mem_sl = NULL;
    if(dimsNeeded > p->dims){
        if((sr->type.state & (SPAN_OP_SET|SPAN_OP_RESERVE)) == 0){
            return NOOP;
        }
        slab *exp_sl = NULL;
        slab *shelf_sl = NULL;
        while(p->dims < dimsNeeded){
            slab *new_sl = NULL;
            if(p == &p->m->p){
                new_sl = Slab_WhileExpanding(&mem_sl);
            }else{
                new_sl = (slab *)MemCtx_Alloc((m), sizeof(slab));
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

    i8 dims = p->dims;
    SpanState *st = NULL;
    while(dims != -1){
        st = SpanQuery_SetStack(sr, dims);
        if(st->slab == NULL){
            if((sr->type.state & (SPAN_OP_SET|SPAN_OP_RESERVE)) == 0){
                return NOOP;
            }
            slab *new_sl = NULL; 
            if(p == &p->m->p){
                new_sl = Slab_WhileExpanding(&mem_sl);
            }else{
                new_sl = (slab *)MemCtx_Alloc((m), sizeof(slab));
            }
            SpanState *prev = sr->stack+(dims+1);
            void **ptr = (void **)prev->slab;
            ptr += prev->localIdx;
            *ptr = new_sl;
            st->slab = new_sl;
        }

        dims--;
    }

    if(st == NULL){
        Fatal("Slab not found, SpanState is null\n", TYPE_SPAN);
    }

    if(st->localIdx >= SPAN_STRIDE){
        Fatal("localIdx greater than stride", p->type.of);
    }

    sr->type.state |= SUCCESS;
    return sr->type.state;
}


i32 Span_Capacity(Span *p){
    i32 increment = _increments[p->dims];
    return increment * SPAN_STRIDE;
}

char **Span_ToCharArr(MemCtx *m, Span *p){
    size_t sz = sizeof(char *)*(p->nvalues+1);
    char **arr = MemCtx_Alloc(m, sz);
    /*
    Iter it;
    Iter_Init(&it, p);
    int i = 0;
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)asIfc(Iter_Get(&it), TYPE_STR);
        if(s != NULL){
            arr[i++] = Str_ToCstr(m, s);
        }
    }
    */
    return arr;
}

void *Span_SetFromQ(SpanQuery *sq, Abstract *t){
    Span *p = sq->span;
    status r = Span_Query(sq);
    if((r & SUCCESS) != 0){
        SpanState *st = sq->stack;
        if(sq->idx > p->max_idx+1){
            p->type.state |= FLAG_SPAN_HAS_GAPS;
        }
        void **ptr = (void **)st->slab;
        ptr +=st->localIdx;
        if(sq->type.state & SPAN_OP_REMOVE){
            *ptr = NULL;
            p->nvalues--;
            p->metrics.set = -1;
            p->metrics.available = sq->idx;
        }else{
            *ptr = (void *)t;
            p->nvalues++;
            p->metrics.set = sq->idx;
            if(sq->idx > p->max_idx){
                p->max_idx = sq->idx;
            }
        }

        sq->value = (Abstract *)*ptr;
        return sq->value;
    }

    if(p->type.state & DEBUG){
        printf("SetFromQ: Error\n");
    }

    return NULL;
}

void *Span_Set(Span *p, i32 idx, Abstract *t){
    if(idx < 0 || t == NULL){
        return NULL;
    }
    SpanQuery sq;
    SpanQuery_Setup(&sq, p, SPAN_OP_SET, idx);
    return Span_SetFromQ(&sq, t);
}

void *Span_GetFromQ(SpanQuery *sq){
    Span *p = sq->span;
    SpanState *st = sq->stack;
    void **ptr = (void **)st->slab;
    ptr += st->localIdx;
    sq->span->type.state &= ~(SUCCESS|NOOP);
    if(*ptr != NULL){
        sq->value = *ptr;
        sq->span->type.state |= SUCCESS;
    }else{
        sq->span->type.state |= NOOP;
        sq->value = NULL;
    }
    p->metrics.get = sq->idx;
    return sq->value;
}

void *Span_Get(Span *p, i32 idx){
    if(idx < 0){
        return NULL;
    }
    SpanQuery sq;
    SpanQuery_Setup(&sq, p, SPAN_OP_GET, idx);
    status r = Span_Query(&sq);
    if((r & SUCCESS) == 0){
        p->type.state |= ERROR;
        return NULL;
    }
    return Span_GetFromQ(&sq);
}

void *Span_GetSelected(Span *p){
    return Span_Get(p, p->metrics.selected);
}

i32 Span_Add(Span *p, Abstract *t){
    i32 idx = p->max_idx+1;
    if(Span_Set(p, idx, t) != NULL){
        return idx;
    }

    return 0;
}

status Span_Cull(Span *p, i32 count){
    SpanQuery sq;
    while(count-- > 0){
        i32 idx = p->max_idx;
        if(idx >= 0){
            SpanQuery_Setup(&sq, p, SPAN_OP_REMOVE, idx);
            Span_Query(&sq);
        }
        p->max_idx--;
    }

    return NOOP;
}

status Span_Remove(Span *p, i32 idx){
    SpanQuery sq;
    SpanQuery_Setup(&sq, p, SPAN_OP_REMOVE, idx);
    Span_SetFromQ(&sq, NULL);
    return sq.type.state;
}

status Span_ReInit(Span *p){
    p->nvalues = 0;
    p->max_idx = p->metrics.get = p->metrics.selected = p->metrics.set = -1;
    return SUCCESS;
}

status Span_Setup(Span *p){
    p->type.of = TYPE_SPAN;
    p->max_idx = p->metrics.available = \
        p->metrics.get = p->metrics.selected = p->metrics.set = -1;
    return SUCCESS;
}

Span *Span_Make(MemCtx *m){
    Span *p = MemCtx_Alloc(m, sizeof(Span));
    p->type.of = TYPE_SPAN;
    p->max_idx = p->metrics.available = p->metrics.get = p->metrics.selected = p->metrics.set = -1;
    p->m = m;
    p->root = (slab *)MemCtx_Alloc((m), sizeof(slab));
    return p;
}
