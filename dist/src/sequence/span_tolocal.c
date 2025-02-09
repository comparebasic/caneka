#include <external.h>
#include <caneka.h>

static status slab_ToLocal(MemCtx *m, byte dim, SpanDef *def, void *sl){
    status r = READY;
    LocalSetter ls;
    LocalSetter *ptr = &ls;
    memset(&ls, 0, sizeof(LocalSetter));
    ls.type.of = TYPE_MEMLOCAL_SETTER;
    if(dim > 0){
        int stride = def->idxStride;
        for(int i = 0; i < stride; i += def->idxSize){
            void *subSl = (void *)Slab_nextSlot(sl, def, i);
            if(subSl != NULL){
                slab_ToLocal(m, dim-1, def, subSl);
                ls.dptr = &subSl;
                r |= MemLocal_SetLocal(m, (Abstract **)&ptr);
            }
        }
    }else if((def->flags & SPAN_INLINE) == 0){
        int stride = def->stride;
        for(int i = 0; i < stride; i++){
            void *item = Slab_valueAddr(sl, def, i);
            if(*((util *)(item)) != 0){
                r |= MemLocal_SetLocal(m, (Abstract **)&item);
            }
        }
    }else{
        int stride = def->stride;
        for(int i = 0; i < stride; i++){
            void *item = Slab_valueAddr(sl, def, i);
            if(item != NULL){
                r |= MemLocal_To(m, (Abstract *)item);
            }
        }
    }
    return r;
}

status Span_ToLocal(MemCtx *m, Abstract *a){
    status r = READY;
    Span *p = (Span *)asIfc(a, TYPE_SPAN);
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
