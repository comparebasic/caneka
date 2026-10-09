#include <external.h>
#include "base_module.h"

static inline Slate *Iter_getMakeSlate(Slate *parent, i8 local, boolean fill){
    Slate *sl = parent->slots[local];
    if(sl == NULL){
        if(!fill){
            return NULL;
        }else{
            sl = Slate_Make(m);
            parent->slots[it->localIdx[idim+1]] = sl;
        }
    }
    return sl;
}

static inline status Iter_resize(Iter *it){
    Span *p = it->p;
    MemCh *m = p->m;
    status r = ZERO;
    if(it->range.range == 0 || it->range.range < p->range.range){
        it->stack = (Slate **)MemCh_Alloc(m, SizeOf(Slate *)*(p->range.range*2));
        it->localIdx = (i8 *)MemCh_Alloc(m, SizeOf(i8)*(p->range.range*2));
        it->range.range = p->range.range;
        r = PROCESS;
    }
    return r;
}

static inline status Iter_Add(Iter *it){
    status r = ZERO;
    Span *p = it->p;
    i64 local = 0;
    it->idx = 0;

    Slate *sl = p->root;
    i8 idim = (i8)p->range.range;
    while(idim >= 0){
        if(idim == p->range.range){
            sl = p->root;
            it->stack[idim] = p->root;
            local = sl->idx.max;
            it->localIdx[idim] = local;
        }else{
            Slate *parent = it->stack[idim+1];
            sl = parent->slots[it->localIdx[idim+1]];
            if(sl->type.state & END){
                it->localIdx[idim+1]++;
                sl = Iter_getMakeSlate(parent, it->localIdx[idim+1], TRUE);
            }

            local = sl->idx.max;
            it->stack[idim] = sl;
            it->localIdx[idim] = local;
        }

        it->idx |= (local << (SPAN_DIM_SHIFT*(p->range.range-idim)));

        if(idim == 0){
            local = Slate_Add(sl, it->value);
            it->idx |= local;
            r |= PROCESS;
        }

        idim--;
    }

    return r;
}

static inline status Iter_getSetRemove(Iter *it){
    status r = ZERO;
    Span *p = it->p;

    i8 idim = (i8)p->range.range;
    while(idim >= 0){
        i64 local = 0;
        it->idx = 0;

        i64 local = it->idx;
        local = (local >> (SPAN_DIM_SHIFT*idim)) & (SPAN_LOCAL_MAX);

        Slate *sl = NULL;
        if(idim == p->range.range){
            sl = p->root;
            it->stack[idim] = p->root;
            it->localIdx[idim] = local;
        }else{
            Slate *parent = it->stack[idim+1];
            sl = Iter_getMakeSlate(parent, it->localIdx[idim+1], fill);
            if(sl == NULL){
                it->type.state |= NOOP;
                break;
            }
            it->stack[idim] = sl;
            it->localIdx[idim] = local;
        }

        if(idim == 0){
            if(!fill){
                if(sl->slots[local] == NULL){
                    it->type.state |= NOOP;
                }else{
                    it->value = Slate_Get(sl, local);
                }
                break;
            }else{
                if(it->type.state & (ITER_SET|ITER_ADD)){
                    i8 slIdx = Slate_Add(sl, it->value);
                    it->idx = (it->idx & SPAN_LOCAL_MASK) & (i64)slIdx;
                }else if(it->type.state & (ITER_SET)){
                    Slate_Insert(sl, local, it->value);
                }else if(it->type.state & ITER_REMOVE){
                    Slate_Remove(sl, local);
                }
                r |= PROCESS;
            }
        }
        idim--;
    }

    return r;
}

static status Iter_Query(Iter *it){
    it->type.state &= ~(NOOP|MORE|TAIL|PROCESS);
    MemCh *m = it->p->m;
    Span *p = it->p;

    if(it->type.state & ITER_ADD){
        it->idx = it->p->maxIdx+1;
        it->type.state &= ~END;
    }

    boolean fill = ((it->type.state & (ITER_SET|ITER_RESERVE|ITER_ADD)) != 0);
    if(!fill){
        it->value = NULL;
    }

    if((it->type.state & ITER_ADD) && (p->root->type.state & END)){
        it->idx = p->size;
    }

    if(it->idx >= p->size){
        if(!fill){
            it->type.state |= NOOP;
            return it->type.state;
        }

        Span_Resize(p, it->idx); 
    }

    Iter_resize(it);

    if(it->type.state & ITER_ADD){
        it->type.state |= Iter_Add(it);
    }else if(it->type.state & (ITER_SET|ITER_GET|ITER_REMOVE)){
        it->type.state |= Iter_getSetRemove(it);
    }

    if(it->idx == p->maxIdx){
        it->type.state |= TAIL;
    }else{
        it->type.state &= ~TAIL;
    }

    return it->type.state;
}

status Iter_Incr(Iter *it){
    i8 dim = 0;
    i8 topDim = it->p->range.range;
    i64 idx = it->idx;
    Slate *sl;
    Slate *parent;

    it->value = NULL;

    i8 incr = 1;
    if(it->type.state & ITER_REVERSE){
        incr = -1;
    }
    i64 factor = incr;

    if(it->p == NULL || it->p->root->idx.count == 0){
        it->type.state |= END; 
        return it->type.state;
    }
    if((it->type.state & PROCESS) == 0){
        if(it->stack[0] == NULL){
            word fl = it->type.state;
            it->type.state = (it->type.state & (NORMAL_FLAGS|ITER_REVERSE)) | ITER_GET;
            Iter_Query(it);
            it->type.state = fl;
        }
    }else{
        if(topDim == 0){
            if(((incr > 0) && (it->localIdx[dim] + incr) < SPAN_STRIDE || (it->localIdx[dim] + incr) >= 0)){
                it->localIdx[dim] += incr;
                it->stack[0] += incr;
            }
            idx += factor;
            it->value = *((void **)it->stack[dim]);
        }else{
            i16 guard = 0;
            while(it->value == NULL && dim <= topDim && ((incr > 0) && idx <= it->p->maxIdx || idx >= 0)){
                Guard_Incr(it->p->m, &guard, ITER_MAX, FUNCNAME, FILENAME, LINENUMBER);
                if((incr > 0) && (it->localIdx[dim] + incr) < SPAN_STRIDE || (it->localIdx[dim] + incr) < 0){
                    it->localIdx[dim] -= incr;
                    parent = it->stack[dim+1];
                    it->stack[dim] = (Slate *)parent->slots[it->localIdx[dim]];

                    idx += factor;
                    if(dim == 0){
                        it->value = (void *)it->stack[0];
                        if(it->value != NULL || (it->type.state & ITER_SKIP_NULL) == 0){
                            break;
                        }
                    }else if(it->stack[dim] != NULL){
                        Slate *current = it->stack[dim];
                        idx += factor-1;
                        while(dim-1 >= 0){
                            dim--;
                            factor *= SPAN_STRIDE;
                            it->localIdx[dim] = incr > 0 ? 0 : (SPAN_STRIDE-1);
                            parent = it->stack[dim+incr];
                            it->stack[dim] = (Slate *)parent->slots[it->localIdx[dim]];
                            if(it->stack[dim] == NULL){
                                dim++;
                                factor /= SPAN_STRIDE;
                                break;
                            }else if(dim == 0){
                                if(it->stack[dim] == NULL && (it->type.state & ITER_SKIP_NULL)){
                                    continue;
                                }else{
                                    it->value = (void *)it->stack[dim];
                                    break;
                                }
                            }
                        }
                    }
                }else{
                    if(incr > 0){
                        idx -= it->localIdx[dim] * factor;
                        it->localIdx[dim] = 0;
                    }else{
                        idx += it->localIdx[dim] * factor;
                        it->localIdx[dim] = 0;
                    }
                    dim++;
                }
            }
        }
    }

    it->idx = idx;
    if(idx == 0){
        it->type.state |= TAIL;
    }else{
        it->type.state &= ~TAIL;
    }

    if(it->value != NULL){
        it->type.state &= ~NOOP;
    }else{
        it->type.state |= NOOP;
    }

    return it->type.state;
}

status Iter_Next(Iter *it){
    it->type.state &= ITER_REVERSE;
    return Iter_Incr(it);
}

i64 Iter_NextIdx(Iter *it){
    if(it->type.state & SPAN_QUEUED){
        /* queue algorithm for finding next here */
        return -1;
    }else{
        return it->p->maxIdx+1;
    }
}


status Iter_Prev(Iter *it){
    it->type.state |= ITER_REVERSE;
    return Iter_Incr(it);
}

status Iter_Remove(Iter *it, i64 idx){
    it->type.state = (it->type.state & NORMAL_FLAGS) | ITER_REMOVE;
    it->value = (void *)NULL;
    return Iter_Query(it);
}

void *Iter_Pop(Iter *it){
    Iter_GoToIdx(it, it->p->maxIdx);
    void *value = it->value;
    Iter_Remove(it, it->idx);
    Iter_Prev(it);
    return value;
}

void Iter_Set(Iter *it, i64 idx, void *value){
    it->type.state = (it->type.state & NORMAL_FLAGS) | ITER_SET;
    it->idx = idx;
    it->value = value;
    Iter_Query(it);
}

void Iter_Add(Iter *it, void *value){
    it->type.state = (it->type.state & NORMAL_FLAGS) | ITER_ADD;
    it->value = value;
    Iter_Query(it);
}

void Iter_Push(Iter *it, void *value){
    i64 idx = it->idx;
    if(it->type.state & END){
        idx--;
    }
    Iter_Add(it, value);
    Iter_GoToIdx(it, idx);
}

void Iter_GoToIdx(Iter *it, i64 idx){
    it->type.state = (it->type.state & NORMAL_FLAGS) | ITER_GET;
    it->idx = idx;
    Iter_Query(it);
}

void Iter_Reset(Iter *it){
    it->type.state &= FOCUS;
    it->idx = 0;
}

void Iter_Init(Iter *it, Span *p){
    memset(it, 0, sizeof(Iter));
    it->type.of = TYPE_ITER;
    it->p = p;
    it->range.range = -1;
}

Iter *Iter_Make(MemCh *m, Span *p){
    Iter *it = MemCh_Alloc(m, sizeof(Iter));
    if(p != NULL){
        Iter_Init(it, p);
    }
    return it;
}
