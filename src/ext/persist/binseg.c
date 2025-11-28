#include <external.h>
#include <caneka.h>

Lookup *BinSegLookup = NULL;

static status Inst_ToBinSeg(BinSegCtx *ctx, void *a, i16 id, i16 idx){
    MemCh *m = ctx->bf->m;
    status r = READY;
    Inst *inst = asInst(m, a);
    Span *ord = Seel_OrdSeel(m, inst->type.of);

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
        ptr = (i64 *)(entry->bytes+sizeof(BinSegHeader));
    }

    hdr->total = ord->nvalues;
    hdr->kind = BINSEG_TYPE_INST;
    hdr->ident.id = id;
    hdr->ident.idx = idx;
    *ptr = inst->type.of;

    if(ctx->type.state & BINSEG_REVERSED){
        r |= Buff_Add(ctx->bf, entry);
    }

    Iter it;
    Iter_Init(&it, inst);
    while((Iter_Next(&it) & END) == 0){
        r |= BinSegCtx_SendByIdent(ctx, Iter_Get(&it), id+1, it.idx);
    };

    if((ctx->type.state & BINSEG_REVERSED) == 0){
        r |= Buff_Add(ctx->bf, entry);
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
    entry->length = entry->alloc;
    m->level--;

    BinSegHeader *hdr = (BinSegHeader *)entry->bytes;

    hdr->total = tbl->nvalues;
    hdr->kind = BINSEG_TYPE_DICTIONARY;
    hdr->ident.id = id;
    hdr->ident.idx = idx;

    if(ctx->type.state & BINSEG_REVERSED){
        r |= Buff_Add(ctx->bf, entry);
    }

    Iter it;
    Iter_Init(&it, tbl);
    i32 count = 0;
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);
        if(h != NULL){
            r |= BinSegCtx_SendByIdent(ctx, h->key, id+1, count*2);
            r |= BinSegCtx_SendByIdent(ctx, h->value, id+1, (count*2)+1);
            count++;
        }
    };

    if((ctx->type.state & BINSEG_REVERSED) == 0){
        r |= Buff_Add(ctx->bf, entry);
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
    entry->length = entry->alloc;
    m->level--;

    BinSegHeader *hdr = (BinSegHeader *)entry->bytes;
    hdr->total = p->max_idx;
    hdr->kind = BINSEG_TYPE_COLLECTION;
    hdr->ident.id = id;
    hdr->ident.idx = idx;

    if(ctx->type.state & BINSEG_REVERSED){
        r |= Buff_Add(ctx->bf, entry);
    }

    Iter it;
    Iter_Init(&it, p);
    while((Iter_Next(&it) & END) == 0){
        r |= BinSegCtx_SendByIdent(ctx, Iter_Get(&it), id+1, it.idx);
    };

    if((ctx->type.state & BINSEG_REVERSED) == 0){
        r |= Buff_Add(ctx->bf, entry);
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
    entry->length = entry->alloc;
    m->level--;

    BinSegHeader *hdr = (BinSegHeader *)entry->bytes;

    hdr->total = v->p->max_idx;
    hdr->kind = BINSEG_TYPE_BYTES_COLLECTION;
    hdr->ident.id = id;
    hdr->ident.idx = idx;

    if(ctx->type.state & BINSEG_REVERSED){
        r |= Buff_Add(ctx->bf, entry);
    }

    Iter it;
    Iter_Init(&it, v->p);
    while((Iter_Next(&it) & END) == 0){
        r |= BinSegCtx_SendByIdent(ctx, Iter_Get(&it), id+1, it.idx);
    };

    if((ctx->type.state & BINSEG_REVERSED) == 0){
        r |= Buff_Add(ctx->bf, entry);
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
        ptr = (i64 *)(entry->bytes+sizeof(BinSegHeader));
    }

    hdr->total = 0;
    hdr->kind = BINSEG_TYPE_NUMBER;
    hdr->ident.id = id;
    hdr->ident.idx = idx;
    *ptr = sg->val.value;

    return Buff_Add(ctx->bf, entry);
}

static status Str_ToBinSeg(BinSegCtx *ctx, void *a, i16 id, i16 idx){
    Str *s = (Str *)as(a, TYPE_STR);
    MemCh *m = ctx->bf->m;

    word sz = BinSegCtx_HeaderSize(BINSEG_TYPE_BYTES, s->length);

    m->level++;
    Str *entry = Str_Make(m, sz);
    entry->length = entry->alloc;
    m->level--;

    BinSegHeader *hdr = NULL;
    byte *ptr = NULL;
    if(ctx->type.state & BINSEG_REVERSED){
        ptr = entry->bytes;
        hdr = (BinSegHeader *)entry->bytes+s->length;
    }else{
        hdr = (BinSegHeader *)entry->bytes;
        ptr = (entry->bytes+sizeof(BinSegHeader));
    }

    hdr->total = s->length;
    hdr->kind = BINSEG_TYPE_BYTES;
    hdr->ident.id = id;
    hdr->ident.idx = idx;
    memcpy(ptr, s->bytes, s->length);

    return Buff_Add(ctx->bf, entry);
}

static status BinSegCtx_buildKind(BinSegCtx *ctx, BinSegHeader *hdr, Str *ftr){
    DebugStack_Push(ctx, ctx->type.of);
    MemCh *m = ctx->bf->m;
    
    Iter it;
    Span *shelf = Span_Get(ctx->shelves, hdr->ident.id);
    Abstract *a = NULL;
    if(hdr->kind == BINSEG_TYPE_BYTES){
        a = (Abstract *)ftr;
    }else if(hdr->kind == BINSEG_TYPE_NUMBER){
        i64 *ip = (i64 *)ftr->bytes;
        a = (Abstract *)I64_Wrapped(m, *ip);
    }else if(hdr->kind == BINSEG_TYPE_COLLECTION){
        Span *p = Span_Make(m);
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
                if((it.idx & 1) == 0){
                    key = a;
                }else{
                    Table_Set(tbl, key, a);
                }
            }
        }
        a = (Abstract *)tbl;
    }else if(hdr->kind == BINSEG_TYPE_INST){
        word *wp = (word *)ftr->bytes;
        a = (Abstract *)Inst_Make(m, *wp);
    }

    if(hdr->ident.id == 0){
        Span_Add(ctx->records, a);
        MemCh_FreeTemp(ctx->bf->m);
        ctx->bf->m++;
        ctx->shelves = Span_Make(m);
        ctx->bf->m--;
    }else {
        word parentId = hdr->ident.id-1;
        shelf = Span_Get(ctx->shelves, parentId);
        if(shelf == NULL){
            ctx->bf->m++;
            shelf = Span_Make(m);
            ctx->bf->m--;
            Span_Set(ctx->shelves, parentId, shelf);
        }
        Span_Set(shelf, hdr->ident.idx, a);
    }

    DebugStack_Pop();
    return ctx->type.state;
}

status BinSegCtx_Load(BinSegCtx *ctx){
    DebugStack_Push(ctx, ctx->type.of);
    void *args[4];
    MemCh *m = ctx->bf->m;
    ctx->type.state &= ~(SUCCESS|ERROR|NOOP|END);
    if(ctx->type.state & BINSEG_REVERSED){
        Buff_PosEnd(ctx->bf);
    }else{
        Buff_PosAbs(ctx->bf, 0);
    }

    i16 guard = 0;
    byte _hdrBytes[sizeof(BinSegHeader)];
    Str hdrS = {
        .type = {TYPE_STR, STRING_CONST},
        .length = 0,
        .alloc = sizeof(BinSegHeader),
        .bytes = _hdrBytes
    };

    while(((ctx->bf->type.state|ctx->type.state) & (SUCCESS|ERROR|NOOP|END)) == 0){
        Guard_Incr(m, &guard, BINSEG_SEG_MAX, FUNCNAME, FILENAME, LINENUMBER);

        hdrS.length = 0;
        if(ctx->type.state & BINSEG_REVERSED){
            Buff_RevGetStr(ctx->bf, &hdrS);
        }else{
            Buff_GetStr(ctx->bf, &hdrS);
        }

        if(hdrS.length != sizeof(BinSegHeader)){
            ctx->type.state |= ERROR;
            break;
        }

        BinSegHeader *hdr = (BinSegHeader*)hdrS.bytes;
        i16 sz = BinSegCtx_HeaderSize(hdr->kind, hdr->total) - sizeof(BinSegHeader);
        Str *ftr = NULL;
        if(sz > 0){
            ftr = Str_Make(m, sz);
            if(ctx->type.state & BINSEG_REVERSED){
                Buff_RevGetStr(ctx->bf, ftr);
            }else{
                Buff_GetStr(ctx->bf, ftr);
            }
        }

        BinSegCtx_buildKind(ctx, hdr, ftr);
    }

    ctx->type.state |= (ctx->bf->type.state & (ERROR|END));

    DebugStack_Pop();
    return ctx->type.state;
}

word BinSegCtx_HeaderSize(word kind, word length){
    word sz = (word)sizeof(BinSegHeader);
    if(kind == BINSEG_TYPE_INST){
        sz += sizeof(word);
    }else if(kind == BINSEG_TYPE_NUMBER){
        sz += sizeof(i64);
    }else if(kind == BINSEG_TYPE_BYTES){
        sz += length;
    }
    return sz;
}

status BinSegCtx_SendByIdent(BinSegCtx *ctx, void *_a, i16 id, i16 idx){
    if(_a == NULL){
        return NOOP;
    }
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

BinSegCtx *BinSegCtx_Make(Buff *bf, word flags){
    MemCh *m = bf->m;
    BinSegCtx *ctx = (BinSegCtx *)MemCh_AllocOf(m, sizeof(BinSegCtx), TYPE_BINSEG_CTX);
    ctx->type.of = TYPE_BINSEG_CTX;
    ctx->type.state = flags;
    bf->type.state |= BUFF_DATASYNC;
    ctx->bf = bf;
    ctx->bf->m++;
    ctx->shelves = Span_Make(m);
    ctx->records = Span_Make(m);
    ctx->bf->m--;
    return ctx;
}

status BinSeg_Init(MemCh *m){
    status r = READY;
    if(BinSegLookup == NULL){
        BinSegLookup = Lookup_Make(m, _TYPE_ZERO);
        r |= Lookup_Add(m, BinSegLookup, TYPE_STR, (void *)Str_ToBinSeg);
        r |= Lookup_Add(m, BinSegLookup, TYPE_SPAN, (void *)Span_ToBinSeg);
        r |= Lookup_Add(m, BinSegLookup, TYPE_TABLE, (void *)Table_ToBinSeg);
        r |= Lookup_Add(m, BinSegLookup, TYPE_STRVEC, (void *)StrVec_ToBinSeg);
        r |= Lookup_Add(m, BinSegLookup, TYPE_INSTANCE, (void *)Inst_ToBinSeg);
        r |= Lookup_Add(m, BinSegLookup, TYPE_WRAPPED_I64, (void *)I64_ToBinSeg);
    }
    return r;
}
