#include <external.h>
#include <caneka.h>

i32 _increments[SPAN_MAX_DIMS] = {1, 16, 256, 4096, 65536};

static inline SpanIter_SetStack(SpanIter *sq, i8 dim, i32 offset){
    Span *p = sq->span; 
    void **ptr = sq->stack+dim;
    i32 increment = _increments[dim];
    i32 localIdx = (offset / increment);
    sq->stack[localIdx] = localIdx;
    if(localIdx > SPAN_STRIDE){
        printf("Stride %d\n", localIdx);
        Fatal("Error localIdx larger than span stride");
        return -1;
    }
    *ptr = (*ptr)+localIdx;
    return offset % increment;
}

status SpanIter_Next(SpanIter *it){
    return it->type.state;
}

Abstract *SpanIter_Get(SpanIter *it){
    return Span_GetFromQ(&it->sq);
}

status SpanIter_Reset(SpanIter *it){
    it->type.state |= END;
    return SUCCESS;
}


void SpanIter_Setup(SpanIter *sr, Span *p, status op, i32 idx){
    memset(sr, 0, sizeof(SpanIter));
    sr->type.of = TYPE_SPAN_QUERY;
    sr->type.state = op;
    sr->span = p;
    sr->idx = idx;
    return;
}

status Span_Query(SpanIter *sr){
    sr->type.state &= ~SUCCESS;
    MemCtx *m = sr->span->m;
    i32 idx = sr->idx;

    i8 dimsNeeded = 0;
    while(_increments[dimsNeeded+1] <= idx){
        dimsNeeded++;
    }

    Span *p = sr->span;
    MemSlab *mem_sl = NULL;
    if(dimsNeeded > p->dims){
        if((sr->type.state & (SPAN_OP_SET|SPAN_OP_RESERVE)) == 0){
            return NOOP;
        }
        slab *exp_sl = NULL;
        slab *shelf_sl = NULL;
        while(p->dims < dimsNeeded){
            slab *new_sl = NULL;
            new_sl = (slab *)MemCtx_Alloc((m), sizeof(slab));

            if(exp_sl == NULL){
                shelf_sl = sr->span->root;
                sr->span->root = new_sl;
            }else{
                void **ptr = (void *)exp_sl;
                *ptr = new_sl;
            }

            exp_sl = new_sl;
            p->dims++;
        }
        void **ptr = (void *)exp_sl;
        *ptr = shelf_sl;
    }

    i8 dim = p->dims;
    i32 offset = idx;
    void **ptr = NULL;
    while(dim != -1){
        offset = SpanIter_SetStack(sr, dim, offset);
        ptr = sr->stack+dim;
        if(*ptr == NULL){
            if((sr->type.state & (SPAN_OP_SET|SPAN_OP_RESERVE)) == 0){
                return NOOP;
            }
            *ptr = (slab *)MemCtx_Alloc((m), sizeof(slab));
        }

        dim--;
    }


    sr->type.state |= SUCCESS;
    return sr->type.state;
}


i32 Span_Capacity(Span *p){
    i32 increment = _increments[p->dims];
    return increment * SPAN_STRIDE;
}

char **Span_ToCharArr(MemCtx *m, Span *p){
    size_t sz = sizeof(char *)*(p->nvalues+1);
    char **arr = MemCtx_Alloc(m, sz);
    /*
    Iter it;
    Iter_Init(&it, p);
    int i = 0;
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)asIfc(Iter_Get(&it), TYPE_STR);
        if(s != NULL){
            arr[i++] = Str_ToCstr(m, s);
        }
    }
    */
    return arr;
}

void *Span_SetFromQ(SpanIter *sq, Abstract *t){
    Span *p = sq->span;
    p->type.state &= ~(SUCCESS|NOOP);
    status r = Span_Query(sq);
    if((r & SUCCESS) != 0){
        if(sq->idx > p->max_idx+1){
            p->type.state |= FLAG_SPAN_HAS_GAPS;
        }
        if(sq->type.state & SPAN_OP_REMOVE){
            *(sq->stack[0]) = NULL;
            p->nvalues--;
            p->metrics.set = -1;
            p->metrics.available = sq->idx;
        }else{
            *(sq->stack[0]) = t;
            p->nvalues++;
            p->metrics.set = sq->idx;
            if(sq->idx > p->max_idx){
                p->max_idx = sq->idx;
            }
        }

        return *(sq->stack[0]);
    }

    return NULL;
}

void *Span_Set(Span *p, i32 idx, Abstract *t){
    if(idx < 0 || t == NULL){
        return NULL;
    }
    SpanIter sq;
    SpanIter_Setup(&sq, p, SPAN_OP_SET, idx);
    return Span_SetFromQ(&sq, t);
}

void *Span_GetFromQ(SpanIter *sq){
    Span *p = sq->span;
    p->type.state &= ~(SUCCESS|NOOP);
    status r = Span_Query(sq);
    if((r & SUCCESS) == 0){
        p->type.state |= ERROR;
        return NULL;
    }

    if(*(st->stack[0]) != NULL){
        sq->span->type.state |= SUCCESS;
        p->metrics.get = sq->idx;
    }else{
        sq->span->type.state |= NOOP;
        p->metrics.get = -1;
    }

    return *(sq->stack[0]);
}

void *Span_Get(Span *p, i32 idx){
    if(idx < 0){
        return NULL;
    }
    SpanIter sq;
    SpanIter_Setup(&sq, p, SPAN_OP_GET, idx);
    return Span_GetFromQ(&sq);
}

void *Span_SetRaw(Span *p, i32 idx, util *u){
    if(idx < 0 || u == NULL){
        return NULL;
    }
    p->type.state |= FLAG_SPAN_RAW;
    SpanIter sq;
    SpanIter_Setup(&sq, p, SPAN_OP_SET, idx);
    return Span_SetFromQ(&sq, (Abstract *)*u);
}

util Span_GetRaw(Span *p, i32 idx){
    if(idx < 0){
        return 0;
    }
    if((p->type.state & FLAG_SPAN_RAW) == 0){
        Fatal("Tried to get raw value from non raw span", TYPE_SPAN);
        return 0;
    }
    SpanIter sq;
    SpanIter_Setup(&sq, p, SPAN_OP_GET, idx);
    status r = Span_Query(&sq);
    if((r & SUCCESS) == 0){
        p->type.state |= ERROR;
        return 0;
    }
    return (util)Span_GetFromQ(&sq);
}


void *Span_GetSelected(Span *p){
    return Span_Get(p, p->metrics.selected);
}

i32 Span_Add(Span *p, Abstract *t){
    i32 idx = p->max_idx+1;
    if(Span_Set(p, idx, t) != NULL){
        return idx;
    }

    return 0;
}

status Span_Cull(Span *p, i32 count){
    SpanIter sq;
    while(count-- > 0){
        i32 idx = p->max_idx;
        if(idx >= 0){
            SpanIter_Setup(&sq, p, SPAN_OP_REMOVE, idx);
            Span_Query(&sq);
        }
        p->max_idx--;
    }

    return NOOP;
}

status Span_Remove(Span *p, i32 idx){
    SpanIter sq;
    SpanIter_Setup(&sq, p, SPAN_OP_REMOVE, idx);
    Span_SetFromQ(&sq, NULL);
    return sq.type.state;
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

Span *Span_Clone(MemCtx *m, Span *p){
    Iter it;
    Iter_Init(&it, p);
    Span *p2 = Span_Make(m);
    int i = 0;
    while((Iter_Next(&it) & END) == 0){
        Abstract *a = asIfc(Iter_Get(&it), TYPE_STR);
        if(a != NULL){
            Span_Set(p2, it.idx, a); 
        }
    }
    return p2;
}

Span *Span_Make(MemCtx *m){
    Span *p = MemCtx_Alloc(m, sizeof(Span));
    p->type.of = TYPE_SPAN;
    p->max_idx = p->metrics.available = p->metrics.get = p->metrics.selected = p->metrics.set = -1;
    p->m = m;
    p->root = (slab *)MemCtx_Alloc((m), sizeof(slab));
    return p;
}
