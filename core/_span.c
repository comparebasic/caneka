#include <external.h>
#include <caneka.h>

static int availableByDim(int dims, int stride){
    int _dims = dims;
    int n = stride;
    int r = n;
    if(dims <= 0){
        r = 1;
    }else if(dims == 1){
        return stride;
    }else{
        while(dims > 1){
            n *= stride;
            dims--;
        }
        r = n;
    }

    return r;
}

/* API */

void SlabResult_Setup(SlabResult *sr, Span *p, byte op, int idx){
    memset(sr, 0, sizeof(SlabResult));

    sr->op = op;
    sr->m = p->m;
    sr->span = p;
    sr->dims = p->dims;
    sr->slab = p->root;
    sr->idx = idx;
    sr->dimsNeeded = SpanDef_GetDimNeeded(p->def, idx);

    return;
}

status Span_GetSet(SlabResult *sr, Abstract *t){
    MemCtx *m = sr->span->m;
    if(m == NULL && sr->op != SPAN_OP_GET){
        return ERROR;
    }
    return NOOP;
}

status Span_Set(Span *p, int idx, Abstract *t){
    SlabResult sr;
    SlabResult_Setup(&sr, p, SPAN_OP_SET, idx);
    return Span_GetSet(&sr, t);
}

void *Span_Get(Span *p, int idx){
    SlabResult sr;
    SlabResult_Setup(&sr, p, SPAN_OP_GET, idx);
    status r = Span_GetSet(&sr, NULL);
    if(HasFlag(r, SUCCESS)){
        return sr.value;
    }else{
        return NULL;
    }
}

status Span_Remove(Span *p, int idx){
    SlabResult sr;
    SlabResult_Setup(&sr, p, SPAN_OP_REMOVE, idx);
    return Span_GetSet(&sr, NULL);
}

status Span_Move(Span *p, int fromIdx, int toIdx){
    return NOOP;
}

/* internals */
static status span_GrowToNeeded(MemCtx *m, SlabResult *sr){
    boolean expand = sr->span->dims < sr->dimsNeeded;
    int dims = sr->span->dims;
    SpanDef *def = sr->span->def;

    if(expand){
        Slab *exp_sl = NULL;
        Slab *shelf_sl = NULL;
        while(dims < sr->dimsNeeded){
            Slab *new_sl = Span_idxSlab_Make(m, def);

            if(exp_sl == NULL){
                shelf_sl = sr->span->root;
                sr->span->root = new_sl;
            }else{
                exp_sl[0] = (Abstract *)new_sl;
            }

            exp_sl = new_sl;
            dims++;
        }
        exp_sl[0] = (Abstract *)shelf_sl;
    }

    sr->slab = sr->span->root;
    return SUCCESS;
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
    while(availableByDim(dims, def->idxStride) < nslabs){
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
    }
    return NULL;
}

void *Span_valueSlab_Make(MemCtx *m, SpanDef *def){
    i64 sz = sizeof(Abstract *)*def->stride*def->slotSize;
    return MemCtx_Alloc(m, sz);
}

void *Span_idxSlab_Make(MemCtx *m, SpanDef *def){
    i64 sz = sizeof(Abstract *)*(1+def->idxExtraSlots)*def->idxStride;
    return MemCtx_Alloc(m, sz);
}

void *Span_nextByIdx(SlabResult *sr){
    SpanDef *def = sr->span->def;
    return (void *)(sr->slab[sr->local_idx*(def->slotSize)]);
}

void *Span_nextBySlot(SlabResult *sr){
    SpanDef *def = sr->span->def;
    return (void *)(sr->slab[sr->local_idx*(1+def->idxExtraSlots)]);
}

void *Span_reserve(SlabResult *sr){
    return NULL;
}

Span *Span_Make(MemCtx *m, cls type){
    Span *p = MemCtx_Alloc(m, sizeof(Span));
    p->m = m;
    p->def = SpanDef_FromCls(type);
    p->root = p->def->valueSlab_Make(m, p->def);
    return p;
}
