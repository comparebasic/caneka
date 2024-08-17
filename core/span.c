#include <external.h>
#include <caneka.h>

static status Span_Extend(SlabResult *sr);
static status span_GrowToNeeded(SlabResult *sr);

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
    sr->dimsNeeded = SpanDef_GetDimNeeded(p->def, (idx+1));

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
        span_GrowToNeeded(sr);
    }
    return Span_Extend(sr);
}

status Span_Set(Span *p, int idx, Abstract *t){
    SlabResult sr;
    SlabResult_Setup(&sr, p, SPAN_OP_SET, idx);
    status r = Span_Query(&sr);
    if(HasFlag(r, SUCCESS)){
        void *ptr = Slab_valueAddr(&sr, p->def, sr.local_idx);
        if(HasFlag(p->def->flags, RAW)){
            memcpy(ptr, t, (size_t)p->def->itemSize);
        }else{
            memcpy(ptr, &t, sizeof(void *));
        }
        p->nvalues++;
        if(idx > p->max_idx){
            p->max_idx = idx;
        }
        return SUCCESS;
    }
    return r;
}

void *Span_Get(Span *p, int idx){
    SlabResult sr;
    SlabResult_Setup(&sr, p, SPAN_OP_GET, idx);
    status r = Span_Query(&sr);
    if(HasFlag(r, SUCCESS)){
        void *ptr = Slab_valueAddr(&sr, p->def, sr.local_idx);
        if(HasFlag(p->def->flags, RAW)){
            sr.value = ptr;
        }else if(*((Abstract **)ptr) != NULL){
            void **dptr = (void **)ptr;
            sr.value = *dptr;
        }
        return sr.value;
    }else{
        return NULL;
    }
}

int Span_Add(Span *p, Abstract *t){
    int idx = Span_NextIdx(p);
    if(Span_Set(p, idx, t) == SUCCESS){
        return p->max_idx;
    }

    return 0;
}

void *Span_ReserveNext(Span *p){
    int idx = Span_NextIdx(p);
    SlabResult sr;
    SlabResult_Setup(&sr, p, SPAN_OP_RESERVE, idx);
    status r = Span_Query(&sr);
    if(HasFlag(r, SUCCESS)){
        return sr.value;
    }else{
        return NULL;
    }
}

status Span_Remove(Span *p, int idx){
    SlabResult sr;
    SlabResult_Setup(&sr, p, SPAN_OP_REMOVE, idx);
    return Span_Query(&sr);
}

status Span_Move(Span *p, int fromIdx, int toIdx){
    return NOOP;
}

/* internals */
static status span_GrowToNeeded(SlabResult *sr){
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
    while(dims > 0){
        int increment = Span_availableByDim(dims, p->def->stride, p->def->idxStride);
        sr->local_idx = ((sr->idx - sr->offset) / increment);
        sr->offset += increment*sr->local_idx;
        if(sr->local_idx >= p->def->idxStride){
            Fatal("local_idx greater than idxStride", p->type.of);
        }

        /* find or allocate a space for the new span */
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

static SpanDef *SpanDef_FromCls(word cls){
    if(cls == TYPE_SPAN){
        return Span16_MakeDef();
    }else if(cls == TYPE_MINI_SPAN){
        return Span4x16_MakeDef();
    }else if(cls == TYPE_QUEUE_SPAN){
        return Span16x32m_MakeDef();
    }else if(cls == TYPE_SLAB_SPAN){
        return Span4kx32m_MakeDef();
    }else if(cls == TYPE_STRING_SPAN){
        return SpanString_MakeDef();
    }else{
        return Span16_MakeDef();
    }
}

void *Span_valueSlab_Make(MemCtx *m, SpanDef *def){
    i64 sz = sizeof(Abstract *)*def->stride*def->slotSize;
    return MemCtx_Alloc(m, sz);
}

void *Span_idxSlab_Make(MemCtx *m, SpanDef *def){
    i64 sz = sizeof(Abstract *)*(def->idxSize)*def->idxStride;
    return MemCtx_Alloc(m, sz);
}

void *Span_reserve(SlabResult *sr){
    return NULL;
}

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

SpanDef *SpanDef_Clone(MemCtx *m, SpanDef *_def){
    SpanDef *def = (SpanDef *)MemCtx_Alloc(m, sizeof(SpanDef));
    memcpy(def, _def, sizeof(SpanDef));
    return def;
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
    p->type.state |= RAW;
    p->def->flags |= RAW;
    p->root = Span_valueSlab_Make(m, p->def);
    return p;
}
