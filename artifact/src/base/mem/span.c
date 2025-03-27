#include <external.h>
#include <caneka.h>

status Span_Query(Iter *it){
    it->type.state &= ~SUCCESS;
    MemCh *m = it->span->m;
    i32 idx = it->idx;

    i8 dimsNeeded = 0;
    while(_increments[dimsNeeded+1] <= idx){
        dimsNeeded++;
    }

    Span *p = it->span;
    MemSlab *mem_sl = NULL;
    if(dimsNeeded > p->dims){
        if(p->type.state & DEBUG){
            printf("\x1b[33mExpand %d to %d\x1b[0m\n", p->dims, dimsNeeded);
        }
        if((it->type.state & (SPAN_OP_SET|SPAN_OP_RESERVE)) == 0){
            return NOOP;
        }
        slab *exp_sl = NULL;
        slab *shelf_sl = NULL;
        while(p->dims < dimsNeeded){
            slab *new_sl = NULL;
            new_sl = (slab *)MemCh_Alloc((m), sizeof(slab));

            if(exp_sl == NULL){
                shelf_sl = it->span->root;
                it->span->root = new_sl;
                if(p->type.state & DEBUG){
                    printf("    \x1b[33mExpand shelving root:%lu new_sl:%lu\x1b[0m\n", (util)shelf_sl, (util)new_sl);
                }
            }else{
                void **ptr = (void **)exp_sl;
                if(p->type.state & DEBUG){
                    printf("    \x1b[33mExpand placing new_sl %lu on %lu\x1b[0m\n", (util)new_sl, (util)ptr);
                }
                *ptr = new_sl;
            }

            exp_sl = new_sl;
            p->dims++;
        }
        void **ptr = (void **)exp_sl;
        if(it->span->type.state & DEBUG){
            printf("    \x1b[33mExpand placing shelf %lu\x1b[0m\n", (util)shelf_sl);
        }
        *ptr = shelf_sl;
    }

    i8 dim = p->dims;
    i32 offset = idx;
    void **ptr = NULL;
    while(dim >= 0){
        if(it->span->type.state & DEBUG){
            printf("\x1b[33mSearching for %d at dim:%d\x1b[0m\n", idx, (i32)dim);
        }
        offset = Iter_SetStack(it, dim, offset);
        ptr = (void **)it->stack[dim];
        if(dim > 0){
            if(*ptr == NULL){
                if((it->type.state & (SPAN_OP_SET|SPAN_OP_RESERVE)) == 0){
                    printf("NOT FOUND RETUR\n");
                    return NOOP;
                }
                *ptr = (slab *)MemCh_Alloc((m), sizeof(slab));
                memset(*ptr, 0, sizeof(slab));
                if(it->span->type.state & DEBUG){
                    printf("    \x1b[33mSetting slab to %lu dim:%d\x1b[0m\n", (util)*ptr, (i32)dim);
                }
            }else{
                if(it->span->type.state & DEBUG){
                    printf("    \x1b[33mSlab exists %lu dim:%d\x1b[0m\n", (util)*ptr, (i32)dim);
                }
            }
        }

        if(it->span->type.state & DEBUG){
            printf("\x1b[35m\n");
            Iter_Print(NULL, NULL, 0, (Abstract *)it, 0, TRUE);
            printf("\x1b[0m\n");
        }

        dim--;
    }


    it->type.state |= SUCCESS;
    return it->type.state;
}


i32 Span_Capacity(Span *p){
    i32 increment = _increments[p->dims];
    return increment * SPAN_STRIDE;
}

char **Span_ToCharArr(MemCh *m, Span *p){
    size_t sz = sizeof(char *)*(p->nvalues+1);
    char **arr = MemCh_Alloc(m, sz);
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

void *Span_SetFromQ(Iter *it, Abstract *t){
    Span *p = it->span;
    p->type.state &= ~(SUCCESS|NOOP);
    status r = Span_Query(it);
    if((r & SUCCESS) != 0){
        if(it->idx > p->max_idx+1){
            p->type.state |= FLAG_SPAN_HAS_GAPS;
        }
        if(it->type.state & SPAN_OP_REMOVE){
            *((void **)it->stack[0]) = NULL;
            p->nvalues--;
            it->metrics.set = -1;
            it->metrics.available = it->idx;
        }else{
            *((void **)it->stack[0]) = t;
            p->nvalues++;
            it->metrics.set = it->idx;
            if(it->idx > p->max_idx){
                p->max_idx = it->idx;
            }
        }

        return *((void **)it->stack[0]);
    }

    return NULL;
}

void *Span_GetFromQ(Iter *it){
    Span *p = it->span;
    p->type.state &= ~(SUCCESS|NOOP);
    status r = Span_Query(it);
    if((r & SUCCESS) == 0){
        p->type.state |= ERROR;
        return NULL;
    }

    if(it->stack[0] != NULL){
        it->span->type.state |= SUCCESS;
        it->metrics.get = it->idx;
    }else{
        it->span->type.state |= NOOP;
        it->metrics.get = -1;
    }

    return *((void **)it->stack[0]);
}

void *Span_Set(Span *p, i32 idx, Abstract *t){
    if(idx < 0 || t == NULL){
        return NULL;
    }
    Iter it;
    memset(&it, 0, sizeof(Iter));
    Iter_Setup(&it, p, SPAN_OP_SET, idx);
    return Span_SetFromQ(&it, t);
}

void *Span_Get(Span *p, i32 idx){
    if(idx < 0){
        return NULL;
    }
    Iter it;
    memset(&it, 0, sizeof(Iter));
    Iter_Setup(&it, p, SPAN_OP_GET, idx);
    return Span_GetFromQ(&it);
}

void *Span_SetRaw(Span *p, i32 idx, util *u){
    if(idx < 0 || u == NULL){
        return NULL;
    }
    p->type.state |= FLAG_SPAN_RAW;
    Iter it;
    memset(&it, 0, sizeof(Iter));
    Iter_Setup(&it, p, SPAN_OP_SET, idx);
    return Span_SetFromQ(&it, (Abstract *)*u);
}

util Span_GetRaw(Span *p, i32 idx){
    if(idx < 0){
        return 0;
    }
    if((p->type.state & FLAG_SPAN_RAW) == 0){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, 
            "Tried to get raw value from non raw span");
        return 0;
    }
    Iter it;
    memset(&it, 0, sizeof(Iter));
    Iter_Setup(&it, p, SPAN_OP_GET, idx);
    status r = Span_Query(&it);
    if((r & SUCCESS) == 0){
        p->type.state |= ERROR;
        return 0;
    }
    return (util)Span_GetFromQ(&it);
}

i32 Span_Add(Span *p, Abstract *t){
    i32 idx = p->max_idx+1;
    if(Span_Set(p, idx, t) != NULL){
        return idx;
    }

    return 0;
}

status Span_Cull(Span *p, i32 count){
    Iter it;
    while(count-- > 0){
        i32 idx = p->max_idx;
        if(idx >= 0){
            memset(&it, 0, sizeof(Iter));
            Iter_Setup(&it, p, SPAN_OP_REMOVE, idx);
            Span_Query(&it);
        }
        p->max_idx--;
    }

    return NOOP;
}

status Span_Remove(Span *p, i32 idx){
    Iter it;
    memset(&it, 0, sizeof(Iter));
    Iter_Setup(&it, p, SPAN_OP_REMOVE, idx);
    Span_SetFromQ(&it, NULL);
    return it.type.state;
}

status Span_ReInit(Span *p){
    p->nvalues = 0;
    p->max_idx = -1;
    return SUCCESS;
}

status Span_Setup(Span *p){
    p->type.of = TYPE_SPAN;
    p->max_idx = -1;
    return SUCCESS;
}

Span *Span_Clone(MemCh *m, Span *p){
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

Span *Span_Make(MemCh *m){
    Span *p = MemCh_Alloc(m, sizeof(Span));
    p->type.of = TYPE_SPAN;
    p->max_idx = -1;
    p->m = m;
    p->root = (slab *)MemCh_Alloc((m), sizeof(slab));
    return p;
}
