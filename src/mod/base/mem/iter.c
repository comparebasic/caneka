#include <external.h>
#include "base_module.h"

static status Iter_Query(Iter *it){
    it->type.state &= ~(NOOP|MORE|TAIL);
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

    if(it->idx >= p->size){
        printf("Resizing\n");
        fflush(stdout);
        if(!fill){
            it->type.state |= NOOP;
            return it->type.state;
        }

        i8 dims = p->range.range;
        i64 size = p->size;
        while((size-1) < it->idx){
            size *= SPAN_STRIDE;
            printf("dims resizing currently %d, size %ld\n", (i32)dims, size);
            fflush(stdout);
            if(dims >= DIM_MAX){
                Fatal(m, FUNCNAME, FILENAME, LINENUMBER,
                    "Span unable to grow to greater than STRIDE^DIM_MAX", NULL);
            }
            dims++;
        }

        printf("Resizing to dims %d size %ld\n", dims, size);
        fflush(stdout);

        Slate *prev = NULL;
        Slate *shelf = NULL;
        Slate *sl = NULL;
        while(p->range.range < dims){
            sl = Slate_Make(m);
            if(prev == NULL){
                shelf = it->p->root;
                it->p->root = sl;
            }else{
                prev->slots[0] = sl;
            }

            prev = sl;
            p->range.range++;
        }

        prev->slots[0] = shelf;
        p->size = size;
        printf("p->size is now %ld new slab is %p\n", size, sl);
        fflush(stdout);
        p->range.range = dims;
    }

    if(it->range.range == 0 || it->range.range < p->range.range){
        it->stack = (Slate **)MemCh_Alloc(m, SizeOf(Slate *)*(p->range.range*2));
        it->localIdx = (i8 *)MemCh_Alloc(m, SizeOf(i8)*(p->range.range*2));
        it->range.range = p->range.range;
    }

    i8 idim = (i8)p->range.range;
    while(idim >= 0){
        i64 local = (it->idx >> (SPAN_DIM_SHIFT*idim)) & (SPAN_LOCAL_MAX);
        printf("\x1b[33mFinding a place dim %d idx %ld local %ld\n", p->range.range - idim, it->idx, local);
        for(i16 i = 0; i <= p->range.range; i++){
            printf("  dim%d @%p + local %d\n", i, it->stack[i], (i32)it->localIdx[i]);
        }
        printf("\x1b[0m\n");
        fflush(stdout);

        Slate *sl = NULL;
        if(idim == p->range.range){
            it->stack[idim] = p->root;
            it->localIdx[idim] = local;
            sl = p->root;
        }else{
            Slate *parent = it->stack[idim+1];
            sl = parent->slots[it->localIdx[idim+1]];
            printf("    non 0 idim parent %p sl %p local %d\n", 
                parent,
                sl,
                it->localIdx[idim+1]);
            fflush(stdout);
            if(sl == NULL){
                printf("    sl is NULL - MAKING non 0 idim sl\n");
                fflush(stdout);
                if(!fill){
                    it->type.state |= NOOP;
                    break;
                }else{
                    sl = Slate_Make(m);
                    parent->slots[it->localIdx[idim+1]] = sl;
                    printf("    new mid sl is %p/%p\n", 
                        sl, 
                        parent->slots[it->localIdx[idim+1]]);
                    fflush(stdout);
                }
            }
            it->stack[idim] = sl;
            it->localIdx[idim] = local;
            printf("> parent is %p  sl is %p\n", parent, sl);
            fflush(stdout);
        }

        if(idim == 0){
            printf("    idim 0 sl = %p\n", sl);
            fflush(stdout);
            void *ptr = sl->slots[local];
            if(!fill){
                if(ptr == NULL){
                    it->type.state |= NOOP;
                }else{
                    it->value = ptr;
                }
                break;
            }else{
                if(it->type.state & (ITER_SET|ITER_ADD)){
                    if(ptr == NULL){
                        sl->idx.count++;
                    }
                    sl->slots[local] = it->value;
                    if(local > sl->idx.max){
                        sl->idx.max = local;
                    }
                    if(local < sl->idx.min){
                        sl->idx.min = local;
                    }
                }else if(it->type.state & ITER_REMOVE){
                    if(ptr != NULL){
                        sl->idx.count--;
                    }
                    sl->slots[local] = NULL;
                    if(local == sl->idx.min){
                       for(i64 i = local; i < SPAN_STRIDE; i++){
                            if(sl->slots[i] != NULL){
                                sl->idx.min = i;
                                break;
                            }
                       }
                    }
                    if(local == sl->idx.max){
                        for(i64 i = local; i > 0; i--){
                            if(sl->slots[i] != NULL){
                                sl->idx.max = i;
                                break;
                            }
                        }
                    }
                }
            }
        }
        idim--;
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
