#include <external.h>
#include <filestore.h>

#define STRIDE SPAN_DIM_SIZE

static Slab *openNewSlab(MemCtx *m, int local_idx, int offset, int dims, int increment, status flags){
    Slab *new_sl = Slab_Alloc(m, flags);
    printf("INCREMENT %d\n", increment);
    new_sl->increment = increment; 
    new_sl->offset = offset;

    return new_sl;
}

static int slotsAvailable(int dims){
    int n = STRIDE;
    int _dims = dims;
    int r = n;
    if(dims <= 0){
        r = 1;
    }else if(dims == 1){
        r = STRIDE;
    }else{
        while(dims > 1){
            n *= STRIDE;
            dims--;
        }
        r = n;
    }

    return r;
}

static status span_GrowToNeeded(MemCtx *m, SlabResult *sr){
    int dims = sr->dims = sr->span->ndims = sr->dimsNeeded;

    int slots_available = slotsAvailable(dims);

    int needed = slots_available;
    boolean expand = sr->slab->increment < needed;
    if(expand){
        Slab *exp_sl = openNewSlab(m, 0, 0, dims, needed, (sr->span->type.state|RAW));

        exp_sl->type.state |= sr->span->type.state;

        Slab *shelf_sl = sr->span->slab;
        sr->span->slab = exp_sl;
        needed /= STRIDE;
        while(sr->slab->increment < needed){
            Slab *new_sl = openNewSlab(m, 0, 0, dims, needed, (sr->span->type.state|RAW));
            exp_sl->items[0] = (Unit *)new_sl;
            exp_sl = new_sl;
            needed /= STRIDE;
        }
        exp_sl->items[0] = (Unit *)shelf_sl;
    }
    return SUCCESS;
}

static status Span_Expand(MemCtx *m, SlabResult *sr){
    /* resize the span by adding dimensions and slabs as needed */
    if(sr->dimsNeeded > sr->dims){
        span_GrowToNeeded(m, sr);
    }

    int dims = sr->dims;
    int increment = slotsAvailable(dims-1);
    while(dims > 1){
        sr->local_idx = (sr->idx - sr->offset) / increment;

        /* find or allocate a space for the new span */
        sr->shelfSlab = (Slab *)sr->slab->items[sr->local_idx];

        /* make new if not exists */
        if(sr->shelfSlab == NULL){
            Slab *new_sl = openNewSlab(m, 
                sr->local_idx, sr->offset, dims-1, increment, (sr->span->type.state|RAW));
            sr->slab->items[sr->local_idx] = (Unit *)new_sl;
            sr->slab = new_sl;
        }else{
            if((sr->shelfSlab->type.state & TYPE_SLAB) != 0){
                printf("Error expected to find a mid dimenstion slab");
                sr->type.state = ERROR;
                return sr->type.state;
            }
            sr->slab = sr->shelfSlab;
        }

        /* prepare for another found of inquiry */
        sr->offset += increment*sr->local_idx;
        dims--;
        increment = slotsAvailable(dims-1);
    }

    /* conclude by setting the local idx and setting the state */
    sr->local_idx = (sr->idx - sr->offset);
    sr->type.state = SUCCESS;

    return sr->type.state;
}

static void SlabResult_Setup(SlabResult *sr, Span *p, byte op, int idx){
    memset(sr, 0, sizeof(SlabResult));

    sr->op = op;
    sr->span = p;
    sr->dims = p->ndims;
    sr->slab = p->slab;
    sr->slotSize = p->slotSize;
    sr->idx = idx * p->slotSize;
    sr->local_idx = sr->idx;
    sr->offset = 0;

    sr->dimsNeeded = 1;
    while(sr->idx >= slotsAvailable(sr->dimsNeeded)) {
        sr->dimsNeeded++;
    };

    return;
}

static status Span_GetSet(MemCtx *m, SlabResult *sr, int idx, Unit *t){
    Span_Expand(m, sr);
    Span *p = sr->span;
    if(HasFlag(sr->type.state, SUCCESS)){
        if(HasFlag(p->type.state, RAW)){
            size_t offset = sr->local_idx*p->slotSize;
            void *ptr = ((void *)sr->slab->items)+offset;
            if(sr->op == SPAN_OP_REMOVE){
                memset(ptr, 0, p->itemSize);
            }else if(sr->op == SPAN_OP_SET){
                memcpy(ptr, t, p->itemSize);
            }else if(sr->op == SPAN_OP_GET){
                sr->value = ptr;
            }
        }else{
            if(sr->op == SPAN_OP_GET){
                sr->value = sr->slab->items[sr->local_idx];
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

static Span* Span_Init(MemCtx* m){
    return (Span *)MemCtx_Alloc(m, sizeof(Span));
}

Span* Span_Make(MemCtx* m){
    Span *p = Span_Init(m);
    p->max_idx = -1;
    p->slotSize = 1;
    p->slab = Slab_Alloc(m, (p->type.state|RAW));
    p->slab->increment = STRIDE;
    p->type.of = TYPE_SPAN;

    return p;
}

Span* Span_MakeInline(MemCtx* m, cls type, int itemSize){
    int slotSize = 1;
    if(itemSize > sizeof(void *)){
        slotSize = itemSize / sizeof(void *);
        if(itemSize % sizeof(void *) > 1){
            slotSize += 1;
        }
    }
    Span *sp = Span_Make(m);
    sp->itemSize = itemSize;
    sp->itemType = type;
    sp->slotSize = slotSize;
    sp->slab->type.state |= RAW;
    sp->type.state |= RAW;

    return sp;
}

status Span_Set(MemCtx *m, Span *p, int idx, Unit *t){
    SlabResult sr;
    SlabResult_Setup(&sr, p, SPAN_OP_SET, idx);
    return Span_GetSet(m, &sr, idx, t);
}

status Span_Remove(MemCtx *m, Span *p, int idx){
    SlabResult sr;
    SlabResult_Setup(&sr, p, SPAN_OP_REMOVE, idx);
    return Span_GetSet(m, &sr, idx, NULL);
}

void *Span_Get(MemCtx *m, Span *p, int idx){
    SlabResult sr;
    SlabResult_Setup(&sr, p, SPAN_OP_GET, idx);
    status r = Span_GetSet(m, &sr, idx, NULL);
    if(HasFlag(r, SUCCESS)){
        return sr.value;
    }else{
        return NULL;
    }
}

int Span_Add(MemCtx *m, Span *p, Unit *t){
    int idx = Span_NextIdx(p);
    if(Span_Set(m, p, idx, t) == SUCCESS){
        return p->max_idx;
    }

    return 0;
}
