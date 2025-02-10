#include <external.h>
#include <caneka.h>

static status slab_ToLocal(MemCtx *m, byte dim, SpanDef *def, void *sl){
    status r = READY;
    if(dim > 0){
        int stride = def->idxStride;
        for(int i = 0; i < stride; i += def->idxSize){
            void *subSl = (void *)Slab_nextSlot(sl, def, i);
            if(subSl != NULL){
                slab_ToLocal(m, dim-1, def, subSl);
                r |= MemLocal_SetLocal(m, (Abstract **)&subSl);
            }
        }
    }else if((def->flags & SPAN_INLINE) != 0){
        int stride = def->stride;
        for(int i = 0; i < stride; i++){
            void *item = Slab_valueAddr(sl, def, i);
            if(*((util *)(item)) != 0){
                r |= MemLocal_To(m, (Abstract *)item);
            }
        }
    }else{
        int stride = def->stride;
        for(int i = 0; i < stride; i++){
            Abstract **item = (Abstract **)Slab_valueAddr(sl, def, i);
            if(item != NULL){
                r |= MemLocal_To(m, (Abstract *)*item);
            }
        }
    }
    return r;
}

static status slab_FromLocal(MemCtx *m, byte dim, SpanDef *def, void *sl){
    status r = READY;
    if(dim > 0){
        int stride = def->idxStride;
        for(int i = 0; i < stride; i += def->idxSize){
            void *subSl = (void *)Slab_nextSlot(sl, def, i);
            if(subSl != NULL){
                r |= MemLocal_UnSetLocal(m, (Abstract **)&subSl);
                slab_FromLocal(m, dim-1, def, subSl);
            }
        }
    }else if((def->flags & SPAN_INLINE) != 0){
        int stride = def->stride;
        for(int i = 0; i < stride; i++){
            void *item = Slab_valueAddr(sl, def, i);
            if(*((util *)(item)) != 0){
                r |= MemLocal_From(m, (Abstract *)item);
            }
        }
    }else{
        int stride = def->stride;
        for(int i = 0; i < stride; i++){
            Abstract **item = (Abstract **)Slab_valueAddr(sl, def, i);
            if(item != NULL){
                r |= MemLocal_From(m, (Abstract *)*item);
            }
        }
    }
    return r;
}


status Span_ToLocal(MemCtx *m, Abstract *a){
    DebugStack_Push("Span_ToLocal", TYPE_CSTR);
    status r = READY;
    Span *p = (Span *)asIfc(a, TYPE_SPAN);
    if((p->def->flags & SPAN_RAW) != 0){
        DebugStack_Pop();
        return ERROR;
    }
    r |= slab_ToLocal(m, p->dims, p->def, p->root);
    a->type.of += HTYPE_LOCAL;

    if(r == READY){
        r |= NOOP;
    }

    DebugStack_Pop();
    return r;
}

status Span_FromLocal(MemCtx *m, Abstract *a){
    DebugStack_Push("Span_FromLocal", TYPE_CSTR);
    status r = READY;
    Span *p = (Span *)asIfcOffset(a, TYPE_SPAN, HTYPE_LOCAL);
    if((p->def->flags & SPAN_RAW) != 0){
        DebugStack_Pop();
        return ERROR;
    }
    r |= slab_FromLocal(m, p->dims, p->def, p->root);
    p->m = m;
    a->type.of -= HTYPE_LOCAL;

    if(r == READY){
        r |= NOOP;
    }

    DebugStack_Pop();
    return NOOP;
}
