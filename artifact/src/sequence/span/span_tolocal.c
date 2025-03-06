#include <external.h>
#include <caneka.h>

static status slab_ToLocal(MemCtx *m, byte dim, byte slotSize, byte ptrSlot, void *sl){
    status r = READY;
    int stride = SPAN_STRIDE;
    for(int i = 0; i < stride; i += slotSize){
        void *subSl = (void *)Slab_nextSlot(sl, slotSize, ptrSlot, i);
        if(subSl != NULL){
            slab_ToLocal(m, dim-1, slotSize, ptrSlot, subSl);
            r |= MemLocal_SetLocal(m, (Abstract **)&subSl);
        }
    }
    return r;
}

static status slab_FromLocal(MemCtx *m, byte dim, byte slotSize, byte ptrSlot, void *sl){
    status r = READY;
    int stride = SPAN_STRIDE;
    for(int i = 0; i < stride; i += slotSize){
        void *subSl = (void *)Slab_nextSlot(sl, slotSize, ptrSlot, i);
        if(subSl != NULL){
            r |= MemLocal_UnSetLocal(m, (Abstract **)&subSl);
            slab_FromLocal(m, dim-1, slotSize, ptrSlot, subSl);
        }
    }
    return r;
}


status Span_ToLocal(MemCtx *m, Abstract *a){
    DebugStack_Push("Span_ToLocal", TYPE_CSTR);
    status r = READY;
    Span *p = (Span *)asIfc(a, TYPE_SPAN);
    if((p->type.state & SPAN_RAW) != 0){
        DebugStack_Pop();
        return ERROR;
    }
    r |= slab_ToLocal(m, p->dims, p->slotSize, p->ptrSlot, p->root);
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
    if((p->type.state & SPAN_RAW) != 0){
        DebugStack_Pop();
        return ERROR;
    }
    r |= slab_FromLocal(m, p->dims, p->slotSize, p->ptrSlot, p->root);
    p->m = m;
    a->type.of -= HTYPE_LOCAL;

    if(r == READY){
        r |= NOOP;
    }

    DebugStack_Pop();
    return NOOP;
}
