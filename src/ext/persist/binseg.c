#include <external.h>
#include <caneka.h>

Lookup *BinSegNames = NULL;
Lookup *BinSegLookup = NULL;

static status BinSegCtx_PushLoad(BinSegCtx *ctx, BinSegHeader *hdr, Str *s){
    DebugStack_Push(ctx, ctx->type.of);
    MemCh *m = ctx->bf->m;
    void *args[3];
    
    Abstract *a = NULL;
    Single *id = I16_Wrapped(m, hdr->id);
    if(hdr->kind == BINSEG_TYPE_BYTES){
        a = (Abstract *)s;
        Table_Set(ctx->cortext, id, a);
    }else if(hdr->kind == BINSEG_TYPE_NUMBER){
        i32 *ip = (i32 *)s->bytes;
        a = (Abstract *)I32_Wrapped(m, *ip);
        Table_Set(ctx->cortext, id, a);
    }else if(hdr->kind == BINSEG_TYPE_BYTES_COLLECTION){
        StrVec *v = StrVec_Make(m);
        a = (Abstract *)v;
        Table_Set(ctx->cortext, id, a);
        i16 *idp = (i16 *)s->bytes;
        Single *sg = I16_Wrapped(m, 0);
        for(i32 i = 0; i < hdr->total; i++){
            sg->val.w = idp[i];
            Abstract *item = Table_Get(ctx->cortext, sg);
            if(item == NULL){
                Table_Set(ctx->unresolved, sg, a);
            }else{
                StrVec_Add(v, (Str *)as(item, TYPE_STR));
            }
        }
    }else if(hdr->kind == BINSEG_TYPE_COLLECTION){
        Span *p = Span_Make(m);
        a = (Abstract *)p;
        Table_Set(ctx->cortext, id, a);
        i16 *idp = (i16 *)s->bytes;
        Single *sg = I16_Wrapped(m, 0);
        for(i32 i = 0; i < hdr->total; i++){
            sg->val.w = idp[i];
            Abstract *item = Table_Get(ctx->cortext, sg);
            if(item == NULL){
                args[0] = sg;
                args[1] = ctx->cortext;
                args[2] = NULL;
                Error(m, FUNCNAME, FILENAME, LINENUMBER,
                    "Span child item not found at $ @", args);
                DebugStack_Pop();
                return ERROR;
            }else{
                Span_Add(p, item);
            }
        }
    }else if(hdr->kind == BINSEG_TYPE_DICTIONARY){
        Table *tbl = Table_Make(m);
        a = (Abstract *)tbl;
        Table_Set(ctx->cortext, id, a);
        i16 *idp = (i16 *)s->bytes;
        Single *keySg = I16_Wrapped(m, 0);
        Single *sg = I16_Wrapped(m, 0);
        for(i32 i = 0; i < hdr->total; i++){
            keySg->val.w = idp[i*2];
            sg->val.w = idp[i*2+1];
            Abstract *key = Table_Get(ctx->cortext, keySg);
            Abstract *item = Table_Get(ctx->cortext, sg);
            if(key == NULL || item == NULL){
                Error(m, FUNCNAME, FILENAME, LINENUMBER,
                    "Table child item(s) not found", NULL);
                DebugStack_Pop();
                return ERROR;
            }else{
                Table_Set(tbl, key, item);
            }
        }
    }else if(hdr->kind == BINSEG_TYPE_NODE){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Node binseg not yet implemented ", NULL);
        DebugStack_Pop();
        return ERROR;
    }

    if(ctx->dest != NULL){
        if(ctx->type.state & BINSEG_SEEL){
            Span *ord = (Span *)ctx->target;
            Table *tbl = NULL;
            if(ctx->type.state & BINSEG_RECORDS){
                tbl = Iter_GetByIdx((Iter *)ctx->dest, ((Iter *)ctx->dest)->p->max_idx);
            }else{
                tbl = (Table *)ctx->dest;
            }
            if(id->val.w == 0){
                Iter_Add((Iter *)ctx->dest, a);
            }
            Hashed *h = (Str *)Span_Get(ord, id->val.w);
            if(h != NULL){
                if(Table_Get(tbl, h->key) == NULL){ 
                    Table_Set(ctx->tbl, h->key, a);
                }

                if(ctx->tbl->nvalues == ord->nvalues){
                    if(ctx->type.state & BINSEG_RECORDS){
                        Iter_Add((Iter *)ctx->dest, Table_Make(m));
                    }else{
                        ctx->type.state |= END;
                    }
                }
            }
        }
    }else{
        ctx->type.state |= END;
    }

    if(ctx->type.state & DEBUG){
        void *args[4];
        args[0] = Ptr_Wrapped(OutStream->m,
            (byte *)hdr, TYPE_BINSEG_HEADER);
        args[1] = a;
        args[2] = NULL;
        Out("^y.Found Header @ -> &^0\n", args);
    }

    DebugStack_Pop();
    return ctx->type.state;
}

status BinSegCtx_Load(BinSegCtx *ctx){
    DebugStack_Push(ctx, ctx->type.of);
    void *args[4];
    if(ctx->type.state & DEBUG){
        args[0] = ctx;
        args[1] = ctx->bf;
        args[2] = NULL;
        Out("^p.Load & &\n", args);
    }
    MemCh *m = ctx->bf->m;
    ctx->type.state &= ~(SUCCESS|ERROR|NOOP);
    if(ctx->type.state & BINSEG_REVERSED){
        Buff_PosEnd(ctx->bf);
        if(ctx->type.state & DEBUG){
            args[0] = ctx->bf;
            args[1] = NULL;
            Out("^p.Positioning @\n", args);
        }
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
            if(ctx->type.state & DEBUG){
                args[0] = s;
                args[1] = NULL;
                Out("^p.Hdr &\n", args);
            }
            BinSegHeader *hdr = (BinSegHeader*)s->bytes;
            if(ctx->type.state & DEBUG){
                args[0] = Ptr_Wrapped(m, hdr, TYPE_BINSEG_HEADER);
                args[1] = NULL;
                Out("^p.Header &^0\n", args);
            }
            
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

            if(BinSegCtx_PushLoad(ctx, hdr, ftr) & END){
                ctx->type.state |= (SUCCESS|END);
            }
        }
    }

    DebugStack_Pop();
    return ctx->type.state;
}

Str *BinSegCtx_KindName(i8 kind){
    return Lookup_Get(BinSegNames, (i16)kind);;
}

i16 BinSegCtx_IdxCounter(BinSegCtx *ctx, void *arg){
    ctx->latestId++;
    return ctx->latestId;
 }

status BinSegCtx_ToBuff(BinSegCtx *ctx, BinSegHeader *hdr, Str *entry){
    DebugStack_Push(ctx, ctx->type.of);
    MemCh *m = ctx->bf->m;
    if(ctx->type.state & BINSEG_VISIBLE){
        entry = Str_ToHex(m, entry);
    }
    m->level--;
    status r = Buff_Add(ctx->bf, entry);
    m->level++;
    if(ctx->type.state & DEBUG){
        void *args[5];
        args[0] = Type_StateVec(m, ctx->type.of, ctx->type.state),
        args[1] = Ptr_Wrapped(m, (byte *)hdr, TYPE_BINSEG_HEADER);
        args[2] = I16_Wrapped(m, entry->length);
        args[3] = ctx;
        args[4] = NULL;
        Out("^p.ToBuff($ &)/$ -> @^0\n", args);
    }
    DebugStack_Pop();
    return r;
}

status BinSegCtx_Send(BinSegCtx *ctx, void *_a, i16 id){
    status r = READY;
    Abstract *a = (Abstract *)_a;
    DebugStack_Push(ctx, ctx->type.of);
    void *args[2];
    BinSegFunc func = Lookup_Get(BinSegLookup, a->type.of);
    if(func == NULL){
        args[0] = Type_ToStr(ctx->bf->m, a->type.of);
        args[1] = NULL;
        Error(ctx->bf->m, FUNCNAME, FILENAME, LINENUMBER,
            "Unable to find BinSegFunc for type $", args);
        DebugStack_Pop();
        return 0;
    }
    DebugStack_Pop();
    r = func(ctx, a, id);
    MemCh_FreeTemp(ctx->bf->m);
    return r;
}

BinSegCtx *BinSegCtx_Make(Buff *bf, BinSegIdxFunc func, void *source, word flags){
    MemCh *m = bf->m;
    BinSegCtx *ctx = (BinSegCtx *)MemCh_AllocOf(m, sizeof(BinSegCtx), TYPE_BINSEG_CTX);
    ctx->type.of = TYPE_BINSEG_CTX;
    ctx->type.state = flags;
    bf->type.state |= BUFF_DATASYNC;
    ctx->bf = bf;

    if(func == NULL){
        ctx->func = BinSegCtx_IdxCounter;
        ctx->latestId = -1;
    }else{
        ctx->func = func;
    }

    ctx->cortext = Table_Make(m);
    ctx->unresolved = Table_Make(m);
    ctx->dest = (Abstract *)Iter_Make(m, Span_Make(m));
    ctx->target = (Abstract *)Table_Ordered(seel);

    return ZERO;
}

status BinSeg_Init(MemCh *m){
    status r = READY;
    if(BinSegLookup == NULL){
        BinSegLookup = Lookup_Make(m, _TYPE_ZERO);
        r |= BinSeg_BasicInit(m, BinSegLookup);
        BinSegNames = Lookup_Make(m, _TYPE_ZERO);
        Lookup_Add(m, BinSegNames, BINSEG_TYPE_BYTES,
            (void *)Str_CstrRef(m, "Binary"));
        Lookup_Add(m, BinSegNames, BINSEG_TYPE_COLLECTION,
            (void *)Str_CstrRef(m, "Collection"));
        Lookup_Add(m, BinSegNames, BINSEG_TYPE_BYTES_COLLECTION,
            (void *)Str_CstrRef(m, "BinarySegCollection"));
        Lookup_Add(m, BinSegNames, BINSEG_TYPE_DICTIONARY,
            (void *)Str_CstrRef(m, "Dictionary"));
        Lookup_Add(m, BinSegNames, BINSEG_TYPE_NODE,
            (void *)Str_CstrRef(m, "Node"));
        Lookup_Add(m, BinSegNames, BINSEG_TYPE_NUMBER,
            (void *)Str_CstrRef(m, "Number"));
        r |= SUCCESS;
    }

    return r;
}
