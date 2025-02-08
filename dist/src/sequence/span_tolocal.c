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
            }

            LocalPtr lptr;
            r |= MemLocal_GetLocal(m, subSl, &lptr);
            memcpy(subSl, &lptr, sizeof(void *));
        }
    }else if((def->flags & SPAN_INLINE) == 0){
        int stride = def->stride;
        for(int i = 0; i < stride; i++){
            void *item = Slab_valueAddr(sl, def, i);
            MemLocal_To(m, item);

            LocalPtr lptr;
            r |= MemLocal_GetLocal(m, item, &lptr);
            memcpy(item, &lptr, sizeof(void *));
        }
    }
    return r;
}

status Span_ToLocal(MemCtx *m, Abstract *a){
    status r = READY;
    Span *p = (Span *)asIfc(a, TYPE_SPAN);
    Iter it;
    Iter_Init(&it, p);
    while((Iter_Next(&it) & END) == 0){
        Abstract *item = Iter_Get(&it);
        if(item != NULL){
            r |= MemLocal_To(m, item);
        }
    }

    r |= slab_ToLocal(m, p->dims, p->def, p->root);
    a->type.of += HTYPE_LOCAL;

    if(r == READY){
        r |= NOOP;
    }

    return r;
}

status Span_FromLocal(MemCtx *m, Abstract *a){
    return NOOP;
}
