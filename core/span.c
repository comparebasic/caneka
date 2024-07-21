#include <external.h>
#include <caneka.h>

#define STRIDE(p) (((p)->type.of == TYPE_MINISPAN) ? SPAN_MINI_DIM_SIZE : SPAN_DIM_SIZE)

static Slab *openNewSlab(MemCtx *m, int local_idx, int offset, int increment, status flags){
    Slab *new_sl = Slab_Alloc(m, flags);
    new_sl->increment = increment; 
    new_sl->offset = offset;

    return new_sl;
}

static int slotsAvailable(Span *p, int dims){
    int n = STRIDE(p);
    int _dims = dims;
    int r = n;
    if(dims <= 0){
        r = 1;
    }else if(dims == 1){
        r = STRIDE(p);
    }else{
        while(dims > 1){
            n *= STRIDE(p);
            dims--;
        }
        r = n;
    }

    return r;
}

static status span_GrowToNeeded(MemCtx *m, SlabResult *sr){
    int dims = sr->dims = sr->span->dims = sr->dimsNeeded;

    int needed = slotsAvailable(sr->span, dims);
    boolean expand = sr->slab->increment < needed;
    if(expand){
        Slab *exp_sl = NULL;
        Slab *shelf_sl = NULL;
        while(sr->slab->increment < needed){
            Slab *new_sl = openNewSlab(m, 0, 0, needed, (sr->span->type.state|RAW));

            if(exp_sl == NULL){
                shelf_sl = sr->span->slab;
                sr->span->slab = new_sl;
            }else{
                exp_sl->items[0] = (Abstract *)new_sl;
            }

            needed /= STRIDE(sr->span);
            exp_sl = new_sl;
        }
        exp_sl->items[0] = (Abstract *)shelf_sl;
    }

    sr->slab = sr->span->slab;
    return SUCCESS;
}

static status Span_Expand(MemCtx *m, SlabResult *sr){
    /* resize the span by adding dimensions and slabs as needed */
    if(sr->dimsNeeded > sr->dims){
        if(sr->op != SPAN_OP_SET){
            return MISS;
        }
        span_GrowToNeeded(m, sr);
    }

    byte dims = sr->dims = sr->span->dims = max(sr->dims, sr->span->dims);
    int increment = slotsAvailable(sr->span, dims-1);
    Slab *prev_sl = sr->slab;
    while(dims > 1){
        sr->local_idx = (sr->idx - sr->offset) / increment;
        sr->offset += increment*sr->local_idx;

        /* find or allocate a space for the new span */
        sr->slab = (Slab *)sr->slab->items[sr->local_idx];

        /* make new if not exists */
        if(sr->slab == NULL){
            if(sr->op != SPAN_OP_SET){
                return MISS;
            }
            Slab *new_sl = openNewSlab(m, 
                sr->local_idx, sr->offset, increment, (sr->span->type.state|RAW));
            prev_sl->items[sr->local_idx] = (Abstract *)new_sl;
            prev_sl = sr->slab = new_sl;
        }else{
            prev_sl = sr->slab;
        }

        if((sr->slab->type.state & TYPE_SLAB) != 0){
            printf("Error expected to find a mid dimenstion slab");
            sr->type.state = ERROR;
            return sr->type.state;
        }

        /* prepare for another found of inquiry */
        dims--;
        increment = slotsAvailable(sr->span, dims-1);
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
    sr->dims = p->dims;
    sr->slab = p->slab;
    sr->slotSize = p->slotSize;
    sr->idx = idx * p->slotSize;
    sr->local_idx = sr->idx;
    sr->offset = 0;

    sr->dimsNeeded = 1;
    while(sr->idx >= slotsAvailable(sr->span, sr->dimsNeeded)) {
        sr->dimsNeeded++;
    };

    return;
}

static status Span_GetSet(SlabResult *sr, int idx, Abstract *t){
    MemCtx *m = sr->span->m;
    if(m == NULL && sr->op != SPAN_OP_GET){
        return ERROR;
    }
    status r = Span_Expand(m, sr);
    if(r != SUCCESS){
        return r; 
    }
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
        if(sr->op != SPAN_OP_GET && idx > p->max_idx){
            p->nvalues++;
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
    p->m = m;
    p->max_idx = -1;
    p->slotSize = 1;
    p->dims = 1;
    p->slab = Slab_Alloc(m, (p->type.state|RAW));
    p->type.of = TYPE_SPAN;
    p->slab->increment = STRIDE(p);

    return p;
}

Span* Span_MakeMini(MemCtx* m){
    Span *p = Span_Init(m);
    p->m = m;
    p->max_idx = -1;
    p->slotSize = 1;
    p->slab = Slab_Alloc(m, (p->type.state|RAW));
    p->type.of = TYPE_MINISPAN;
    p->slab->increment = STRIDE(p);

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

status Span_Set(Span *p, int idx, Abstract *t){
    SlabResult sr;
    SlabResult_Setup(&sr, p, SPAN_OP_SET, idx);
    return Span_GetSet(&sr, idx, t);
}

status Span_Remove(Span *p, int idx){
    SlabResult sr;
    SlabResult_Setup(&sr, p, SPAN_OP_REMOVE, idx);
    return Span_GetSet(&sr, idx, NULL);
}

void *Span_Get(Span *p, int idx){
    SlabResult sr;
    SlabResult_Setup(&sr, p, SPAN_OP_GET, idx);
    status r = Span_GetSet(&sr, idx, NULL);
    if(HasFlag(r, SUCCESS)){
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

void Span_Run(MemCtx *m, Span *p, Maker func, Abstract *arg){
    if(p == NULL){
        return;
    }
    for(int i = 0; i <= p->max_idx; i++){
        Abstract *t = Span_Get(p, i);
        if(t != NULL){
            func(m, t);
        }
    };
}

void Span_RunMaker(MemCtx *m, Span *p, Maker func, Abstract *arg){
    if(p == NULL){
        return;
    }
    for(int i = 0; i < p->max_idx; i++){
        Abstract *t = Span_Get(p, i);
        if(t != NULL){
            func(m, t);
        }
    };
}

Span *Span_From(MemCtx *m, int count, ...){
    Span *p = Span_Make(m);
    Abstract *v = NULL;
	va_list arg;
    va_start(arg, count);
    for(int i = 0; i < count; i++){
        v = va_arg(arg, AbstractPtr);
        Span_Add(p, v);
    }
    return p;
}

int Span_GetIdx(Span *p, void *a, EqualFunc eq){
    for(int i = 0; i < p->max_idx; i++){
        Abstract *t = Span_Get(p, i);
        if(eq(t, a)){
            return i;
        }
    };

    return -1;
}

status Span_Merge(Span *dest, Span *additional){
    for(int i = 0; i < additional->max_idx; i++){
        Abstract *t = Span_Get(additional, i);
        if(t != NULL){
            Span_Add(dest, t);
        }
    };

    return SUCCESS;
}
