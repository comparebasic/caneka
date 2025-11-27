#include <external.h>
#include <caneka.h>

Lookup *BinSegLookup = NULL;

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

status BinSegCtx_SendEntry(BinSegCtx *ctx, BinSegHeader *hdr, Str *entry){
    if(ctx->type.state & BINSEG_VISIBLE){
        entry = Str_ToHex(m, entry);
    }
    if(entry != NULL){
        return Buff_Add(ctx->bf, entry);
    }
    return ZERO;
}

status BinSegCtx_Send(BinSegCtx *ctx, void *_a, i16 id){
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
    return func(ctx, a, id);
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
    }
    return r;
}
