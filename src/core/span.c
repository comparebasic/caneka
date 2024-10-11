#include <external.h>
#include <caneka.h>

static status Span_Extend(SlabResult *sr);

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

/* DEBUG */


/* API */
void SlabResult_Setup(SlabResult *sr, Span *p, byte op, int idx){
    memset(sr, 0, sizeof(SlabResult));

    sr->op = op;
    sr->m = p->m;
    sr->span = p;
    sr->dims = p->dims;
    sr->slab = p->root;
    sr->idx = idx;
    sr->dimsNeeded = SpanDef_GetDimNeeded(p->def, (idx));

    return;
}

status Span_Query(SlabResult *sr){
    MemCtx *m = sr->span->m;
    if(sr->dimsNeeded > sr->dims){
        if(sr->op == SPAN_OP_GET){
            return MISS;
        }
        if(sr->op != SPAN_OP_SET && sr->op != SPAN_OP_RESERVE){
            return MISS;
        }
        Span_GrowToNeeded(sr);
    }
    return Span_Extend(sr);
}
void *_span_Set(SlabResult *sr, int idx, Abstract *t){
    status r = READY;
    Span *p = sr->span;
    if(HasFlag(r, SUCCESS)){
        void *ptr = Slab_valueAddr(sr, p->def, sr->local_idx);
        if(HasFlag(p->def->flags, INLINE)){
            size_t sz = (size_t)p->def->itemSize;
            if(!HasFlag(p->def->flags, RAW) && t->type.of == TYPE_RESERVE){
                sz = sizeof(Reserve);
            }
            memcpy(ptr, t, sz);
        }else{
            memcpy(ptr, &t, sizeof(void *));
        }
        p->nvalues++;
        p->metrics.set = sr->local_idx;
        if(idx > p->max_idx){
            p->max_idx = idx;
        }
        sr->value = ptr;
        return sr->value;
    }
    return NULL;

}

void *Span_Set(Span *p, int idx, Abstract *t){
    if(idx < 0 || t == NULL){
        return NULL;
    }
    SlabResult sr;
    SlabResult_Setup(&sr, p, SPAN_OP_SET, idx);
    status r = Span_Query(&sr);
    return _span_Set(&sr, idx, t);
}

void *Span_Get(Span *p, int idx){
    if(idx < 0){
        return NULL;
    }
    SlabResult sr;
    SlabResult_Setup(&sr, p, SPAN_OP_GET, idx);
    status r = Span_Query(&sr);
    if(HasFlag(r, SUCCESS)){
        void *ptr = Slab_valueAddr(&sr, p->def, sr.local_idx);
        if(HasFlag(p->def->flags, INLINE)){
            if(!HasFlag(p->def->flags, RAW) && (*(util *)ptr) == 0){
                sr.value = NULL;
            }else{
                sr.value = ptr;
            }
        }else if(*((Abstract **)ptr) != NULL){
            void **dptr = (void **)ptr;
            sr.value = *dptr;
        }
        p->metrics.get = sr.local_idx;
        return sr.value;
    }else{
        return NULL;
    }
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

void *Span_ReserveNext(Span *p){
    Reserve rsv;
    memset(&rsv, 0, sizeof(Reserve));
    rsv.type.of = TYPE_RESERVE;
    return Span_Set(p, Span_NextIdx(p), (Abstract *)&rsv);
}

status Span_Remove(Span *p, int idx){
    SlabResult sr;
    SlabResult_Setup(&sr, p, SPAN_OP_REMOVE, idx);
    return Span_Query(&sr);
}

status Span_Move(Span *p, int fromIdx, int toIdx){
    return NOOP;
}

/* utils */
Span *Span_From(MemCtx *m, int count, ...){
    Span *p = Span_Make(m, TYPE_SPAN);
    Abstract *v = NULL;
	va_list arg;
    va_start(arg, count);
    for(int i = 0; i < count; i++){
        v = va_arg(arg, AbstractPtr);
        Span_Add(p, v);
    }
    return p;
}

status Span_Run(MemHandle *mh, Span *p, DoFunc func){
    status r = READY;
    MHAbstract ma;
    MemCtx *m = NULL;
    if(mh != NULL){
        mh = asIfc(mh, TYPE_MEMHANDLE);
        m = mh->m;
    }
    MHAbstract_Init(&ma, m, NULL);
    for(int i = 0; i < p->nvalues; i++){
        Abstract *a = (Abstract *)Span_Get(p, i);
        if(a != NULL && (*(util *)a) != 0){
            ma.a = a;
            r |= func((MemHandle *)&ma);
        }
    }
    return r;
}

/* internals */
status Span_GrowToNeeded(SlabResult *sr){
    boolean expand = sr->span->dims < sr->dimsNeeded;
    SpanDef *def = sr->span->def;
    Span *p = sr->span;

    if(expand){
        void *exp_sl = NULL;
        void *shelf_sl = NULL;
        while(p->dims < sr->dimsNeeded){
            void *new_sl = Span_idxSlab_Make(p->m, def);

            if(exp_sl == NULL){
                shelf_sl = sr->span->root;
                sr->span->root = new_sl;
            }else{
                Slab_setSlot(exp_sl, p->def, 0, &new_sl, sizeof(void *));
            }

            exp_sl = new_sl;
            p->dims++;
        }
        Slab_setSlot(exp_sl, p->def, 0, &shelf_sl, sizeof(void *));
    }

    sr->slab = sr->span->root;
    return SUCCESS;
}

static status Span_Extend(SlabResult *sr){
    /* resize the span by adding dimensions and slabs as needed */
    Span *p = sr->span;

    byte dims = p->dims;
    void *prev_sl = sr->slab;
    /*
    SlabResult_SetStack(sr, sr->slab, 0);
    */
    while(dims > 0){
        int increment = Span_availableByDim(dims, p->def->stride, p->def->idxStride);
        sr->local_idx = ((sr->idx - sr->offset) / increment);
        sr->offset += increment*sr->local_idx;
        if(sr->local_idx >= p->def->idxStride){
            Fatal("local_idx greater than idxStride", p->type.of);
        }

        /* find or allocate a space for the new span */
        /*
        SlabResult_SetStack(sr, Slab_nextSlotPtr(sr, p->def, sr->local_idx), sr->span->dims - dims+1);
        */
        sr->slab = (void *)Slab_nextSlot(sr, p->def, sr->local_idx);

        /* make new if not exists */
        if(sr->slab == NULL){
            if(sr->op != SPAN_OP_SET && sr->op != SPAN_OP_RESERVE){
                return MISS;
            }
            void *new_sl = NULL;
            if(dims > 1){
                new_sl = Span_idxSlab_Make(p->m, p->def); 
            }else{
                new_sl = Span_valueSlab_Make(p->m, p->def); 
            }
            Slab_setSlot(prev_sl, p->def, sr->local_idx, &new_sl, sizeof(void *));
            prev_sl = sr->slab = new_sl;
        }else{
            prev_sl = sr->slab;
        }

        dims--;
    }

    /* conclude by setting the local idx and setting the state */
    sr->local_idx = (sr->idx - sr->offset);

    if(sr->local_idx >= p->def->stride){
        Fatal("local_idx greater than stride", p->type.of);
    }

    sr->type.state = SUCCESS;
    return sr->type.state;
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

void *Span_reserve(SlabResult *sr){
    return NULL;
}

status Span_ReInit(Span *p){
    p->nvalues = 0;
    p->max_idx = p->metrics.get = p->metrics.selected = p->metrics.set = -1;
    return SUCCESS;
}

static void *span_CloneSlabs(MemCtx *m, void *p_sl, Span *p, int dim){
    SpanDef *def = p->def;
    if(dim == 0){
       return MemCtx_Realloc(m, SPAN_VALUE_SIZE(p->def), p_sl, SPAN_VALUE_SIZE(p->def)); 
    }else{
        void **new_sl = MemCtx_Alloc(m, SPAN_IDX_SIZE(def));
        for(int local_idx = 0; local_idx < def->idxStride; local_idx++){
            int pos = local_idx*(def->slotSize)*sizeof(void *);
            void *ptr = p_sl+local_idx;
            util *a = (util *)ptr;
            if(*a != 0){
                void *copied_sl = span_CloneSlabs(m, *((void **)ptr), p, dim-1);
                new_sl[pos] = copied_sl;
            }
        }
        return new_sl;
    }
}

Span *Span_Clone(MemCtx *m, Span *p){
    Span *copy;
    copy->root = span_CloneSlabs(m, p->root, p, p->dims);
    return copy;
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

Span* Span_MakeInline(MemCtx* m, cls type, int itemSize){
    Span *p = MemCtx_Alloc(m, sizeof(Span));
    p->m = m;
    p->def = SpanDef_Clone(m, SpanDef_FromCls(type));
    p->type.of = p->def->typeOf;
    p->max_idx = -1;

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
    p->def->flags |= INLINE;
    p->root = Span_valueSlab_Make(m, p->def);
    return p;
}
