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

void SlabResult_Setup(Span *p, SlabResult *sr, int idx){
    memset(sr, 0, sizeof(SlabResult));

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

lifecycle_t Span_Cull(MemCtx *m, Span *sp, int amount){
    lifecycle_t r = ERROR;
    while(amount--){
        int idx = sp->max_idx;
        r = Span_Remove(m, sp, idx);
        if(r == SUCCESS){
            sp->max_idx--;
        }else{
            break;
        }
    }
    return r;
}

lifecycle_t Span_Remove(MemCtx *m, Span *p, int idx){
    if(idx > p->max_idx){
        return MISS;
    }

    SlabResult sr;
    SlabResult_Setup(p, &sr, idx);
    Span_Expand(m, &sr);
    if(sr.lifecycle == SUCCESS){
        if(HasFlag(p, FLAG_RAW)){
            size_t size = p->slotSize*sizeof(Unit *);
            size_t offset = sr.local_idx*sizeof(Unit *);
            void *ptr = (void *)((Unit *)sr.slab->items);
            ptr += offset;

            long end = (long)ptr + (p->slotSize*sizeof(Unit *));
            long slab_end = (long)sr.slab->items+(sizeof(Unit *)*p->stride);
            long start = (long)ptr;
            long slab_start = (long)sr.slab->items;
            if(end > slab_end){
                printf("Error: Remove ptr is out of bounds for slot %ld_%ld vs %ld local_idx is %d stride is:%d size is:%d\n", start-slab_start, end-slab_start, slab_end-slab_start, sr.local_idx, sr.slab->stride, p->slotSize);
                return ERROR;
            }
            memset(ptr, 0, p->itemSize);
        }else{
            sr.slab->items[sr.local_idx] = NULL;
        }
        return SUCCESS;
    }

    return MISS;
}

lifecycle_t Span_Set(MemCtx *m, Span *p, int idx, Unit *t){
    SlabResult sr;
    SlabResult_Setup(p, &sr, idx);

    if(DEBUG_SPAN_SET){
        String *db = String_Format(m, "\x1b[%dmSet %d: ", DEBUG_SPAN_SET, idx);
        String_Print(db);
        printf("\x1b[0m\n");
    }

    Span_Expand(m, &sr);
    if(sr.lifecycle == SUCCESS){
        if(HasFlag(p, FLAG_RAW)){
            size_t size = p->slotSize*sizeof(Unit *);
            size_t offset = sr.local_idx*sizeof(Unit *);
            void *ptr = (void *)((Unit *)sr.slab->items);
            ptr += offset;

            long end = (long)ptr + (p->slotSize*sizeof(Unit *));
            long slab_end = (long)sr.slab->items+(sizeof(Unit *)*p->stride);
            long start = (long)ptr;
            long slab_start = (long)sr.slab->items;
            if(end > slab_end){
                printf("Error: ptr is out of bounds for slot %ld_%ld vs %ld local_idx is %d stride is:%d size is:%d\n", start-slab_start, end-slab_start, slab_end-slab_start, sr.local_idx, sr.slab->stride, p->slotSize);
                return ERROR;
            }
            memcpy(ptr, t, p->itemSize);
        }else{
            sr.slab->items[sr.local_idx] = t;
        }
        p->nvalues++;
        if(idx > p->max_idx){
            p->max_idx = idx;
        }

        return SUCCESS;
    }

    return ERROR;
}

idx_t Span_NextIdx(Span *p){
    return p->max_idx+1;
}

idx_t Span_Add(MemCtx *m, Span *p, Unit *t){
    idx_t idx = Span_NextIdx(p);
    if(Span_Set(m, p, idx, t) == SUCCESS){
        return p->max_idx;
    }

    return 0;
}

Unit *Span_Search(MemCtx *m, Span *p, Unit *t){
    if(p->rslv->cmp.type == 0){
        return NULL;
    }

    Comp *cmp = &(p->rslv->cmp);
    Unit *result = NULL;
    for(int i = 0; i <= p->max_idx; i++){
        Unit *item = Span_Get(m, p, i);
        if(item != NULL){
            Compare_With(m, item, t, cmp);
            if(cmp->lifecycle == SUCCESS){
                result = cmp->dest;
                break;
            }
        }
    };

    return result;
}

Unit *Span_Get(MemCtx *m, Span *p, int idx){

    if(idx < 0){
        idx = p->max_idx + idx+1;
    }

    if(idx < 0 || idx > p->max_idx){
        return NULL;
    }

    SlabResult sr;
    SlabResult_Setup(p, &sr, idx);

    if(DEBUG_SPAN_GET){
        printf("\x1b[%dmGet %d:", DEBUG_SPAN_GET, idx);
        printf("\x1b[0m\n");
    }

    Span_Expand(m, &sr);

    if(sr.lifecycle == SUCCESS){
        if(HasFlag(p, FLAG_RAW)){
            size_t size = p->slotSize*sizeof(Unit *);
            size_t offset = sr.local_idx*sizeof(Unit *);
            void *ptr = (void *)((Unit *)sr.slab->items);

            long end = (long)ptr + (p->slotSize*sizeof(Unit *));
            long slab_end = (long)sr.slab->items+(sizeof(Unit *)*p->stride);
            long start = (long)ptr;
            long slab_start = (long)sr.slab->items;

            if(end > slab_end){
                printf("Error: ptr to get is out of bounds for slot %ld_%ld vs %ld local_idx is %d stride is:%d size is:%d\n", start-slab_start, end-slab_start, slab_end-slab_start, sr.local_idx, sr.slab->stride, p->slotSize);
                return NULL;
            }

            return  ptr + offset;

        }

        return sr.slab->items[sr.local_idx];
    }

    return NULL;
}
