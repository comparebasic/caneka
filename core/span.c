#include <external.h>
#include <caneka.h>

#define STRIDE SPAN_DIM_SIZE

static int slotsAvailable(Span *p, int dims){
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
    boolean expand = sr->span->dims < sr->dimsNeeded;
    int dims = sr->span->dims;

    int needed = slotsAvailable(sr->span, dims);
    if(expand){
        Slab *exp_sl = NULL;
        Slab *shelf_sl = NULL;
        int stride = STRIDE;
        while(dims < sr->dimsNeeded){
            Slab *new_sl = Slab_Alloc(m);

            if(exp_sl == NULL){
                shelf_sl = sr->span->slab;
                sr->span->slab = new_sl;
            }else{
                exp_sl[0] = (Abstract *)new_sl;
            }

            needed /= stride;
            exp_sl = new_sl;
            dims++;
        }
        exp_sl[0] = (Abstract *)shelf_sl;
    }

    sr->slab = sr->span->slab;
    return SUCCESS;
}

static status Span_Expand(MemCtx *m, SlabResult *sr){
    /* resize the span by adding dimensions and slabs as needed */
    if(sr->dimsNeeded > sr->dims){
        if(sr->op != SPAN_OP_SET && sr->op != SPAN_OP_RESERVE){
            return MISS;
        }
        span_GrowToNeeded(m, sr);
    }

    byte idxSlotSize = sr->span->idxSlotSize;

    byte dims = sr->dims = sr->span->dims = max(sr->dims, sr->span->dims);
    Slab *prev_sl = sr->slab;
    while(dims > 1){
        int increment = slotsAvailable(sr->span, dims-1);
        sr->local_idx = ((sr->idx - sr->offset) / increment);
        if(dims > 1){
            increment /= sr->span->idxSlotSize;
            sr->local_idx *= sr->span->idxSlotSize;
        }
        sr->offset += increment*sr->local_idx;

        /* find or allocate a space for the new span */
        sr->slab = (Slab *)sr->slab[sr->local_idx];

        /* make new if not exists */
        if(sr->slab == NULL){
            if(sr->op != SPAN_OP_SET && sr->op != SPAN_OP_RESERVE){
                return MISS;
            }
            Slab *new_sl = openNewSlab(m, 
                sr->local_idx, sr->offset, increment, (sr->span->type.state & RAW),
                sr->span->slotSize);
            prev_sl[sr->local_idx] = (Abstract *)new_sl;
            prev_sl = sr->slab = new_sl;
        }else{
            prev_sl = sr->slab;
        }

        dims--;
    }

    /* conclude by setting the local idx and setting the state */
    sr->local_idx = (sr->idx - sr->offset);

    sr->type.state = SUCCESS;
    return sr->type.state;
}

static void SlabResult_Setup(SlabResult *sr, Span *p, byte op, int idx){
    memset(sr, 0, sizeof(SlabResult));

    sr->op = op;
    sr->m = p->m;
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

static Slab_UpdateFlags(SlabResult *sr){
    if(HasFlag(sr->span->type.state, TRACKED)){
        SlabSlot *slt = (SlabSlot *)sr->slab;
        sl->meta.pos = sr->local_idx+1;
        sl->meta.flags |= SLAB_ACTIVE;
        if(sl->meta.pos >= STRIDE){
            sl->meta.flags |= SLAB_FULL;
            sl->meta.pos = -1;
        }
    }
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
            void *ptr = Slab_GetPtr(sr->slab, sr->local_idx);
            if(sr->op == SPAN_OP_REMOVE || sr->op == SPAN_OP_RESERVE){
                memset(ptr, 0, p->itemSize);
                sr->value = ptr;
                sr->span->metrics.set = idx;
            }else if(sr->op == SPAN_OP_SET){
                sr->span->metrics.set = idx;
                memcpy(ptr, t, p->itemSize);
                Slab_UpdateFlags(sr);
            }else if(sr->op == SPAN_OP_GET){
                sr->value = ptr;
                sr->span->metrics.get = idx;
            }
        }else{
            if(sr->op == SPAN_OP_GET){
                sr->value = sr->slab[sr->local_idx];
                sr->span->metrics.get = idx;
            }else{
                sr->slab[sr->local_idx] = t;
                sr->span->metrics.set = idx;
                Slab_UpdateFlags(sr);
            }
        }
        if(sr->op != SPAN_OP_GET){
            p->nvalues++;
            if(idx > p->max_idx){
                p->max_idx = idx;
            }
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
    p->slab = Slab_Alloc(m, (p->type.state & RAW), p->slotSize);
    p->type.of = TYPE_SPAN;

    return p;
}

Span* Span_MakeInline(MemCtx* m, cls type, int itemSize){
    Span *sp = Span_Make(m);
    sp->idxSlotSize = 1;

    int slotSize = 1;
    if(itemSize > sizeof(void *)){
        slotSize = itemSize / sizeof(void *);
        if(itemSize % sizeof(void *) > 1){
            slotSize += 1;
        }
    }
    if(Ifc_Match(cls, TYPE_QUEUE)){
        slotSize += 1;
        sp->type.state |= TRACKED;
        sp->idxSlotSize += 1;
    }

    byte pwrSlot = SPAN_DIM_SIZE;
    while((pwrSlot / 2) >= slotSize){
        pwrSlot /= 2;
    }

    sp->itemSize = itemSize;
    sp->itemType = type;
    sp->slotSize = pwrSlot;
    sp->type.state |= RAW;
    if(type == 0){
        sp->type.of = TYPE_SPAN;
    }else{
        sp->type.of = cls;
    }

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

void *Span_GetSelected(Span *p){
    return Span_Get(p, p->metrics.selected);
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
    status r = Span_GetSet(&sr, idx, NULL);
    if(HasFlag(r, SUCCESS)){
        return sr.value;
    }else{
        return NULL;
    }
}

void Span_Run(MemHandle *m, Span *p, Maker func, Abstract *arg){
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

void Span_RunMaker(MemHandle *m, Span *p, Maker func, Abstract *arg){
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

status Span_Merge(Span *dest, Span *additional){
    for(int i = 0; i <= additional->max_idx; i++){
        Abstract *t = Span_Get(additional, i);
        if(t != NULL){
            Span_Add(dest, t);
        }
    };
    return SUCCESS;
}

status Span_ReInit(Span *p){
    p->nvalues = 0;
    p->max_idx = -1;
    p->metrics.selected = 0;
    return SUCCESS;
}
