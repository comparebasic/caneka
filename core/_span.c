#include <external.h>
#include <caneka.h>

static status Span_Extend(SlabResult *sr);
static status span_GrowToNeeded(SlabResult *sr);

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
/* DEBUG */

static void showSlab(void *sl, int color, boolean extended, int indent){
    /*
    Slab_Print((Abstract *)sl, TYPE_SLAB, "", color, extended); 
    if(sl->increment > SPAN_DIM_SIZE){
        indent++;
        printf("\n");
        boolean first = TRUE;
        for(int i = 0; i < SPAN_DIM_SIZE; i++){
            Abstract *t = sl->items[i];
            if(t != NULL){
                if(!first){
                    printf("\n");
                }
                if(first){
                    first = FALSE;
                }
                indent_Print(indent);
                printf("\x1b[%dm%d=", color, i);
                showSlab((void *)t, color, extended, indent);
            }
        }
    }
    */
}

static void slab_Summarize(void *slab, char *msg, int color, boolean extended){
    /*
    printf("%s\x1b[0;%dmL<incr%d[%d] ", msg, color, slab->increment, slab->offset);
    boolean first = TRUE;
    for(int i = 0; i < SPAN_DIM_SIZE; i++){
        Abstract *t = slab->items[i];
        if(t != NULL){
            if(!first){
                printf(", ");
            }
            if(first){
                first = FALSE;
            }
            printf("%d=", i);
            if(t->type.of == TYPE_SLAB){
                void *slt = (void *)t;
                printf("%u", slt->offset);
            }else{
                printf("%p", t);
            }
            printf("\x1b[%dm", color);
        }
    }
    printf("\x1b[0;%dm>\x1b[0m", color);
    */
}


static void Slab_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    /*
    void *slab = (void *)a;
    if(slab->increment != SPAN_DIM_SIZE){
        return slab_Summarize(slab, msg, color, extended);
    }

    printf("%s\x1b[%dmL<icr%d[%d] \x1b[%dm", msg, color, slab->increment, slab->offset, color);
    boolean first = TRUE;
    for(int i = 0; i < SPAN_DIM_SIZE; i+= slab->slotSize){
        void *t = NULL;
        if((slab->type.state & RAW) != 0){
            t = Slab_GetPtr(slab, i);
        }else{
            t = slab->items[i];
        }
        if(t != NULL && ((Abstract *)t)->type.of != 0){
            if(!first){
                printf(", ");
            }
            if(first){
                first = FALSE;
            }
            printf("%d=", i);
            Debug_Print((void *)t, 0, "", color, extended); 

            printf("\x1b[%dm", color);
        }
    }
    printf("\x1b[%dm>\x1b[0m", color);
    */
}

static void SpanDef_Print(SpanDef *def){
    printf("def=[idxStride:%d stride:%d idxSize:%d slotSize:%d itemSize:%d, valueHdr:%d]", 
         def->idxStride, def->stride, def->idxSize, def->slotSize, def->itemSize, def->valueHdr);
}

void Span_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Span *p = (Span *)asIfc(a, TYPE_SPAN);
    printf("\x1b[0;%dm%sP<%u items in %u dims ", color, msg,
        p->nvalues, p->dims);
    if(extended){
        SpanDef_Print(p->def);
    }
    printf(">\x1b[0m");
    /*
    indent_Print(1);
    showSlab(p->slab, color, extended, 1);
    printf("\n\x1b[0;%dm>\x1b[0m\n", color);
    */
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

status Span_Query(SlabResult *sr){
    printf("Query %d\n", sr->idx);
    MemCtx *m = sr->span->m;
    if(sr->dimsNeeded > sr->dims){
        if(sr->op == SPAN_OP_GET){
            return MISS;
        }
        if(sr->op != SPAN_OP_SET && sr->op != SPAN_OP_RESERVE){
            return MISS;
        }
        span_GrowToNeeded(sr);
        printf("Grew to %d\n", sr->span->dims);
    }
    return Span_Extend(sr);
}

status Span_Set(Span *p, int idx, Abstract *t){
    SlabResult sr;
    SlabResult_Setup(&sr, p, SPAN_OP_SET, idx);
    status r = Span_Query(&sr);
    /*
    if(HasFlag(r, SUCCESS)){
        void **ptr = NULL;
        Span_addrByIdx(&sr, ptr);
        if(HasFlag(p->def->flags, RAW)){
            memcpy(*ptr, t, p->def->itemSize);
        }else{
            *ptr = (void *)t;
        }
        return SUCCESS;
    }
    */
    return r;
}

void *Span_Get(Span *p, int idx){
    SlabResult sr;
    SlabResult_Setup(&sr, p, SPAN_OP_GET, idx);
    status r = Span_Query(&sr);
    if(HasFlag(r, SUCCESS)){
        return sr.value;
    }else{
        return NULL;
    }
}

status Span_Remove(Span *p, int idx){
    SlabResult sr;
    SlabResult_Setup(&sr, p, SPAN_OP_REMOVE, idx);
    return Span_Query(&sr);
}

status Span_Move(Span *p, int fromIdx, int toIdx){
    return NOOP;
}

/* internals */
static status span_GrowToNeeded(SlabResult *sr){
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

    sr->slab = sr->span->root;
    return SUCCESS;
}

static status Span_Extend(SlabResult *sr){
    /* resize the span by adding dimensions and slabs as needed */
    Span *p = sr->span;

    byte dims = p->dims;
    void *prev_sl = sr->slab;
    printf("Extend called\n");
    while(dims > 0){
        int increment = availableByDim(dims, p->def->idxStride);
        sr->local_idx = ((sr->idx - sr->offset) / increment);
        sr->offset += increment*sr->local_idx;
        printf("local_idx %d/%d\n", sr->local_idx, increment);

        /* find or allocate a space for the new span */
        sr->slab = (void *)Span_nextSlot(sr);

        /* make new if not exists */
        if(sr->slab == NULL){
            if(sr->op != SPAN_OP_SET && sr->op != SPAN_OP_RESERVE){
                return MISS;
            }
            void *new_sl = Span_idxSlab_Make(p->m, p->def); 
            Slab_setSlot(prev_sl, p->def, sr->local_idx, new_sl, sizeof(void *));
            prev_sl = sr->slab = new_sl;
        }else{
            prev_sl = sr->slab;
        }

        dims--;
    }

    /* conclude by setting the local idx and setting the state */
    sr->local_idx = (sr->idx - sr->offset);

    sr->type.state = SUCCESS;
    return sr->type.state;
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
    i64 sz = sizeof(Abstract *)*(def->idxSize)*def->idxStride;
    return MemCtx_Alloc(m, sz);
}

status Span_addrByIdx(SlabResult *sr, void **addr){
    SpanDef *def = sr->span->def;
    int pos = sr->local_idx*(def->slotSize);
    printf("Getting addr %d in span %p\n", pos, sr->slab);
    /*
    *addr = (void *)(((void *)sr->slab)+pos);
    */
    return SUCCESS;
}

void *Span_nextSlot(SlabResult *sr){
    SpanDef *def = sr->span->def;
    void *sl = (void *)sr->slab;
    int pos = sr->local_idx*(def->idxSize)*sizeof(void *);
    void **ptr = (void *)(sl)+pos;
    printf("Getting local_idx:%d slot addr %d/%ld in span %p ptr:%p\n", sr->local_idx, pos, pos / sizeof(void *), sl, *ptr);
    return *ptr;
}

void *Span_reserve(SlabResult *sr){
    return NULL;
}

Span *Span_Make(MemCtx *m, cls type){
    Span *p = MemCtx_Alloc(m, sizeof(Span));
    p->m = m;
    p->def = SpanDef_FromCls(type);
    p->type.of = p->def->typeOf;
    p->root = Span_valueSlab_Make(m, p->def);
    return p;
}
