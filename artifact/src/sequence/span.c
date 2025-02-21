#include <external.h>
#include <caneka.h>

int Span_availableByDim(int dims, int stride, int idxStride){
    int _dims = dims;
    int n = stride;
    int r = n;
    if(dims <= 0){
        r = 1;
    }else if(dims == 1){
        return stride;
    }else{
        while(dims > 1){
            n *= idxStride;
            dims--;
        }
        r = n;
    }

    return r;
}

int Span_Capacity(Span *p){
    int increment = Span_availableByDim(p->dims, p->def->stride, p->def->idxStride);
    return increment * p->def->idxStride;
}

/* API */
char **Span_ToCharArr(MemCtx *m, Span *p){
    size_t sz = sizeof(char *)*(p->nvalues+1);
    char **arr = MemCtx_Alloc(m, sz);
    memset(arr, 0, sz); 
    Iter it;
    Iter_Init(&it, p);
    int i = 0;
    while((Iter_Next(&it) & END) == 0){
        String *s = (String *)asIfc(Iter_Get(&it), TYPE_STRING);
        if(s != NULL){
            arr[i++] = String_ToChars(m, s);
        }
    }
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
        void *ptr = Slab_valueAddr(st->slab, p->def, st->localIdx);
        if((p->def->flags & SPAN_INLINE) != 0){
            size_t sz = (size_t)p->def->itemSize;
            if((p->def->flags & SPAN_RAW) == 0 && t != NULL && t->type.of == TYPE_RESERVE){
                sz = sizeof(Reserve);
            }
            if(sq->op == SPAN_OP_REMOVE){
                memset(ptr, 0, sz);
            }else{
                memcpy(ptr, t, sz);
            }
        }else{
            if(sq->op == SPAN_OP_REMOVE){
                memset(ptr, 0, sizeof(void *));
            }else{
                memcpy(ptr, &t, sizeof(void *));
            }
        }
        if(sq->op == SPAN_OP_REMOVE){
            p->nvalues--;
        }else{
            p->nvalues++;
        }
        if(sq->op != SPAN_OP_REMOVE){
            p->metrics.set = sq->idx;
        }
        if(sq->idx > p->max_idx){
            p->max_idx = sq->idx;
        }
        sq->value = ptr;
        return sq->value;
    }
    return NULL;

}

void *Span_Set(Span *p, int idx, Abstract *t){
    if(idx < 0 || t == NULL){
        return NULL;
    }
    SpanQuery sq;
    SpanQuery_Setup(&sq, p, SPAN_OP_SET, idx);
    return Span_SetFromQ(&sq, t);
}

void *Span_GetFromQ(SpanQuery *sq){
    Span *p = sq->span;
    SpanDef *def = p->def;

    SpanState *st = sq->stack;
    void *ptr = Slab_valueAddr(st->slab, def, st->localIdx);
    sq->span->type.state &= ~(SUCCESS|NOOP);
    if((def->flags & SPAN_INLINE) != 0){
        if((def->flags & SPAN_RAW) == 0 && (*(util *)ptr) == 0){
            sq->span->type.state |= NOOP;
            sq->value = NULL;
        }else{
            sq->span->type.state |= SUCCESS;
            sq->value = ptr;
        }
    }else if(*((Abstract **)ptr) != NULL){
        void **dptr = (void **)ptr;
        sq->value = *dptr;
        sq->span->type.state |= SUCCESS;
    }else{
        sq->span->type.state |= NOOP;
        sq->value = NULL;
    }
    p->metrics.get = sq->idx;
    return sq->value;
}

void *Span_Get(Span *p, int idx){
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

int Span_Add(Span *p, Abstract *t){
    int idx = Span_NextIdx(p);
    if(Span_Set(p, idx, t) != NULL){
        return idx;
    }

    return 0;
}

status Span_Cull(Span *p, int count){
    SpanQuery sq;
    while(count-- > 0){
        int idx = p->max_idx;
        if(idx >= 0){
            SpanQuery_Setup(&sq, p, SPAN_OP_REMOVE, idx);
            Span_Query(&sq);
        }
        p->max_idx--;
    }

    return NOOP;
}


status Span_Remove(Span *p, int idx){
    SpanQuery sq;
    SpanQuery_Setup(&sq, p, SPAN_OP_REMOVE, idx);
    return Span_Query(&sq);
}

/* internals */
status Span_GrowToNeeded(SpanQuery *sq){
    boolean expand = sq->span->dims < sq->dimsNeeded;
    SpanDef *def = sq->span->def;
    Span *p = sq->span;

    if(expand){
        void *exp_sl = NULL;
        void *shelf_sl = NULL;
        while(p->dims < sq->dimsNeeded){
            void *new_sl = Span_idxSlab_Make(p->m, def);

            if(exp_sl == NULL){
                shelf_sl = sq->span->root;
                sq->span->root = new_sl;
            }else{
                Slab_setSlot(exp_sl, p->def, 0, &new_sl, sizeof(void *));
            }

            exp_sl = new_sl;
            p->dims++;
        }
        Slab_setSlot(exp_sl, p->def, 0, &shelf_sl, sizeof(void *));
    }

    sq->dims = p->dims;

    return SUCCESS;
}

status Span_Extend(SpanQuery *sq){
    /* resize the span by adding dimensions and slabs as needed */
    Span *p = sq->span;

    byte dims = p->dims;
    SpanState *st = NULL;
    while(TRUE){
        /* make new if not exists */
        st = SpanQuery_SetStack(sq, dims, 0, 0);
        if(st->slab == NULL){
            if(sq->op != SPAN_OP_SET && sq->op != SPAN_OP_RESERVE){
                return NOOP;
            }
            void *new_sl = NULL;
            if(dims > 1){
                new_sl = Span_idxSlab_Make(p->m, p->def); 
            }else{
                new_sl = Span_valueSlab_Make(p->m, p->def); 
            }
            SpanState *prev = SpanQuery_StateByDim(sq, dims+1);
            Slab_setSlot(prev->slab, p->def, prev->localIdx, &new_sl, sizeof(void *));
            st->slab = new_sl;
        }else{
            sq->queryDim = dims;
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

    if(st->localIdx >= p->def->stride){
        Fatal("localIdx greater than stride", p->type.of);
    }

    sq->type.state |= SUCCESS;
    return sq->type.state;
}

byte SpanDef_GetDimNeeded(SpanDef *def, int idx){
    if(idx < def->stride){
        return 0;
    }

    int nslabs = idx / def->stride;
    if(idx % nslabs > 0){
        nslabs++;
    }
    int dims = 1;
    while(Span_availableByDim(dims, def->stride, def->idxStride) < nslabs){
        dims++;
    }

    return dims;
}

void *Span_valueSlab_Make(MemCtx *m, SpanDef *def){
    i64 sz = SPAN_VALUE_SIZE(def);
    return MemCtx_Alloc(m, sz);
}

void *Span_idxSlab_Make(MemCtx *m, SpanDef *def){
    i64 sz = SPAN_IDX_SIZE(def);
    return MemCtx_Alloc(m, sz);
}

status Span_ReInit(Span *p){
    p->nvalues = 0;
    p->max_idx = p->metrics.get = p->metrics.selected = p->metrics.set = -1;
    return SUCCESS;
}

Span* Span_MakeInline(MemCtx* m, cls type, int itemSize){
    Span *p = MemCtx_Alloc(m, sizeof(Span));
    p->m = m;
    p->def = SpanDef_Clone(m, SpanDef_FromCls(type));
    p->type.of = p->def->typeOf;
    p->max_idx = p->metrics.get = p->metrics.selected = p->metrics.set = -1;

    p->def->itemSize = itemSize;
    int slotSize = 1;
    if(itemSize > sizeof(void *)){
        slotSize = itemSize / sizeof(void *);
        if(itemSize % sizeof(void *) > 1){
            slotSize += 1;
        }
    }
    int pwrSlot = p->def->stride;
    while((pwrSlot / 2) >= slotSize){
        pwrSlot /= 2;
    }

    p->def->slotSize = pwrSlot;
    p->def->flags |= SPAN_INLINE;
    p->root = Span_valueSlab_Make(m, p->def);
    return p;
}

Span *Span_Make(MemCtx *m, cls type){
    Span *p = MemCtx_Alloc(m, sizeof(Span));
    p->m = m;
    p->def = SpanDef_FromCls(type);
    p->type.of = p->def->typeOf;
    p->root = Span_valueSlab_Make(m, p->def);
    p->max_idx = -1;
    return p;
}
