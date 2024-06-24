#include <basictypes_external.h> 
#include <caneka/caneka.h> 

Span *Span_Clone(MemCtx *m, Span *sp){
    Span *sp2 = Memcpy(m, sp, sizeof(Span));
    sp2->slab = Slab_Clone(m, sp->slab);
    if(sp->rslv != NULL){
        sp2->rslv = Resolve_Clone(m, sp->rslv);
    }
    return sp2;
}

String *Span_ToDebug(MemCtx *m, Span *p){
    if(p == NULL){
        return Nstr(m, "NULL");
    }

    lifecycle_t prevCycle = m->destCycle;
    if(m->destCycle == 0){
        m->destCycle = MEM_TEMP;
    }

    String *build = String_New(m);
    String_Extend(build, String_FormatV(m, "Span(", "%d", &(p->id), ")<\n", NULL));
    for(int i = p->ndims; i >= 1; i--){
        String_Extend(build, Span_PrintSlabsByLevel(m, p->slab, p, i)); 
    }

    String_Extend(build, Nstr(m, ">\n"));

    m->destCycle = prevCycle;
    SetFlag(build, STRING_REALIGN);
    return  String_Clone(m, build);
}

String *Span_ToString(MemCtx *m, Span *p){
    if(p == NULL){
        return Nstr(m, "NULL");
    }
    String *ret = String_New(m);
    String_Extend(ret, String_FormatN(m, 4, "Span<", "%d", &(p->id), ">"));
    return ret;
}

String *Span_ToOset(MemCtx *m, Span *sp){
    if(sp == NULL){
        return Nstr(m, "x;");
    }

    if(sp->rslv != NULL && sp->rslv->type == TYPE_TABLE_RESOLVER){
        return Table_ToOset(m, sp);
    }

    if(HasFlag(sp, FLAG_RAW)){
        printf("Error: oset from raw contents of span not yet supported");
        return NULL;
    }

    String *ret = String_Format(m, "oset/%ld=", sp->nvalues);
    Iter *it = Iter_Make(m, (Typed *)sp);
    Typed *t = NULL;
    while(!IsDone(it) && (t = Iter_Next(it))){
        String_Extend(ret, String_Format(m, "%ld!", it->position.idx));
        String_Extend(ret, Typed_ToOset(m, t));
    }

    return ret;
}

Span* Span_MakeInline(MemCtx* m, int stride, utype_t type, int itemSize){
    int slotSize = 1;
    if(itemSize > sizeof(void *)){
        slotSize = itemSize / sizeof(void *);
        if(itemSize % sizeof(void *) > 1){
            slotSize += 1;
        }
    }
    Span *sp = Span_Make(m, stride*slotSize);
    sp->itemSize = itemSize;
    sp->itemType = type;
    sp->slotSize = slotSize;
    SetFlag(sp->slab, FLAG_RAW);

    SetFlag(sp, FLAG_RAW);
    return sp;
}

lifecycle_t Span_Reset(MemCtx* m, Span *sp, utype_t type, int itemSize, void **shelf){
    if(sp->ndims > 1 || sp->nslabs > 1){
        printf("multi slab span reset not yet supported\n");
        return ERROR;
    }

    util_t shelfSize = sp->stride *sp->itemSize;
    sp->itemSize = itemSize;
    sp->itemType = type;
    sp->slotSize = itemSize / sizeof(void *);
    sp->nvalues = 0;
    if(itemSize % sizeof(void *) > 0){
        sp->slotSize++;
    }

    *shelf = Alloc(m, shelfSize);
    memcpy(*shelf, sp->slab->items, shelfSize);
    memset(sp->slab->items, 0, shelfSize);
    if(type == TYPE_TYPED){
        UnsetFlag(sp, FLAG_RAW);
        UnsetFlag(sp->slab, FLAG_RAW);
    }

    return SUCCESS;
}

Span* Span_Make(MemCtx* m, int stride){
    Span *p = Span_New(m);
    p->stride = stride;
    p->max_idx = -1;
    p->slotSize = 1;
    p->slab = Slab_Alloc(m, p->stride, p->flags & FLAG_RAW);
    p->slab->flags |= (p->flags & ACCEPT_fl);
    p->slab->increment = stride;
    p->slab->level = 1;
    p->slab->stride = stride;
    p->nslabs++;

    SetFlag(p, FLAG_SPAN_AUTOEXPAND);
    return p;
}

static int span_id = 0;
Span* Span_New(MemCtx* m){
    Span *p = Alloc(m, sizeof(Span));
    p->type = TYPE_SPAN;
    SetFlag(p, FLAG_SERIES);
    p->id = ++span_id;

    return p;
}

static int slotsAvailable(int stride, int dims){
    int n = stride;
    int _dims = dims;
    int r = n;
    if(dims <= 0){
        r = 1;
    }else if(dims == 1){
        r = stride;
    }else{
        while(dims > 1){
            n *= stride;
            dims--;
        }
        r = n;
    }

    return r;
}

static cbool_t isInSlice(SlabResult *sr, int level){
    return false;
}

static Slab *openNewSlab(MemCtx *m, Slab *sl, int stride, int local_idx, int offset, int dims, int slots_available, flags_t flags){
    Slab *new_sl = Slab_Alloc(m, stride, flags);
    new_sl->stride = stride;
    new_sl->level = dims;

    new_sl->local_idx = local_idx;
    new_sl->offset = offset;
    new_sl->increment = slots_available; 

    return new_sl;
}

static Slab *makeNewSlab(MemCtx *m, int stride, int dims, int slots_available, flags_t flags){
    Slab *new_sl = Slab_Alloc(m, stride, flags);
    new_sl->stride = stride;
    new_sl->level = dims;
    new_sl->increment = slots_available; 
    new_sl->local_idx = 0;
    new_sl->offset = 0;

    return new_sl;
}


void SlabResult_Setup(Span *p, SlabResult *sr, int idx){
    memset(sr, 0, sizeof(SlabResult));

    sr->span = p;
    sr->dims = p->ndims;
    sr->stride = p->stride;
    sr->slab = p->slab;
    sr->slotSize = p->slotSize;
    sr->idx = idx * p->slotSize;
    sr->local_idx = sr->idx;
    sr->level = p->ndims-1;
    return ;
}

static cbool_t notSufficientSlot(SlabResult *sr, int dims){
    return sr->idx >= slotsAvailable(sr->stride, dims);
}

lifecycle_t Span_Expand(MemCtx *m, SlabResult *sr){

    int dims = 1;
    while(notSufficientSlot(sr, dims)) {
        dims++;
    };

    if(dims > sr->dims){
        sr->dims = sr->span->ndims = dims;

        if(DEBUG_SPAN_EXPAND){
            printf("\x1b[%dm\x1b[1mSpan_Expand\x1b[0m\x1b[%dm idx:%d dims:%d:",
                DEBUG_SPAN_EXPAND, DEBUG_SPAN_EXPAND, sr->idx, sr->dims);
            printf("\x1b[0m\n");
        }

        int slots_available = slotsAvailable(sr->stride, dims);

        int needed = slots_available;
        cbool_t expand = sr->slab->increment < needed;
        if(expand){
            if(!sr->grow){
                sr->lifecycle = MISS;
                return sr->lifecycle;
            }

            if(DEBUG_SPAN_EXPAND){
                printf("\x1b[%dm\x1b[1mSpan_Expand\x1b[0m\x1b[%dm expanding dims %d:",
                    DEBUG_SPAN_EXPAND, DEBUG_SPAN_EXPAND, dims);
                printf("\x1b[0m\n");
            }

            Slab *exp_sl = makeNewSlab(m, sr->stride, dims, needed, sr->span->flags & FLAG_RAW);
            exp_sl->flags |= (sr->span->flags & ACCEPT_fl);
            sr->span->nslabs++;

            Slab *shelf_sl = sr->span->slab;
            sr->span->slab = exp_sl;
            needed /= sr->span->stride;
            while(sr->slab->increment < needed){
                dims--;
                Slab *new_sl = makeNewSlab(m, sr->stride, dims, needed, sr->span->flags & FLAG_RAW);
                exp_sl->items[0] = (Typed *)new_sl;
                exp_sl = new_sl;
                needed /= sr->span->stride;
            }
            exp_sl->items[0] = (Typed *)shelf_sl;
            if(DEBUG_SPAN_EXPAND){
                String_Print(Span_ToString(m, sr->span));
            }
        }else{
            if(DEBUG_SPAN_EXPAND){
                printf("\x1b[%dm\x1b[1mSpan_Expand\x1b[0m\x1b[%dm no expand\n",
                    DEBUG_SPAN_EXPAND, DEBUG_SPAN_EXPAND);
            }
        }
    }

    int offset = 0;
    int local_idx = sr->idx;
    dims = sr->dims;
    int slots_available = slotsAvailable(sr->stride, dims);
    int increment = slotsAvailable(sr->stride, dims-1);
    sr->slab = sr->span->slab;
    Slab *st = NULL;
    Slab *z_sl = NULL;
    while(dims > 1){

        int local_idx = (sr->idx - offset) / increment;
        sr->local_idx = local_idx;

        /* find or allocate a space for the new span - or return if not found and grow is false */
        st = (Slab *)sr->slab->items[local_idx];

        if(DEBUG_SPAN_EXPAND){
            printf("\x1b[%dm\x1b[1mSpan_Expand\x1b[0m\x1b[%dm --------  loop start dim %d, slots of %d, local_idx: %d, stride: %d, offset:%d -------- \x1b[0m\n",
                DEBUG_SPAN_EXPAND, DEBUG_SPAN_EXPAND, dims, slots_available, local_idx, sr->stride, offset); 
            printf("    \x1b[%dm searching at %d for %d: found %s(%d) - from: ",
                DEBUG_SPAN_EXPAND, slots_available, local_idx, Typed_ToCStr((Typed *)st), (st == NULL) ? -1 : st->id);
            String_Print(Slab_ToString(m, sr->slab, sr->span));
            printf("\x1b[0m\n");
        }

        if(st == NULL){
            if(!sr->grow){
                int idx = sr->idx;
                Span *p = sr->span;
                memset(sr, 0, sizeof(SlabResult));
                sr->idx = idx;
                sr->span = p;
                sr->lifecycle = MISS;

                return sr->lifecycle;
            }else{
                Slab *new_sl = openNewSlab(m, sr->slab, sr->stride,
                    local_idx, offset, dims-1, increment, sr->span->flags & FLAG_RAW);
                new_sl->flags |= (sr->span->flags & ACCEPT_fl);
                sr->slab->items[local_idx] = (Typed *)new_sl;
                sr->span->nslabs++;

                sr->slab = new_sl;
            }
        }else if(st->type == TYPE_SLAB){
            sr->slab = st;
        }else{
            String *msg_s = Nstr(m, "Slab not of slab type: ");
            String_Extend(msg_s, Typed_ToStringAbbrev(m, (Typed *)st));
            String_PrintColor(msg_s, ansi_red);
            String_Add(m, msg_s, '\n');

            sr->lifecycle = ERROR;
            return sr->lifecycle;
        }

        offset += increment*local_idx;
        dims--;
        slots_available = slotsAvailable(sr->stride, dims);
        increment = slotsAvailable(sr->stride, dims-1);
    }

    sr->local_idx = (sr->idx - offset);
    sr->lifecycle = SUCCESS;

    return SUCCESS;
}

lifecycle_t Span_Cull(MemCtx *m, Span *sp, int amount){
    lifecycle_t r = ERROR;
    while(amount--){
        int idx = sp->max_idx;
        r = Span_Remove(m, sp, idx);
        if(r == SUCCESS){
            sp->max_idx--;
        }else{
            break;
        }
    }
    return r;
}

lifecycle_t Span_Remove(MemCtx *m, Span *p, int idx){
    if(idx > p->max_idx){
        return MISS;
    }

    SlabResult sr;
    SlabResult_Setup(p, &sr, idx);
    Span_Expand(m, &sr);
    if(sr.lifecycle == SUCCESS){
        if(HasFlag(p, FLAG_RAW)){
            size_t size = p->slotSize*sizeof(Typed *);
            size_t offset = sr.local_idx*sizeof(Typed *);
            void *ptr = (void *)((Typed *)sr.slab->items);
            ptr += offset;

            long end = (long)ptr + (p->slotSize*sizeof(Typed *));
            long slab_end = (long)sr.slab->items+(sizeof(Typed *)*p->stride);
            long start = (long)ptr;
            long slab_start = (long)sr.slab->items;
            if(end > slab_end){
                printf("Error: Remove ptr is out of bounds for slot %ld_%ld vs %ld local_idx is %d stride is:%d size is:%d\n", start-slab_start, end-slab_start, slab_end-slab_start, sr.local_idx, sr.slab->stride, p->slotSize);
                return ERROR;
            }
            memset(ptr, 0, p->itemSize);
        }else{
            sr.slab->items[sr.local_idx] = NULL;
        }
        return SUCCESS;
    }

    return MISS;
}

lifecycle_t Span_Set(MemCtx *m, Span *p, int idx, Typed *t){
    SlabResult sr;
    SlabResult_Setup(p, &sr, idx);
    sr.grow = true;

    if(DEBUG_SPAN_SET){
        String *db = String_Format(m, "\x1b[%dmSet %d: ", DEBUG_SPAN_SET, idx);
        String_Print(db);
        printf("\x1b[0m\n");
    }

    Span_Expand(m, &sr);
    if(sr.lifecycle == SUCCESS){
        if(HasFlag(p, FLAG_RAW)){
            size_t size = p->slotSize*sizeof(Typed *);
            size_t offset = sr.local_idx*sizeof(Typed *);
            void *ptr = (void *)((Typed *)sr.slab->items);
            ptr += offset;

            long end = (long)ptr + (p->slotSize*sizeof(Typed *));
            long slab_end = (long)sr.slab->items+(sizeof(Typed *)*p->stride);
            long start = (long)ptr;
            long slab_start = (long)sr.slab->items;
            if(end > slab_end){
                printf("Error: ptr is out of bounds for slot %ld_%ld vs %ld local_idx is %d stride is:%d size is:%d\n", start-slab_start, end-slab_start, slab_end-slab_start, sr.local_idx, sr.slab->stride, p->slotSize);
                return ERROR;
            }
            memcpy(ptr, t, p->itemSize);
        }else{
            sr.slab->items[sr.local_idx] = t;
        }
        p->nvalues++;
        if(idx > p->max_idx){
            p->max_idx = idx;
        }

        return SUCCESS;
    }

    return ERROR;
}

idx_t Span_NextIdx(Span *p){
    return p->max_idx+1;
}

idx_t Span_Add(MemCtx *m, Span *p, Typed *t){
    idx_t idx = Span_NextIdx(p);
    if(Span_Set(m, p, idx, t) == SUCCESS){
        return p->max_idx;
    }

    return 0;
}

Typed *Span_Search(MemCtx *m, Span *p, Typed *t){
    if(p->rslv->cmp.type == 0){
        return NULL;
    }

    Comp *cmp = &(p->rslv->cmp);
    Typed *result = NULL;
    for(int i = 0; i <= p->max_idx; i++){
        Typed *item = Span_Get(m, p, i);
        if(item != NULL){
            Compare_With(m, item, t, cmp);
            if(cmp->lifecycle == SUCCESS){
                result = cmp->dest;
                break;
            }
        }
    };

    return result;
}

Typed *Span_Get(MemCtx *m, Span *p, int idx){

    if(idx < 0){
        idx = p->max_idx + idx+1;
    }

    if(idx < 0 || idx > p->max_idx){
        return NULL;
    }

    SlabResult sr;
    SlabResult_Setup(p, &sr, idx);

    if(DEBUG_SPAN_GET){
        printf("\x1b[%dmGet %d:", DEBUG_SPAN_GET, idx);
        printf("\x1b[0m\n");
    }

    Span_Expand(m, &sr);

    if(sr.lifecycle == SUCCESS){
        if(HasFlag(p, FLAG_RAW)){
            size_t size = p->slotSize*sizeof(Typed *);
            size_t offset = sr.local_idx*sizeof(Typed *);
            void *ptr = (void *)((Typed *)sr.slab->items);

            long end = (long)ptr + (p->slotSize*sizeof(Typed *));
            long slab_end = (long)sr.slab->items+(sizeof(Typed *)*p->stride);
            long start = (long)ptr;
            long slab_start = (long)sr.slab->items;

            if(end > slab_end){
                printf("Error: ptr to get is out of bounds for slot %ld_%ld vs %ld local_idx is %d stride is:%d size is:%d\n", start-slab_start, end-slab_start, slab_end-slab_start, sr.local_idx, sr.slab->stride, p->slotSize);
                return NULL;
            }

            return  ptr + offset;

        }

        return sr.slab->items[sr.local_idx];
    }

    return NULL;
}
