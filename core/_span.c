#include <external.h>
#include <caneka.h>

void SlabResult_Setup(SlabResult *sr, Span *p, byte op, int idx){
    memset(sr, 0, sizeof(SlabResult));

    sr->op = op;
    sr->m = p->m;
    sr->span = p;
    sr->dims = p->dims;
    sr->slab = p->root;
    sr->idx = idx;
    sr->local_idx = sr->idx;
    sr->offset = 0;

    sr->dimsNeeded = 1;
    /*
    while(sr->idx >= slotsAvailable(sr->span, sr->dimsNeeded)) {
        sr->dimsNeeded++;
    };
    */

    return;
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

void *Span_valueSlab_Make(SlabResult *sr){
    SpanDef *def = sr->span->def;
    i64 sz = sizeof(Abstract *)*def->stride*def->slotSize;
    return MemCtx_Alloc(sr->m, sz);
}

void *Span_idxSlab_Make(SlabResult *sr){
    SpanDef *def = sr->span->def;
    return MemCtx_Alloc(sr->m, sizeof(Abstract *)*(1+def->idxExtraSlots)*def->idxStride);
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
    return p;
}
