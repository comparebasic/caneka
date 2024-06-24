#include <basictypes_external.h> 
#include <caneka/caneka.h> 

#define SPAN_DIM_SIZE stride

static Slab *openNewSlab(MemCtx *m, int local_idx, int offset, int dims, int slots_available, status flags){
    Slab *new_sl = Slab_Alloc(m, flags);
    new_sl->stride = stride;
    new_sl->increment = slots_available; 

    new_sl->local_idx = local_idx;
    new_sl->offset = offset;

    return new_sl;
}

static int slotsAvailable(int dims){
    int n = stride;
    int _dims = dims;
    int r = n;
    if(dims <= 0){
        r = 1;
    }else if(dims == 1){
        r = stride;
    }else{
        while(dims > 1){
            n *= stride;
            dims--;
        }
        r = n;
    }

    return r;
}

static status span_GrowToNeeded(MemCtx *m, SlabResult *sr){
    int dims = sr->dims = sr->span->ndims = sr->dimsNeeded;

    int slots_available = slotsAvailable(sr->stride, dims);

    int needed = slots_available;
    boolean expand = sr->slab->increment < needed;
    if(expand){
        slab *exp_sl = opennewslab(m, 0, 0, dims, needed, (sr->span->flags & raw));

        exp_sl->flags |= (sr->span->flags & accept_fl);

        slab *shelf_sl = sr->span->slab;
        sr->span->slab = exp_sl;
        needed /= sr->span->stride;
        while(sr->slab->increment < needed){
            slab *new_sl = makenewslab(m, sr->stride, dims, needed, sr->span->flags & flag_raw);
            exp_sl->items[0] = (typed *)new_sl;
            exp_sl = new_sl;
            needed /= sr->span->stride;
        }
        exp_sl->items[0] = (typed *)shelf_sl;
    }
    return success;
}


Span* Span_Init(MemCtx* m){
    return (Span *)MemCTx_Alloc(m, sizeof(Span));
}

Span* Span_MakeInline(MemCtx* m, cls type, int itemSize){
    int slotSize = 1;
    if(itemSize > sizeof(void *)){
        slotSize = itemSize / sizeof(void *);
        if(itemSize % sizeof(void *) > 1){
            slotSize += 1;
        }
    }
    Span *sp = Span_Make(m, stride*slotSize);
    sp->itemSize = itemSize;
    sp->itemType = type;
    sp->slotSize = slotSize;
    sp->slab->type.state |= RAW;
    sp->type.state |= RAW;

    return sp;
}

Span* Span_Make(MemCtx* m){
    Span *p = Span_Init(m);
    p->stride = stride;
    p->max_idx = -1;
    p->slotSize = 1;
    p->slab = Slab_Alloc(m, p->stride, p->flags & FLAG_RAW);
    p->slab->increment = stride;
    p->slab->level = 1;
    p->slab->stride = stride;

    return p;
}

void SlabResult_Setup(Span *p, SlabResult *sr, byte op, int idx){
    memset(sr, 0, sizeof(SlabResult));

    sr->op = op;
    sr->span = p;
    sr->dims = p->ndims;
    sr->slab = p->slab;
    sr->slotSize = p->slotSize;
    sr->idx = idx * p->slotSize;
    sr->local_idx = sr->idx;
    sr->level = p->ndims-1;
    sr->offset = 0;

    sr->dimsNeeded = 1;
    while(sr->idx >= slotsAvailable(sr->dimsNeeded)) {
        sr->dimsNeeded++;
    };

    return;
}

lifecycle_t Span_Expand(MemCtx *m, SlabResult *sr){
    /* resize the span by adding dimensions and slabs as needed */
    if(sr->dimsNeeded > sr->dims){
        span_GrowToNeeded(m, sr);
    }

    int dims = sr->dims;
    int increment = slotsAvailable(sr->stride, dims-1);
    while(dims > 1){
        sr->local_idx = (sr->idx - sr->offset) / increment;

        /* find or allocate a space for the new span */
        sr->shelfSlab = (Slab *)sr->slab->items[sr->local_idx];

        /* make new if not exists */
        if(sr->shelfSlab == NULL){
            Slab *new_sl = openNewSlab(m, 
                sr->local_idx, sr->offset, dims-1, increment, (sr->span->flags & RAW));
            sr->slab->items[sr->local_idx] = (Unit *)new_sl;
            sr->slab = new_sl;
        }else{
            if((sr->shelfSlab->type.state & TYPE_SLAB) != 0){
                printf("Error expected to find a mid dimenstion slab");
                sr->lifecycle = ERROR;
                return sr->lifecycle;
            }
            sr->slab = sr->shelfSlab;
        }

        /* prepare for another found of inquiry */
        sr->offset += increment*sr->local_idx;
        dims--;
        increment = slotsAvailable(sr->stride, dims-1);
    }

    /* conclude by setting the local idx and setting the state */
    sr->local_idx = (sr->idx - sr->offset);
    sr->type.state = SUCCESS;

    return sr->type.state;
}

static status Span_GetSet(MemCtx *m, SlabResult *sr, Span *p, int idx, Unit *t){
    Span_Expand(m, sr);
    Span *p = sr->p;
    if(HasFlag(sr->type.state, SUCCESS)){
        if(HasFlag(p->type.state, RAW)){
            size_t offset = sr->local_idx*p->itemSize;
            void *ptr += ((void *)sr->slab->items)+offset;
            if(sr->op == SPAN_OP_REMOVE){
                memset(ptr, 0, p->itemSize);
            }else if(sr->op == SPAN_OP_SET){
                memcpy(ptr, t, p->itemSize);
            }else if(sr->op == SPAN_OP_GET){
                sr->value = ptr;
            }
        }else{
            if(sr->op == SPAN_OP_GET){
                sr->value = t;
            }else{
                sr->slab->items[sr->local_idx] = t;
            }
        }
        p->nvalues++;
        if(idx > p->max_idx){
            p->max_idx = idx;
        }

        return SUCCESS;
    }

    return ERROR;
}

status Span_Set(MemCtx *m, Span *p, int idx, Unit *t){
    SlabResult sr;
    SlabResult_Setup(&sr, p, SPAN_OP_SET, idx);
    return Span_GetSet(MemCtx *m, &sr, int idx, t);
}

status Span_Remove(MemCtx *m, Span *p, int idx){
    SlabResult sr;
    SlabResult_Setup(&sr, p, SPAN_OP_REMOVE, idx);
    return Span_GetSet(MemCtx *m, &sr, int idx, NULL);
}

void *Span_Get(MemCtx *m, Span *p, int idx){
    SlabResult sr;
    SlabResult_Setup(&sr, p, SPAN_OP_GET, idx);
    status r = Span_Set(MemCtx *m, &sr, int idx, NULL);
    if(HasFlag(r, SUCCESS)){
        return sr->value;
    }else{
        return NULL;
    }
}

idx_t Span_Add(MemCtx *m, Span *p, Unit *t){
    idx_t idx = Span_NextIdx(p);
    if(Span_Set(m, p, idx, t) == SUCCESS){
        return p->max_idx;
    }

    return 0;
}
