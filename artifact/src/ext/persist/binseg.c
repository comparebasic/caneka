#include <external.h>
#include <caneka.h>

Lookup *BinSegNames = NULL;
Lookup *BinSegLookup = NULL;

Str *BinSegCtx_KindName(i8 kind){
    return Lookup_Get(BinSegNames, (i16)kind);;
}

i32 BinSegCtx_IdxCounter(BinSegCtx *ctx, Abstract *arg){
    Single *sg = (Single *)as(ctx->source, TYPE_WRAPPED_I32);
    sg->val.i++;
    return sg->val.i;
}

i64 BinSegCtx_ToStream(BinSegCtx *ctx, Abstract *a, i32 id){
    Abstract *args[2];
    BinSegFunc func = Lookup_Get(BinSegLookup, a->type.of);
    if(func == NULL){
        args[0] = (Abstract *)Type_ToStr(ctx->sm->m, a->type.of);
        args[1] = NULL;
        Error(ErrStream->m, (Abstract *)ctx, FUNCNAME, FILENAME, LINENUMBER,
            "Unable to find BinSegFunc for type $", args);
        return 0;
    }
    if(ctx->type.state & DEBUG){
        args[0] = (Abstract *)a;
        args[1] = NULL;
        Out("^p.ToStream &^0\n", args);
    }
    return func(ctx, a, id);
}

status BinSegCtx_PushLoad(BinSegCtx *ctx, BinSegHeader *hdr, Abstract *a){
    status r = READY;
    if(ctx->type.state & DEBUG){
        Abstract *args[4];
        args[0] = (Abstract *)Ptr_Wrapped(OutStream->m,
            (byte *)hdr, TYPE_BINSEG_HEADER);
        args[1] = (Abstract *)a;
        args[2] = NULL;
        Out("^y.Found Header @ -> &^0\n", args);
    }

    MemCh *m = ctx->sm->m;

    Single *id = I16_Wrapped(m, hdr->id);
    Str *key = (Str *)Table_Get(ctx->keys, (Abstract *)id);
    if(key != NULL){
        if(hdr->kind == BINSEG_TYPE_BYTES){
            r |= Table_Set(ctx->tblIt.p, (Abstract *)key, a);
        }else{
            /* add cortex ids */
            Error(ErrStream->m, (Abstract *)ctx, FUNCNAME, FILENAME, LINENUMBER,
                "Not implemented", NULL);
            return ERROR;
        }
        if(ctx->keys != NULL && ctx->tblIt.p->nvalues == ctx->keys->nvalues){
            r |= END;
        }
    } else {
        Abstract *cv = Table_Get(ctx->cortext, (Abstract *)id);
        if(cv->type.of == TYPE_TABLE){
            Table *tbl = (Table *)cv;
            if(hdr->kind == BINSEG_TYPE_KEY){
                Table_SetKey(&ctx->tblIt, a);
            }else{
                Table_SetValue(&ctx->tblIt, a);
            }
        }else if(cv->type.of == TYPE_SPAN){
            Span *p = (Span *)cv;
            Span_Add(p, a);
        }else if(cv->type.of == TYPE_STRVEC){
            StrVec *v = (StrVec *)cv;
            StrVec_Add(v, (Str *)as(a, TYPE_STR));
        }
    }

    return r;
}

status BinSegCtx_LoadStream(BinSegCtx *ctx){
    Abstract *args[4];
    MemCh *m = ctx->sm->m;
    if(ctx->type.state & BINSEG_REVERSED){
        Error(ErrStream->m, (Abstract *)ctx, FUNCNAME, FILENAME, LINENUMBER,
            "Not implemented", NULL);
        ctx->type.state |= ERROR;
        return ctx->type.state;
    }else{
        ctx->type.state &= ~(SUCCESS|ERROR|NOOP);
        Stream_Seek(ctx->sm, 0);
        while((ctx->sm->type.state & END) == 0 &&
                (ctx->type.state & (SUCCESS|ERROR|NOOP)) == 0){
            Str *s = Str_Make(m, 0);

            i32 sz = sizeof(BinSegHeader);
            Stream_FillStr(ctx->sm, s, sz);
            Stream_Move(ctx->sm, sz); 
            if(s->length == sz){
                BinSegHeader *hdr = (BinSegHeader*)s->bytes;
                if(ctx->type.state & DEBUG){
                    args[0] = (Abstract *)Ptr_Wrapped(m, hdr, TYPE_BINSEG_HEADER);
                    args[1] = NULL;
                    Out("^p.Header &^0\n", args);
                }
                Str *s = Str_Make(m, hdr->total);

                s->type.state |= STRING_COPY;
                Stream_FillStr(ctx->sm, s, hdr->total);
                Stream_Move(ctx->sm, hdr->total); 
                s->type.state &= ~STRING_COPY;

                if(BinSegCtx_PushLoad(ctx, hdr, (Abstract *)s) & END){
                    ctx->type.state |= (SUCCESS|END);
                }
            }
        }
    }

    return ctx->type.state;
}

BinSegCtx *BinSegCtx_Make(Stream *sm, BinSegIdxFunc func, Abstract *source){
    MemCh *m = sm->m;
    BinSegCtx *ctx = (BinSegCtx *)MemCh_AllocOf(m, sizeof(BinSegCtx), TYPE_BINSEG_CTX);
    ctx->type.of = TYPE_BINSEG_CTX;
    ctx->sm = sm;
    if(func == NULL && source == NULL){
        ctx->func = BinSegCtx_IdxCounter;
        ctx->source = (Abstract *)I32_Wrapped(m, -1);
    }else{
        ctx->source = source;
    }

    ctx->cortext = Table_Make(m);
    Iter_Init(&ctx->tblIt, Table_Make(m));
    return ctx;
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
        Lookup_Add(m, BinSegNames, BINSEG_TYPE_BYTES_SEG,
            (void *)Str_CstrRef(m, "BinarySeg"));
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
