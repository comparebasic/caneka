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

/* DEBUG */


/* API */
void *_span_Set(SpanQuery *sr, int idx, Abstract *t){

    Span *p = sr->span;
    status r = Span_Query(sr);
    SpanQuery_Print((Abstract *)sr, TYPE_SPAN_QUERY, "_span_Set: ", COLOR_PURPLE, TRUE);
    printf("\n");
    if(HasFlag(r, SUCCESS)){
        SpanState *st = sr->stack;
        void *ptr = Slab_valueAddr(st->slab, p->def, st->localIdx);
        if(HasFlag(p->def->flags, INLINE)){
            size_t sz = (size_t)p->def->itemSize;
            if(!HasFlag(p->def->flags, RAW) && t != NULL && t->type.of == TYPE_RESERVE){
                sz = sizeof(Reserve);
            }
            if(sr->op == SPAN_OP_REMOVE){
                memset(ptr, 0, sz);
            }else{
                memcpy(ptr, t, sz);
            }
        }else{
            if(sr->op == SPAN_OP_REMOVE){
                memset(ptr, 0, sizeof(void *));
            }else{
                memcpy(ptr, &t, sizeof(void *));
            }
        }
        if(t == NULL){
            p->nvalues--;
        }else{
            p->nvalues++;
        }
        if(sr->op != SPAN_OP_REMOVE){
            p->metrics.set = idx;
        }
        if(idx > p->max_idx){
            p->max_idx = idx;
        }
        sr->value = ptr;
        return sr->value;
    }
    return NULL;

}

void *Span_Set(Span *p, int idx, Abstract *t){
    printf("Span Set\n");
    if(idx < 0 || t == NULL){
        return NULL;
    }
    SpanQuery sr;
    SpanQuery_Setup(&sr, p, SPAN_OP_SET, idx);
    return _span_Set(&sr, idx, t);
}

void *Span_Get(Span *p, int idx){
    if(idx < 0){
        return NULL;
    }
    SpanQuery sr;
    SpanQuery_Setup(&sr, p, SPAN_OP_GET, idx);
    status r = Span_Query(&sr);
    if(HasFlag(r, SUCCESS)){
        SpanState *st = sr.stack;
        void *ptr = Slab_valueAddr(st->slab, p->def, st->localIdx);
        if(HasFlag(p->def->flags, INLINE)){
            if(!HasFlag(p->def->flags, RAW) && (*(util *)ptr) == 0){
                sr.value = NULL;
            }else{
                sr.value = ptr;
            }
        }else if(*((Abstract **)ptr) != NULL){
            void **dptr = (void **)ptr;
            sr.value = *dptr;
        }
        p->metrics.get = idx;
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
    SpanQuery sr;
    SpanQuery_Setup(&sr, p, SPAN_OP_REMOVE, idx);
    return Span_Query(&sr);
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

status Span_Run(MemHandle *mh, Span *p, DoFunc func){
    status r = READY;
    MHAbstract ma;
    MemCtx *m = NULL;
    if(mh != NULL){
        mh = asIfc(mh, TYPE_MEMHANDLE);
        m = mh->m;
    }
    MHAbstract_Init(&ma, m, NULL);
    for(int i = 0; i < p->nvalues; i++){
        Abstract *a = (Abstract *)Span_Get(p, i);
        if(a != NULL && (*(util *)a) != 0){
            ma.a = a;
            r |= func((MemHandle *)&ma);
        }
    }
    return r;
}

/* internals */
status Span_GrowToNeeded(SpanQuery *sr){
    boolean expand = sr->span->dims < sr->dimsNeeded;
    SpanDef *def = sr->span->def;
    Span *p = sr->span;

    if(expand){
        void *exp_sl = NULL;
        void *shelf_sl = NULL;
        while(p->dims < sr->dimsNeeded){
            void *new_sl = Span_idxSlab_Make(p->m, def);

            if(exp_sl == NULL){
                shelf_sl = sr->span->root;
                sr->span->root = new_sl;
            }else{
                Slab_setSlot(exp_sl, p->def, 0, &new_sl, sizeof(void *));
            }

            exp_sl = new_sl;
            p->dims++;
        }
        Slab_setSlot(exp_sl, p->def, 0, &shelf_sl, sizeof(void *));
    }

    sr->dims = p->dims;

    return SUCCESS;
}

status Span_Extend(SpanQuery *sq){
    /* resize the span by adding dimensions and slabs as needed */
    Span *p = sq->span;

    byte dims = p->dims;
    SpanState *st = SpanQuery_SetStack(sq, dims, 0, 0);
    while(dims > 0){

        /* make new if not exists */
        if(st->slab == NULL){
            if(sq->op != SPAN_OP_SET && sq->op != SPAN_OP_RESERVE){
                return MISS;
            }
            void *new_sl = NULL;
            if(dims > 1){
                new_sl = Span_idxSlab_Make(p->m, p->def); 
            }else{
                new_sl = Span_valueSlab_Make(p->m, p->def); 
            }
            void *prev_sl = SpanQuery_StateByDim(sq, dims+1)->slab;
            Slab_setSlot(prev_sl, p->def, st->localIdx, &new_sl, sizeof(void *));
            st->slab = new_sl;
        }

        dims--;
        /* find or allocate a space for the new span */
        st = SpanQuery_SetStack(sq, dims, 0, 0);
    }

    if(st == NULL){
        Fatal("Slab not found, SpanState is null\n", TYPE_SPAN);
    }

    /* conclude by setting the local idx and setting the state */
    st->localIdx = (sq->idx - st->offset);

    if(st->localIdx >= p->def->stride){
        Fatal("localIdx greater than stride", p->type.of);
    }

    sq->type.state = SUCCESS;
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

void *Span_reserve(SpanQuery *sr){
    return NULL;
}

status Span_ReInit(Span *p){
    p->nvalues = 0;
    p->max_idx = p->metrics.get = p->metrics.selected = p->metrics.set = -1;
    return SUCCESS;
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
    p->max_idx = -1;

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
    p->def->flags |= INLINE;
    p->root = Span_valueSlab_Make(m, p->def);
    return p;
}
