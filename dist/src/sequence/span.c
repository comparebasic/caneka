#include <external.h>
#include <caneka.h>

int Span_availableByDim(int dims, int stride, int idxStride){
    int _dims = dims;
    int n = stride;
    int r = n;
    if(dims <= 0){
        r = 1;
    }else if(dims == 1){
        return stride;
    }else{
        while(dims > 1){
            n *= idxStride;
            dims--;
        }
        r = n;
    }

    return r;
}

int Span_Capacity(Span *p){
    int increment = Span_availableByDim(p->dims, p->def->stride, p->def->idxStride);
    return increment * p->def->idxStride;
}

/* DEBUG */


/* API */
char **Span_ToCharArr(MemCtx *m, Span *p){
    size_t sz = sizeof(char *)*(p->nvalues+1);
    char **arr = MemCtx_Alloc(m, sz);
    memset(arr, 0, sz); 
    Iter it;
    Iter_Init(&it, p);
    int i = 0;
    while((Iter_Next(&it) & END) == 0){
        String *s = (String *)asIfc(Iter_Get(&it), TYPE_STRING);
        if(s != NULL){
            arr[i++] = String_ToChars(m, s);
        }
    }
    return arr;
}
void *Span_SetFromQ(SpanQuery *sq, Abstract *t){

    Span *p = sq->span;
    status r = Span_Query(sq);

    if(DEBUG_SPAN_GET_SET){
        printf("\x1b[%dm", DEBUG_SPAN_GET_SET);
        SpanDef_Print(sq->span->def);
        printf("\n");
        SpanQuery_Print((Abstract *)sq, TYPE_SPAN_QUERY, "Span_SetFromQ: ", DEBUG_SPAN_GET_SET, TRUE);
        printf("\n");
        printf("\n\x1b[0m");
    }

    if((r & SUCCESS) != 0){
        SpanState *st = sq->stack;
        if(sq->idx > p->max_idx+1){
            p->type.state |= FLAG_SPAN_HAS_GAPS;
        }
        void *ptr = Slab_valueAddr(st->slab, p->def, st->localIdx);
        if((p->def->flags & SPAN_INLINE) != 0){
            size_t sz = (size_t)p->def->itemSize;
            if((p->def->flags & SPAN_RAW) == 0 && t != NULL && t->type.of == TYPE_RESERVE){
                sz = sizeof(Reserve);
            }
            if(sq->op == SPAN_OP_REMOVE){
                memset(ptr, 0, sz);
            }else{
                memcpy(ptr, t, sz);
            }
        }else{
            if(sq->op == SPAN_OP_REMOVE){
                memset(ptr, 0, sizeof(void *));
            }else{
                if(p->m->type.range == -1){
                    LocalPtr lp;
                    if((MemLocal_GetLocal(p->m, &t, (LocalPtr *)&lp) & SUCCESS) != 0){
                        memcpy(ptr, &lp, sizeof(void *));
                    }
                }else{
                    memcpy(ptr, &t, sizeof(void *));
                }
            }
        }
        if(sq->op == SPAN_OP_REMOVE){
            p->nvalues--;
        }else{
            p->nvalues++;
        }
        if(sq->op != SPAN_OP_REMOVE){
            p->metrics.set = sq->idx;
        }
        if(sq->idx > p->max_idx){
            p->max_idx = sq->idx;
        }
        sq->value = ptr;
        return sq->value;
    }
    return NULL;

}

void *Span_Set(Span *p, int idx, Abstract *t){
    if(idx < 0 || t == NULL){
        return NULL;
    }
    SpanQuery sq;
    SpanQuery_Setup(&sq, p, SPAN_OP_SET, idx);
    return Span_SetFromQ(&sq, t);
}

void *Span_GetFromQ(SpanQuery *sq){
    Span *p = sq->span;
    SpanDef *def = p->def;

    if(DEBUG_SPAN_GET_SET){
        printf("\x1b[%dm", DEBUG_SPAN_GET_SET);
        SpanDef_Print(sq->span->def);
        printf("\n");
        SpanQuery_Print((Abstract *)sq, TYPE_SPAN_QUERY, "Span_Get: ", DEBUG_SPAN_GET_SET, TRUE);
        printf("\n");
        printf("\n\x1b[0m");
    }

    SpanState *st = sq->stack;
    void *ptr = Slab_valueAddr(st->slab, def, st->localIdx);
    if((def->flags & SPAN_INLINE) != 0){
        if((def->flags & SPAN_RAW) == 0 && (*(util *)ptr) == 0){
            sq->value = NULL;
        }else{
            sq->value = ptr;
        }
    }else if(*((Abstract **)ptr) != NULL){
        void **dptr = (void **)ptr;
        if(p->m->type.range == -1){
            LocalPtr lp;
            if((MemLocal_GetLocal(p->m, *dptr, (LocalPtr *)&lp) & SUCCESS) != 0){
                memcpy(ptr, &lp, sizeof(void *));
            }

        }else{
            sq->value = *dptr;
        }
    }
    p->metrics.get = sq->idx;
    return sq->value;
}

void *Span_Get(Span *p, int idx){
    if(idx < 0){
        return NULL;
    }
    SpanQuery sq;
    SpanQuery_Setup(&sq, p, SPAN_OP_GET, idx);
    status r = Span_Query(&sq);
    if((r & SUCCESS) == 0){
        p->type.state |= ERROR;
        return NULL;
    }
    return Span_GetFromQ(&sq);
}

void *Span_GetSelected(Span *p){
    return Span_Get(p, p->metrics.selected);
}

int Span_AddOrdered(Span *p, Abstract *t){
    int idx = p->max_idx+1;
    int bIdx = p->max_idx;
    Abstract *b = Span_Get(p, bIdx);
    while(Abs_Cmp(t, b) > 0){
        Span_Set(p, idx, b);
        idx = bIdx;
        b = Span_Get(p, --bIdx);
    }
    if(Span_Set(p, idx, t) != NULL){
        return idx;
    }
    return -1;
}

int Span_Add(Span *p, Abstract *t){
    if((p->type.state & SPAN_ORDERED) != 0){
        return Span_AddOrdered(p, t);
    }
    int idx = Span_NextIdx(p);
    if(Span_Set(p, idx, t) != NULL){
        return idx;
    }

    return 0;
}

void *Span_ReserveNext(Span *p){
    Reserve rsv;
    memset(&rsv, 0, sizeof(Reserve));
    rsv.type.of = TYPE_RESERVE;
    return Span_Set(p, Span_NextIdx(p), (Abstract *)&rsv);
}

status Span_Remove(Span *p, int idx){
    SpanQuery sq;
    SpanQuery_Setup(&sq, p, SPAN_OP_REMOVE, idx);
    return Span_Query(&sq);
}

status Span_Cull(Span *p, int count){
    SpanQuery sq;
    while(count-- > 0){
        int idx = p->max_idx;
        if(idx >= 0){
            SpanQuery_Setup(&sq, p, SPAN_OP_REMOVE, idx);
            Span_Query(&sq);
        }
        p->max_idx--;
    }

    return NOOP;
}

status Span_Concat(Span *p, Span *add){
    status r = READY;
    Iter it;
    Iter_Init(&it, add);
    while((Iter_Next(&it) & END) == 0){
        Abstract *a = Iter_Get(&it);
        if(a != NULL){
            r |= Span_Add(p, a);
        }
    }

    if(r == READY){
        r |= NOOP;
    }

    return r;
}

status Span_Move(Span *p, int fromIdx, int toIdx){
    return NOOP;
}

/* utils */
Span *Span_From(MemCtx *m, int count, ...){
    Span *p = Span_Make(m, TYPE_SPAN);
    Abstract *v = NULL;
	va_list arg;
    va_start(arg, count);
    for(int i = 0; i < count; i++){
        v = va_arg(arg, AbstractPtr);
        Span_Add(p, v);
    }
    return p;
}

status Span_Run(MemCtx *m, Span *p, DoFunc func){
    status r = READY;
    for(int i = 0; i < p->nvalues; i++){
        Abstract *a = (Abstract *)Span_Get(p, i);
        if(a != NULL && (*(util *)a) != 0){
            r |= func(m, a);
        }
    }
    return r;
}

/* internals */
status Span_GrowToNeeded(SpanQuery *sq){
    boolean expand = sq->span->dims < sq->dimsNeeded;
    SpanDef *def = sq->span->def;
    Span *p = sq->span;

    if(expand){
        void *exp_sl = NULL;
        void *shelf_sl = NULL;
        while(p->dims < sq->dimsNeeded){
            void *new_sl = Span_idxSlab_Make(p->m, def);

            if(exp_sl == NULL){
                shelf_sl = sq->span->root;
                sq->span->root = new_sl;
            }else{
                Slab_setSlot(exp_sl, p->def, 0, &new_sl, sizeof(void *));
            }

            exp_sl = new_sl;
            p->dims++;
        }
        Slab_setSlot(exp_sl, p->def, 0, &shelf_sl, sizeof(void *));
    }

    sq->dims = p->dims;

    return SUCCESS;
}

status Span_Extend(SpanQuery *sq){
    /* resize the span by adding dimensions and slabs as needed */
    Span *p = sq->span;

    byte dims = p->dims;
    SpanState *st = NULL;
    while(TRUE){
        /* make new if not exists */
        st = SpanQuery_SetStack(sq, dims, 0, 0);
        if(st->slab == NULL){
            if(sq->op != SPAN_OP_SET && sq->op != SPAN_OP_RESERVE){
                return NOOP;
            }
            void *new_sl = NULL;
            if(dims > 1){
                new_sl = Span_idxSlab_Make(p->m, p->def); 
            }else{
                new_sl = Span_valueSlab_Make(p->m, p->def); 
            }
            SpanState *prev = SpanQuery_StateByDim(sq, dims+1);
            Slab_setSlot(prev->slab, p->def, prev->localIdx, &new_sl, sizeof(void *));
            st->slab = new_sl;
        }else{
            sq->queryDim = dims;
        }

        /* find or allocate a space for the new span */
        if(dims == 0){
            break;
        }

        dims--;
    }

    if(st == NULL){
        Fatal("Slab not found, SpanState is null\n", TYPE_SPAN);
    }

    if(st->localIdx >= p->def->stride){
        Fatal("localIdx greater than stride", p->type.of);
    }

    sq->type.state |= SUCCESS;
    return sq->type.state;
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
    while(Span_availableByDim(dims, def->stride, def->idxStride) < nslabs){
        dims++;
    }

    return dims;
}

void *Span_valueSlab_Make(MemCtx *m, SpanDef *def){
    i64 sz = SPAN_VALUE_SIZE(def);
    return MemCtx_Alloc(m, sz);
}

void *Span_idxSlab_Make(MemCtx *m, SpanDef *def){
    i64 sz = SPAN_IDX_SIZE(def);
    return MemCtx_Alloc(m, sz);
}

void *Span_reserve(SpanQuery *sq){
    return NULL;
}

status Span_ReInit(Span *p){
    p->nvalues = 0;
    p->max_idx = p->metrics.get = p->metrics.selected = p->metrics.set = -1;
    return SUCCESS;
}

Span *Span_Clone(MemCtx *m, Span *op){
    Span *p = Span_Make(m, op->type.of);
    if((p->def->flags & SPAN_INLINE) != 0){
        p->def = SpanDef_Clone(m, SpanDef_FromCls(op->type.of));
        p->type.of = p->def->typeOf;
    }
    Iter it;
    Iter_Init(&it, op);
    while((Iter_Next(&it) & END) == 0){
        Abstract *o = Iter_Get(&it);
        if(o != NULL){
            Abstract *a = Clone(m, o); 
            if(a == NULL){
                return NULL;
            }
            Span_Set(p, it.idx, a);
        }
    }
    return p;
}

Span *Span_Make(MemCtx *m, cls type){
    Span *p = MemCtx_Alloc(m, sizeof(Span));
    p->m = m;
    p->def = SpanDef_FromCls(type);
    p->type.of = p->def->typeOf;
    p->root = Span_valueSlab_Make(m, p->def);
    p->max_idx = -1;
    return p;
}

Span* Span_MakeInline(MemCtx* m, cls type, int itemSize){
    Span *p = MemCtx_Alloc(m, sizeof(Span));
    p->m = m;
    p->def = SpanDef_Clone(m, SpanDef_FromCls(type));
    p->type.of = p->def->typeOf;
    p->max_idx = p->metrics.get = p->metrics.selected = p->metrics.set = -1;

    p->def->itemSize = itemSize;
    int slotSize = 1;
    if(itemSize > sizeof(void *)){
        slotSize = itemSize / sizeof(void *);
        if(itemSize % sizeof(void *) > 1){
            slotSize += 1;
        }
    }
    int pwrSlot = p->def->stride;
    while((pwrSlot / 2) >= slotSize){
        pwrSlot /= 2;
    }

    p->def->slotSize = pwrSlot;
    p->def->flags |= SPAN_INLINE;
    p->root = Span_valueSlab_Make(m, p->def);
    return p;
}
