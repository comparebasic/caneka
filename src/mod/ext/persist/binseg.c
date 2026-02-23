#include <external.h>
#include <caneka.h>

Lookup *BinSegLookup = NULL;
Table *BinSegActionNames;

word BinSeg_ActionByStr(void *a){
    Single *sg = Table_Get(BinSegActionNames, a);
    if(sg == NULL){
        return ZERO; 
    }else{
        return sg->val.w;
    }
}

static status Inst_ToBinSeg(BinSegCtx *ctx, void *a, i16 id, i16 idx){
    MemCh *m = ctx->m;
    status r = READY;
    Inst *inst = asInst(m, a);
    Span *ord = Seel_OrdSeel(m, inst->type.of);

    word sz = BinSegCtx_HeaderSize(BINSEG_TYPE_INST, 0);

    m->level++;
    Str *entry = Str_Make(m, sz);
    entry->length = entry->alloc;
    m->level--;

    BinSegHeader *hdr = NULL;
    word *ptr = NULL;
    if(ctx->type.state & BINSEG_REVERSED){
        ptr = (word *)entry->bytes;
        hdr = (BinSegHeader *)(entry->bytes+sizeof(word));
    }else{
        hdr = (BinSegHeader *)entry->bytes;
        ptr = (word *)(entry->bytes+sizeof(BinSegHeader));
    }

    hdr->total = ord->nvalues;
    hdr->kind = BINSEG_TYPE_INST;
    hdr->ident.id = id;
    hdr->ident.idx = idx;
    *ptr = inst->type.of;

    if(ctx->type.state & BINSEG_REVERSED){
        r |= Buff_Add(ctx->add, entry);
    }

    Iter it;
    Iter_Init(&it, inst);
    while((Iter_Next(&it) & END) == 0){
        r |= BinSegCtx_SendByIdent(ctx, Iter_Get(&it), id+1, it.idx);
    };

    if((ctx->type.state & BINSEG_REVERSED) == 0){
        r |= Buff_Add(ctx->add, entry);
    }

    return r;
}

static status Table_ToBinSeg(BinSegCtx *ctx, void *a, i16 id, i16 idx){
    MemCh *m = ctx->m;
    Table *tbl = (Span *)Ifc(m, a, TYPE_TABLE);
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
        r |= Buff_Add(ctx->add, entry);
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
        r |= Buff_Add(ctx->add, entry);
    }

    return r;
}

static status Span_ToBinSeg(BinSegCtx *ctx, void *a, i16 id, i16 idx){
    MemCh *m = ctx->m;
    m->level++;
    Span *p = (Span *)Ifc(m, a, TYPE_SPAN);
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
        r |= Buff_Add(ctx->add, entry);
    }

    Iter it;
    Iter_Init(&it, p);
    while((Iter_Next(&it) & END) == 0){
        r |= BinSegCtx_SendByIdent(ctx, Iter_Get(&it), id+1, it.idx);
    };

    if((ctx->type.state & BINSEG_REVERSED) == 0){
        r |= Buff_Add(ctx->add, entry);
    }

    return r;
}

static status StrVec_ToBinSeg(BinSegCtx *ctx, void *a, i16 id, i16 idx){
    StrVec *v = (StrVec *)Ifc(m, a, TYPE_STRVEC);
    MemCh *m = ctx->m;
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
        r |= Buff_Add(ctx->add, entry);
    }

    Iter it;
    Iter_Init(&it, v->p);
    while((Iter_Next(&it) & END) == 0){
        r |= BinSegCtx_SendByIdent(ctx, Iter_Get(&it), id+1, it.idx);
    };

    if((ctx->type.state & BINSEG_REVERSED) == 0){
        r |= Buff_Add(ctx->add, entry);
    }

    return r;
}

static status I64_ToBinSeg(BinSegCtx *ctx, void *_a, i16 id, i16 idx){
    MemCh *m = ctx->m;
    Abstract *a = (Abstract *)_a;
    if(a->type.of <= _TYPE_WRAPPED_START || a->type.of >= _TYPE_WRAPPED_END){
       return ERROR;
    }

    Single *sg = (Single *)a;

    word sz = BinSegCtx_HeaderSize(BINSEG_TYPE_NUMBER, 0);

    m->level++;
    Str *entry = Str_Make(m, sz);
    m->level--;

    BinSegHeader *hdr = NULL;
    i64 *ptr = NULL;
    if(ctx->type.state & BINSEG_REVERSED){
        ptr = (i64 *)entry->bytes;
        hdr = (BinSegHeader *)(entry->bytes+sizeof(i64));
    }else{
        hdr = (BinSegHeader *)entry->bytes;
        ptr = (i64 *)(entry->bytes+sizeof(BinSegHeader));
    }

    hdr->total = 0;
    hdr->kind = BINSEG_TYPE_NUMBER;
    hdr->ident.id = id;
    hdr->ident.idx = idx;
    *ptr = Single_ToUtil(sg);

    return Buff_Add(ctx->add, entry);
}

static status Str_ToBinSeg(BinSegCtx *ctx, void *a, i16 id, i16 idx){
    Str *s = (Str *)Ifc(m, a, TYPE_STR);
    MemCh *m = ctx->m;

    word sz = BinSegCtx_HeaderSize(BINSEG_TYPE_BYTES, s->length);

    m->level++;
    Str *entry = Str_Make(m, sz);
    entry->length = entry->alloc;
    m->level--;

    BinSegHeader *hdr = NULL;
    byte *ptr = NULL;
    if(ctx->type.state & BINSEG_REVERSED){
        ptr = entry->bytes;
        hdr = (BinSegHeader *)(entry->bytes+s->length);
    }else{
        hdr = (BinSegHeader *)entry->bytes;
        ptr = (entry->bytes+sizeof(BinSegHeader));
    }

    hdr->total = s->length;
    hdr->kind = BINSEG_TYPE_BYTES;
    hdr->ident.id = id;
    hdr->ident.idx = idx;
    memcpy(ptr, s->bytes, s->length);

    return Buff_Add(ctx->add, entry);
}

static status BinSegCtx_buildKind(BinSegCtx *ctx, BinSegHeader *hdr, Str *ftr){
    DebugStack_Push(ctx, ctx->type.of);
    MemCh *m = ctx->m;
    
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
        Inst *inst = Inst_Make(m, *wp);
        if(shelf != NULL){
            Iter_Init(&it, shelf);
            Abstract *key = NULL;
            while((Iter_Next(&it) & END) == 0){
                Abstract *a = Iter_Get(&it);
                if(a != NULL){
                    Span_Set(inst, it.idx, a);
                }
            }
        }
        a = (Abstract *)inst;
    }

    if(hdr->ident.id == 0){
        Span_Add(ctx->records, a);
        MemCh_FreeTemp(ctx->m);
        ctx->m++;
        ctx->shelves = Span_Make(m);
        ctx->m--;
    }else {
        word parentId = hdr->ident.id-1;
        shelf = Span_Get(ctx->shelves, parentId);
        if(shelf == NULL){
            ctx->m++;
            shelf = Span_Make(m);
            ctx->m--;
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
    MemCh *m = ctx->read->m;
    ctx->type.state &= ~(SUCCESS|ERROR|NOOP|END);
    if(ctx->read == NULL){
        return ERROR;
    }

    if(ctx->type.state & BINSEG_REVERSED){
        Buff_PosEnd(ctx->read);
    }else{
        Buff_PosAbs(ctx->read, 0);
    }

    i16 guard = 0;
    byte _hdrBytes[sizeof(BinSegHeader)];
    Str hdrS = {
        .type = {TYPE_STR, STRING_CONST},
        .length = 0,
        .alloc = sizeof(BinSegHeader),
        .bytes = _hdrBytes
    };

    while((((ctx->read->type.state&(ERROR|END))|ctx->type.state) &
            (SUCCESS|ERROR|NOOP|END)) == 0){
        Guard_Incr(m, &guard, BINSEG_SEG_MAX, FUNCNAME, FILENAME, LINENUMBER);


        hdrS.length = 0;
        if(ctx->type.state & BINSEG_REVERSED){
            Buff_RevGetStr(ctx->read, &hdrS);
        }else{
            Buff_GetStr(ctx->read, &hdrS);
        }

        if(ctx->read->type.state & END){
            ctx->read->type.state &= ~ERROR;
            break;
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
                Buff_RevGetStr(ctx->read, ftr);
            }else{
                Buff_GetStr(ctx->read, ftr);
            }
        }

        BinSegCtx_buildKind(ctx, hdr, ftr);
    }

    ctx->type.state |= (ctx->read->type.state & (ERROR|END));

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
    BinSegFunc func = NULL;
    if(a->type.of & TYPE_INSTANCE){
        func = Inst_ToBinSeg;
    }else{
        func = Lookup_Get(BinSegLookup, a->type.of);
        if(func == NULL){
            void *args[] = {
                Type_ToStr(ctx->add->m, a->type.of),
                NULL,
            };
            Error(ctx->add->m, FUNCNAME, FILENAME, LINENUMBER,
                "Unable to find BinSegFunc for type $", args);
            return 0;
        }
    }
    return func(ctx, a, id, idx);
}

status BinSegCtx_Open(BinSegCtx *ctx, Str *path){
    MemCh *m = ctx->m;
    status r = READY;
    if(ctx->type.state & BINSEG_READ){
        ctx->read = Buff_Make(m, BUFF_UNBUFFERED);
        r |= File_Open(ctx->read, path, O_RDONLY);
    }
    if(ctx->type.state & BINSEG_ADD){
        ctx->add = Buff_Make(m, BUFF_UNBUFFERED|BUFF_DATASYNC);
        r |= File_Open(ctx->add, path, O_WRONLY|O_APPEND);
    }
    if(ctx->type.state & BINSEG_MODIFY){
        ctx->modify = Buff_Make(m, BUFF_UNBUFFERED|BUFF_DATASYNC);
        r |= File_Open(ctx->modify, path, O_RDWR);
    }
    ctx->type.state |= r;
    return ctx->type.state;
}

status BinSegCtx_Close(BinSegCtx *ctx){
    MemCh *m = ctx->m;
    status r = READY;
    if(ctx->type.state & BINSEG_READ){
        r |= File_Close(ctx->read);
    }
    if(ctx->type.state & BINSEG_ADD){
        r |= File_Close(ctx->add);
    }
    if(ctx->type.state & BINSEG_MODIFY){
        r |= File_Close(ctx->modify);
    }
    ctx->type.state |= r;
    return ctx->type.state;
}

BinSegCtx *BinSegCtx_Make(MemCh *m, word flags){
    BinSegCtx *ctx = (BinSegCtx *)MemCh_AllocOf(m, sizeof(BinSegCtx), TYPE_BINSEG_CTX);
    ctx->type.of = TYPE_BINSEG_CTX;
    ctx->type.state = flags;
    ctx->m = m;
    ctx->shelves = Span_Make(m);
    ctx->records = Span_Make(m);
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
        r |= Lookup_Add(m, BinSegLookup, TYPE_WRAPPED_UTIL, (void *)I64_ToBinSeg);
        r |= Lookup_Add(m, BinSegLookup, TYPE_WRAPPED_I64, (void *)I64_ToBinSeg);
        r |= Lookup_Add(m, BinSegLookup, TYPE_WRAPPED_U32, (void *)I64_ToBinSeg);
        r |= Lookup_Add(m, BinSegLookup, TYPE_WRAPPED_I32, (void *)I64_ToBinSeg);
        r |= Lookup_Add(m, BinSegLookup, TYPE_WRAPPED_WORD, (void *)I64_ToBinSeg);
        r |= Lookup_Add(m, BinSegLookup, TYPE_WRAPPED_I16, (void *)I64_ToBinSeg);
    }
    if(BinSegActionNames == NULL){
        BinSegActionNames = Table_Make(m);
        Table_Set(BinSegActionNames, S(m, "add"), I16_Wrapped(m, BINSEG_ADD));
        Table_Set(BinSegActionNames, S(m, "read"), I16_Wrapped(m, BINSEG_READ));
        Table_Set(BinSegActionNames, S(m, "modify"), I16_Wrapped(m, BINSEG_MODIFY));
    }
    return r;
}
