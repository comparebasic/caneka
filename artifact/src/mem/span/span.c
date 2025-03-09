#include <external.h>
#include <caneka.h>

i32 _increments[SPAN_MAX_DIMS] = {16, 256, 4096, 65536, 1048576};

int Span_Capacity(Span *p){
    int increment = _increments[p->dims];
    return increment * SPAN_STRIDE;
}

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
    if((r & SUCCESS) != 0){
        SpanState *st = sq->stack;
        if(sq->idx > p->max_idx+1){
            p->type.state |= FLAG_SPAN_HAS_GAPS;
        }
        void *ptr = Slab_valueAddr(st->slab, st->localIdx);
        if(sq->op == SPAN_OP_REMOVE){
            memset(ptr, 0, sizeof(void *));
            p->nvalues--;
            p->metrics.set = -1;
            p->metrics.available = sq->idx;
            if(sq->idx >= p->max_idx){
                p->max_idx--;
            }
        }else{
            memcpy(ptr, &t, sizeof(void *));
            p->nvalues++;
            p->metrics.set = sq->idx;
            if(sq->idx > p->max_idx){
                p->max_idx = sq->idx;
            }
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
    SpanState *st = sq->stack;
    printf("getting %d from %p\n", st->localIdx, st->slab);
    void *ptr = Slab_valueAddr(st->slab, st->localIdx);
    sq->span->type.state &= ~(SUCCESS|NOOP);
    if(*((Abstract **)ptr) != NULL){
        void **dptr = (void **)ptr;
        sq->value = *dptr;
        sq->span->type.state |= SUCCESS;
    }else{
        sq->span->type.state |= NOOP;
        sq->value = NULL;
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

int Span_Add(Span *p, Abstract *t){
    int idx = Span_NextIdx(p);
    if(Span_Set(p, idx, t) != NULL){
        return idx;
    }

    return 0;
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


status Span_Remove(Span *p, int idx){
    SpanQuery sq;
    SpanQuery_Setup(&sq, p, SPAN_OP_REMOVE, idx);
    Span_SetFromQ(&sq, NULL);
    return sq.type.state;
}

/* internals */
byte Span_GetDimNeeded(int idx){
    if(idx < SPAN_STRIDE){
        return 0;
    }

    int nslabs = idx / SPAN_STRIDE;
    if(idx % nslabs > 0){
        nslabs++;
    }
    int dims = 1;
    while(_increments[dims] < nslabs){
        dims++;
    }

    return dims;
}

status Span_ReInit(Span *p){
    p->nvalues = 0;
    p->max_idx = p->metrics.get = p->metrics.selected = p->metrics.set = -1;
    return SUCCESS;
}

status Span_Setup(Span *p){
    p->type.of = TYPE_SPAN;
    p->max_idx = p->metrics.available = \
        p->metrics.get = p->metrics.selected = p->metrics.set = -1;
    return SUCCESS;
}

Span *Span_Make(MemCtx *m){
    Span *p = MemCtx_Alloc(m, sizeof(Span));
    p->type.of = TYPE_SPAN;
    p->max_idx = p->metrics.available = p->metrics.get = p->metrics.selected = p->metrics.set = -1;
    p->m = m;
    p->root = Slab_Make(m);
    return p;
}
