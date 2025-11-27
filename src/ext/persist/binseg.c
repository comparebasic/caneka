#include <external.h>
#include <caneka.h>

Lookup *BinSegLookup = NULL;

static status Inst_ToBinSeg(BinSegCtx *ctx, void *a, i16 id, i16 idx){
    MemCh *m = ctx->bf->m;
    status r = READY;
    Inst *inst = asInst(m, a);
    Span *ord = Seel_OrdSeel(inst->type.of);

    word sz = BinSegCtx_HeaderSize(BINSEG_TYPE_INST, 0);

    m->level++;
    Str *entry = Str_Make(m, sz);
    m->level--;

    BinSegHeader *hdr = NULL;
    i64 *ptr = NULL;
    if(ctx->type.state & BINSEG_REVERSED){
        ptr = (i64 *)entry->bytes;
        hdr = (BinSegHeader *)entry->bytes+sizeof(word);
    }else{
        hdr = (BinSegHeader *)entry->bytes;
        ptr = (i64 *)(entry->bytes+sizeof(BinSegCtx));
    }

    hdr->total = ord->nvalues;
    hdr->kind = BINSEG_TYPE_INST;
    hdr->id = id;
    hdr->idx = idx;
    *ptr = inst->type.of;

    if(ctx->type.state & BINSEG_REVERSED){
        r |= BinSegCtx_SendEntry(ctx, entry);
    }

    Iter it;
    Iter_Init(&it, inst);
    while((Iter_Next(&it) & END) == 0){
        r |= BinSegCtx_Send(ctx, Iter_Get(&it), id+1, it.idx);
    };

    if((ctx->type.state & BINSEG_REVERSED) == 0){
        r |= BinSegCtx_SendEntry(ctx, entry);
    }

    return r;
}

static status Table_ToBinSeg(BinSegCtx *ctx, void *a, i16 id, i16 idx){
    MemCh *m = ctx->bf->m;
    Table *tbl = (Span *)as(a, TYPE_TABLE);
    status r = READY;

    word sz = BinSegCtx_HeaderSize(BINSEG_TYPE_DICTIONARY, 0);

    m->level++;
    Str *entry = Str_Make(m, sz);
    m->level--;

    BinSegHeader *hdr = (BinSegHeader *)entry->bytes;

    hdr->total = tbl->nvalues;
    hdr->kind = BINSEG_TYPE_DICTIONARY;
    hdr->id = id;
    hdr->idx = idx;

    if(ctx->type.state & BINSEG_REVERSED){
        r |= BinSegCtx_ToBuff(ctx, entry);
    }

    Iter it;
    Iter_Init(&it, tbl);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);
        if(h != NULL){
            r |= BinSegCtx_Send(ctx, h->key, id+1, it.idx*2);
            r |= BinSegCtx_Send(ctx, h->value, id+1, (it.idx*2)+1);
        }
    };

    if((ctx->type.state & BINSEG_REVERSED) == 0){
        r |= BinSegCtx_ToBuff(ctx, entry);
    }

    return r;
}

static status Span_ToBinSeg(BinSegCtx *ctx, void *a, i16 id, i16 idx){
    MemCh *m = ctx->bf->m;
    m->level++;
    Span *p = (Span *)as(a, TYPE_SPAN);
    status r = READY;

    word sz = BinSegCtx_HeaderSize(BINSEG_TYPE_COLLECTION, 0);

    m->level++;
    Str *entry = Str_Make(m, sz);
    m->level--;

    BinSegHeader *hdr = (BinSegHeader *)entry->bytes;
    hdr->total = p->max_idx;
    hdr->kind = BINSEG_TYPE_COLLECTION;
    hdr->id = id;
    hdr->idx = idx;

    if(ctx->type.state & BINSEG_REVERSED){
        r |= BinSegCtx_SendEntry(ctx, entry);
    }

    Iter it;
    Iter_Init(&it, p);
    while((Iter_Next(&it) & END) == 0){
        r |= BinSegCtx_Send(ctx, Iter_Get(&it), id+1, it.idx);
        ip++;
    };

    if((ctx->type.state & BINSEG_REVERSED) == 0){
        r |= BinSegCtx_SendEntry(ctx, entry);
    }

    return r;
}

static status StrVec_ToBinSeg(BinSegCtx *ctx, void *a, i16 id, i16 idx){
    StrVec *v = (StrVec *)as(a, TYPE_STRVEC);
    MemCh *m = ctx->bf->m;
    status r = READY;

    word sz = BinSegCtx_HeaderSize(BINSEG_TYPE_BYTES_COLLECTION, 0);

    m->level++;
    Str *entry = Str_Make(m, sz);
    m->level--;

    BinSegHeader *hdr = (BinSegHeader *)entry->bytes;

    hdr->total = v->p->max_idx;
    hdr->kind = BINSEG_TYPE_BYTES_COLLECTION;
    hdr->id = id;
    hdr->idx = idx;

    if(ctx->type.state & BINSEG_REVERSED){
        r |= BinSegCtx_SendEntry(ctx, entry);
    }

    Iter it;
    Iter_Init(&it, v->p);
    while((Iter_Next(&it) & END) == 0){
        r |= Str_ToBinSeg(ctx, Iter_Get(&it), id+1, it.idx);
    };

    if((ctx->type.state & BINSEG_REVERSED) == 0){
        r |= BinSegCtx_SendEntry(ctx, entry);
    }

    return r;
}

static status I64_ToBinSeg(BinSegCtx *ctx, void *a, i16 id, i16 idx){
    MemCh *m = ctx->bf->m;
    Single *sg = (Single *)as(a, TYPE_WRAPPED_I64);

    word sz = BinSegCtx_HeaderSize(BINSEG_TYPE_NUMBER, 0);

    m->level++;
    Str *entry = Str_Make(m, sz);
    m->level--;

    BinSegHeader *hdr = NULL;
    i64 *ptr = NULL;
    if(ctx->type.state & BINSEG_REVERSED){
        ptr = (i64 *)entry->bytes;
        hdr = (BinSegHeader *)entry->bytes+sizeof(i64);
    }else{
        hdr = (BinSegHeader *)entry->bytes;
        ptr = (i64 *)(entry->bytes+sizeof(BinSegCtx));
    }

    hdr->total = 0;
    hdr->kind = BINSEG_TYPE_NUMBER;
    hdr->id = id;
    hdr->idx = idx;
    *ptr = sg->val.value;

    return BinSegCtx_SendEntry(ctx, entry);
}

static status Str_ToBinSeg(BinSegCtx *ctx, void *a, i16 id, i16 idx){
    Str *entry = (Str *)as(a, TYPE_STR);
    MemCh *m = ctx->bf->m;

    word sz = BinSegCtx_HeaderSize(BINSEG_TYPE_STRING, s->length);

    m->level++;
    Str *entry = Str_Make(m, sz);
    m->level--;

    BinSegHeader *hdr = NULL;
    byte *ptr = NULL;
    if(ctx->type.state & BINSEG_REVERSED){
        ptr = (i64 *)entry->bytes;
        hdr = (BinSegHeader *)entry->bytes+s->length;
    }else{
        hdr = (BinSegHeader *)entry->bytes;
        ptr = (i64 *)(entry->bytes+sizeof(BinSegCtx));
    }

    hdr->total = s->length;
    hdr->kind = BINSEG_TYPE_BYTES;
    hdr->id = id;
    hdr->idx = idx;
    memcpy(ptr, s->bytes, s->length);

    return BinSegCtx_SendEntry(ctx, hdr, entry);
}

static status BinSegCtx_buildKind(BinSegCtx *ctx, BinSegHeader *hdr, Str *entry){
    DebugStack_Push(ctx, ctx->type.of);
    MemCh *m = ctx->bf->m;
    
    Iter it;
    Span *shelf = Span_Get(ctx->shelves, hdr->id);
    Abstract *a = NULL;
    if(hdr->kind == BINSEG_TYPE_BYTES){
        a = (Abstract *)s;
    }else if(hdr->kind == BINSEG_TYPE_NUMBER){
        i64 *ip = (i64 *)s->bytes;
        a = (Abstract *)I64_Wrapped(m, *ip);
    }else if(hdr->kind == BINSEG_TYPE_COLLECTION){
        Span *p = (Abstract *)Span_Make(m);
        if(shelf != NULL){
            Iter_Init(&it, shelf);
            while((Iter_Next(&it) & END) == 0){
                Span_Add(p, Iter_Get(&it));
            }
        }
        a = (Abstract *)p;
    }else if(hdr->kind == BINSEG_TYPE_BYTES_COLLECTION){
        StrVec *v = StrVec_Make(m);
        if(shelf != NULL){
            Iter_Init(&it, shelf);
            while((Iter_Next(&it) & END) == 0){
                StrVec_Add(v, Iter_Get(&it));
            }
        }
        a = (Abstract *)v;
    }else if(hdr->kind == BINSEG_TYPE_DICTIONARY){
        Table *tbl = Table_Make(m);
        if(shelf != NULL){
            Iter_Init(&it, shelf);
            Abstract *key = NULL;
            while((Iter_Next(&it) & END) == 0){
                Abstract *a = Iter_Get(&it);
                if(it.idx & 1){
                    key = a;
                }else{
                    Table_Set(tbl, key, a);
                }
            }
        }
        a = (Abstract *)tbl;
    }else if(hdr->kind == BINSEG_TYPE_INST){
        word *wp = (w *)s->bytes;
        a = (Abstract *)Inst_Make(m, *wp);
    }else if(hdr->kind == BINSEG_TYPE_INST_TYPE){
        word *wp = (w *)s->bytes;
        a = (Abstract *)Word_Wrapped(m, *wp);
    }

    if(hdr->id == 0){
        Span_Add(ctx->records, a);
        ctx->bf->m++;
        ctx->shelves = Span_Make(m);
        ctx->bf->m--;
        Mem_WipeTemp(ctx->bf->m);
    }else {
        word parentId = hdr->id-1;
        shelf = Span_Get(ctx->shelves, parentId);
        if(shelf == NULL){
            ctx->bf->m++;
            shelf = Span_Make(m);
            Span_Set(ctx->shelves, shelf);
            ctx->bf->m--;
        }
        Span_Set(shelf, hdr->idx, a);
    }

    DebugStack_Pop();
    return ctx->type.state;
}

status BinSegCtx_Load(BinSegCtx *ctx){
    DebugStack_Push(ctx, ctx->type.of);
    void *args[4];
    MemCh *m = ctx->bf->m;
    ctx->type.state &= ~(SUCCESS|ERROR|NOOP);
    if(ctx->type.state & BINSEG_REVERSED){
        Buff_PosEnd(ctx->bf);
    }else{
        Buff_PosAbs(ctx->bf, 0);
    }

    i16 guard = 0;
    while((ctx->type.state & (SUCCESS|ERROR|NOOP)) == 0){
        Guard_Incr(m, &guard, BINSEG_SEG_MAX, FUNCNAME, FILENAME, LINENUMBER);
        i16 sz = sizeof(BinSegHeader);
        if(ctx->type.state & BINSEG_VISIBLE){
            sz *= 2;
        }
        Str *s = Str_Make(m, sz);

        if(ctx->type.state & BINSEG_REVERSED){
            Buff_RevGetStr(ctx->bf, s);
        }else{
            Buff_GetStr(ctx->bf, s);
        }

        if(s->length == sz){
            if(ctx->type.state & BINSEG_VISIBLE){
                s = Str_FromHex(m, s);
            }
            BinSegHeader *hdr = (BinSegHeader*)s->bytes;
            
            sz = BinSegHeader_FooterSize(hdr->total, hdr->kind);

            if(ctx->type.state & BINSEG_VISIBLE){
                sz *= 2;
            }

            Str *ftr = Str_Make(m, sz);
            ftr->type.state |= STRING_COPY;
            if(ctx->type.state & BINSEG_REVERSED){
                Buff_RevGetStr(ctx->bf, ftr);
            }else{
                Buff_GetStr(ctx->bf, ftr);
            }

            ftr->type.state &= ~STRING_COPY;

            if(ctx->type.state & BINSEG_VISIBLE){
                ftr = Str_FromHex(m, ftr);
            }

            if(BinSegCtx_buildKind(ctx, hdr, ftr) & END){
                ctx->type.state |= (SUCCESS|END);
            }
        }
    }

    DebugStack_Pop();
    return ctx->type.state;
}

word BinSegCtx_HeaderSize(word kind, word length){
    word sz = sizeof(BinSegHeader);
    if(kind == BINSEG_TYPE_INST){
        sz += sizeof(word);
    }else if(kind == BINSEG_TYPE_NUMBER){
        sz += sizeof(i64);
    }else if(kind == BINSEG_TYPE_BYTES){
        sz += length;
    }
    return sz;
}

status BinSegCtx_SendEntry(BinSegCtx *ctx, Str *entry){
    if(ctx->type.state & BINSEG_VISIBLE){
        entry = Str_ToHex(m, entry);
    }
    return Buff_Add(ctx->bf, entry);
}

status BinSegCtx_Send(BinSegCtx *ctx, void *_a, i16 id, i16 idx){
    Abstract *a = (Abstract *)_a;
    BinSegFunc func = Lookup_Get(BinSegLookup, a->type.of);
    if(func == NULL){
        void *args[] = {
            Type_ToStr(ctx->bf->m, a->type.of),
            NULL,
        };
        Error(ctx->bf->m, FUNCNAME, FILENAME, LINENUMBER,
            "Unable to find BinSegFunc for type $", args);
        return 0;
    }
    return func(ctx, a, id, idx);
}

BinSegCtx *BinSegCtx_Make(Buff *bf, word flags, Span *insts){
    MemCh *m = bf->m;
    BinSegCtx *ctx = (BinSegCtx *)MemCh_AllocOf(m, sizeof(BinSegCtx), TYPE_BINSEG_CTX);
    ctx->type.of = TYPE_BINSEG_CTX;
    ctx->type.state = flags;
    bf->type.state |= BUFF_DATASYNC;
    ctx->bf = bf;
    ctx->schema.insts = insts;
    ctx->shelves = Span_Make(m);
    Iter_Init(&ctx->schema.ords, Span_Make(m));
    Iter_Init(&ctx->it, Span_Make(m));

    Iter it;
    Iter_Init(&it, insts);
    while((Iter_Next(&it) & END) == 0){
        Single *sg = Iter_Get(&it); 
        Span *ord = Seel_GetSeel(sg->val.w);
        Iter_SetByIdx(&ctx->schema.ords, it.idx, ord);
    }

    return ZERO;
}

status BinSeg_Init(MemCh *m){
    status r = READY;
    if(BinSegLookup == NULL){
        BinSegLookup = Lookup_Make(m, _TYPE_ZERO);
        r |= BinSeg_BasicInit(m, BinSegLookup);
        r |= Lookup_Add(m, BinSegLookup, TYPE_STR, (void *)Str_ToBinSeg);
        r |= Lookup_Add(m, BinSegLookup, TYPE_SPAN, (void *)Span_ToBinSeg);
        r |= Lookup_Add(m, BinSegLookup, TYPE_TABLE, (void *)Table_ToBinSeg);
        r |= Lookup_Add(m, BinSegLookup, TYPE_STRVEC, (void *)StrVec_ToBinSeg);
        r |= Lookup_Add(m, BinSegLookup, TYPE_INSTANCE, (void *)Inst_ToBinSeg);
        r |= Lookup_Add(m, BinSegLookup, TYPE_WRAPPED_I64, (void *)I64_ToBinSeg);
    }
    return r;
}
