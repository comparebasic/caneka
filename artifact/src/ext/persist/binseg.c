#include <external.h>
#include <caneka.h>

Lookup *BinSegNames = NULL;
Lookup *BinSegLookup = NULL;

Str *BinSeg_KindName(i8 kind){
    return Lookup_Get(BinSegNames, (i16)kind);;
}

i32 BinSeg_IdxCounter(BinSegCtx *ctx, Abstract *arg){
    return ++((Single *)ctx->source)->val.i;
}

i64 BinSeg_ToStream(BinSegCtx *ctx, Abstract *a){
    Abstract *args[2];
    BinSegFunc func = Lookup_Get(BinSegLookup, a->type.of);
    if(func == NULL){
        args[0] = (Abstract *)Type_ToStr(ctx->sm->m, a->type.of);
        args[1] = NULL;
        Error(ErrStream->m, (Abstract *)ctx, FUNCNAME, FILENAME, LINENUMBER,
            "Unable to find BinSegFunc for type $", args);
        return 0;
    }
    return func(ctx, a);
}

status BinSegCtx_PushLoad(BinSegCtx *ctx, BinSegHeader *hdr, Abstract *a){
    if(ctx->type.state & DEBUG){
        Abstract *args[4];
        args[0] = (Abstract *)Ptr_Wrapped(OutStream->m,
            (byte *)hdr, TYPE_BINSEG_HEADER);
        args[1] = (Abstract *)a;
        args[2] = NULL;
        Out("^y.Found Header @ -> &^0\n", args);
    }

    MemCh *m = ctx->sm->m;
    if(ctx->root == NULL){
        ctx->root = a;
    }else{
        Single *id = I16_Wrapped(m, hdr->id);
        Str *key = (Str *s)Table_Get(ctx->keys, (Abstract *)id);
        if(key != NULL){
            if(hdr->kind == BINSEG_TYPE_BINARY){
                Table_Set(ctx->tbl, (Abstract *)key, a);
            }else{
                /* add cortex ids */
            }
        } else {
            Abstract *cv = Table_Get(ctx->cortex, (Abstract *)id);
            if(cv->type.of == TYPE_TABLE){
                Table *tbl = (Table *)cv;
                if(hdr->kind == BINSEG_TYPE_KEY){
                    Table_SetKey(tbl, a);
                }else{
                    Table_SetValue(tbl, a);
                }
            }else if(cv->type.of == TYPE_SPAN){
                Span *p = (Span *)cv;
                Span_Add(p, a);
            }else if(cv->type.of == TYPE_STRVEC){
                StrVec *v = (StrVec *)cv;
                StrVec_Add(v, (Str *)as(a, TYPE_STR));
            }
        }

    }
    return SUCCESS;
}

status BinSegCtx_LoadStream(BinSegCtx *ctx){
    Abstract *args[4];
    MemCh *m = ctx->sm->m;
    if(ctx->type.state & BINSEG_REVERSED){
        Error(ErrStream->m, (Abstract *)ctx, FUNCNAME, FILENAME, LINENUMBER,
            "Not implemented", NULL);
        return ERROR;
    }else{
        ctx->type.state &= ~(SUCCESS|ERROR|NOOP);
        ctx->sm->dest.curs->type.state |= DEBUG;
        while((ctx->type.state & (SUCCESS|ERROR|NOOP)) == 0){
            Abstract *item = NULL;
            Stream_Seek(ctx->sm, 0);
            Str *s = Str_Make(m, 0);
            i32 sz = sizeof(BinSegHeader);
            Stream_FillStr(ctx->sm, s, sz);
            if(s->length == sz){
                BinSegHeader *hdr = (BinSegHeader*)s->bytes;
                Str *s = Str_Make(m, hdr->total);
                Stream_Move(ctx->sm, sz); 

                s->type.state |= STRING_COPY;
                Stream_FillStr(ctx->sm, s, hdr->total);
                s->type.state &= ~STRING_COPY;

                BinSegCtx_PushLoad(ctx, hdr, (Abstract *)a);
            }
            ctx->type.state |= NOOP;
        }

        return SUCCESS;
    }
    return NOOP;
}

BinSegCtx *BinSegCtx_Make(Stream *sm, BinSegIdxFunc func, Abstract *source){
    MemCh *m = sm->m;
    BinSegCtx *ctx = (BinSegCtx *)MemCh_AllocOf(m, sizeof(BinSegCtx), TYPE_BINSEG_CTX);
    ctx->type.of = TYPE_BINSEG_CTX;
    ctx->sm = sm;
    if(func == NULL && source == NULL){
        ctx->func = BinSeg_IdxCounter;
        ctx->source = (Abstract *)I32_Wrapped(m, -1);
    }

    ctx->cortext = Table_Make(m);
    ctx->source = source;
    return ctx;
}

status BinSeg_Init(MemCh *m){
    status r = READY;
    if(BinSegLookup == NULL){
        BinSegLookup = Lookup_Make(m, _TYPE_ZERO);
        r |= BinSeg_BasicInit(m, BinSegLookup);
        BinSegNames = Lookup_Make(m, _TYPE_ZERO);
        Lookup_Add(m, BinSegNames, BINSEG_TYPE_BINARY,
            (void *)Str_CstrRef(m, "Binary"));
        Lookup_Add(m, BinSegNames, BINSEG_TYPE_COLLECTION,
            (void *)Str_CstrRef(m, "Collection"));
        Lookup_Add(m, BinSegNames, BINSEG_TYPE_BINARY_SEG,
            (void *)Str_CstrRef(m, "BinarySeg"));
        Lookup_Add(m, BinSegNames, BINSEG_TYPE_DICTIONARY,
            (void *)Str_CstrRef(m, "Dictionary"));
        Lookup_Add(m, BinSegNames, BINSEG_TYPE_NODE,
            (void *)Str_CstrRef(m, "Node"));
        r |= SUCCESS;
    }

    return r;
}
