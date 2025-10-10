#include <external.h>
#include <caneka.h>

Lookup *BinSegNames = NULL;
Lookup *BinSegLookup = NULL;

static status BinSegCtx_PushLoad(BinSegCtx *ctx, BinSegHeader *hdr, Str *s){
    status r = READY;
    MemCh *m = ctx->sm->m;
    Abstract *args[3];
    
    Abstract *a = NULL;
    if(hdr->kind == BINSEG_TYPE_BYTES){
        a = (Abstract *)s;
    }else if(hdr->kind == BINSEG_TYPE_NUMBER){
        i32 *ip = (i32 *)s->bytes;
        a = (Abstract *)I32_Wrapped(m, *ip);
    }else if(hdr->kind == BINSEG_TYPE_BYTES_COLLECTION){
        StrVec *v = StrVec_Make(m);
        i16 *idp = (i16 *)s->bytes;
        Single *sg = I16_Wrapped(m, 0);
        for(i32 i = 0; i < hdr->total; i++){
            sg->val.w = idp[i];
            Abstract *item = Table_Get(ctx->cortext, (Abstract *)sg);
            if(item == NULL){
                
                Error(ErrStream->m, (Abstract *)ctx, FUNCNAME, FILENAME, LINENUMBER,
                    "StrVec child item not found", NULL);
                return ERROR;
            }else{
                StrVec_Add(v, (Str *)as(item, TYPE_STR));
            }
        }
        a = (Abstract *)v;
    }else if(hdr->kind == BINSEG_TYPE_COLLECTION){
        Span *p = Span_Make(m);
        i16 *idp = (i16 *)s->bytes;
        Single *sg = I16_Wrapped(m, 0);
        for(i32 i = 0; i < hdr->total; i++){
            sg->val.w = idp[i];
            Abstract *item = Table_Get(ctx->cortext, (Abstract *)sg);
            if(item == NULL){
                args[0] = (Abstract *)sg;
                args[1] = (Abstract *)ctx->cortext;
                args[2] = NULL;
                Error(ErrStream->m, (Abstract *)ctx, FUNCNAME, FILENAME, LINENUMBER,
                    "Span child item not found at $ @", args);
                return ERROR;
            }else{
                Span_Add(p, item);
            }
        }
        a = (Abstract *)p;
    }else if(hdr->kind == BINSEG_TYPE_DICTIONARY){
        Table *tbl = Table_Make(m);
        i16 *idp = (i16 *)s->bytes;
        Single *keySg = I16_Wrapped(m, 0);
        Single *sg = I16_Wrapped(m, 0);
        for(i32 i = 0; i < hdr->total; i++){
            keySg->val.w = idp[i*2];
            sg->val.w = idp[i*2+1];
            Abstract *key = Table_Get(ctx->cortext, (Abstract *)keySg);
            Abstract *item = Table_Get(ctx->cortext, (Abstract *)sg);
            if(key == NULL || item == NULL){
                Error(ErrStream->m, (Abstract *)ctx, FUNCNAME, FILENAME, LINENUMBER,
                    "Table child item(s) not found", NULL);
                return ERROR;
            }else{
                Table_Set(tbl, key, item);
            }
        }
        a = (Abstract *)tbl;
    }else if(hdr->kind == BINSEG_TYPE_NODE){
        Error(ErrStream->m, (Abstract *)ctx, FUNCNAME, FILENAME, LINENUMBER,
            "Node binseg not yet implemented ", NULL);
        return ERROR;
    }

    Single *id = I16_Wrapped(m, hdr->id);
    Table_Set(ctx->cortext, (Abstract *)id, a);
    if(ctx->keys != NULL){
        Str *key = (Str *)Table_Get(ctx->keys, (Abstract *)id);
        if(key != NULL){
            r |= Table_Set(ctx->tblIt.p, (Abstract *)key, a);
            if(ctx->tblIt.p->nvalues == ctx->keys->nvalues){
                r |= END;
            }
        }
    }

    if(ctx->type.state & DEBUG){
        Abstract *args[4];
        args[0] = (Abstract *)Ptr_Wrapped(OutStream->m,
            (byte *)hdr, TYPE_BINSEG_HEADER);
        args[1] = a;
        args[2] = NULL;
        Out("^y.Found Header @ -> &^0\n", args);
    }

    return r;
}



Str *BinSegCtx_KindName(i8 kind){
    return Lookup_Get(BinSegNames, (i16)kind);;
}

i16 BinSegCtx_IdxCounter(BinSegCtx *ctx, Abstract *arg){
    Single *sg = (Single *)as(ctx->source, TYPE_WRAPPED_I16);
    sg->val.w++;
    return sg->val.w;
}

i64 BinSegCtx_ToStream(BinSegCtx *ctx, Abstract *a, i16 id){
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

i64 BinSegCtx_FooterToStream(BinSegCtx *ctx, Span *ids){
    i64 total = 0;
    MemCh *m = ctx->sm->m;

    Iter it;
    Iter_Init(&it, ids);
    while((Iter_Next(&it) & END) == 0){
        Single *sg = (Single *)Iter_Get(&it);
        Str *idStr = Str_Ref(m, (byte *)&sg->val.w, sizeof(i16), sizeof(i16), ZERO);
        total += Stream_Bytes(ctx->sm, (byte *)idStr->bytes, idStr->length); 
    }

    return total;
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

            i16 sz = sizeof(BinSegHeader);
            Stream_FillStr(ctx->sm, s, sz);
            Stream_Move(ctx->sm, sz); 
            if(s->length == sz){
                BinSegHeader *hdr = (BinSegHeader*)s->bytes;
                if(ctx->type.state & DEBUG){
                    args[0] = (Abstract *)Ptr_Wrapped(m, hdr, TYPE_BINSEG_HEADER);
                    args[1] = NULL;
                    Out("^p.Header &^0\n", args);
                }

                if(hdr->kind == BINSEG_TYPE_BYTES){
                    sz = hdr->total;
                }else if(hdr->kind == BINSEG_TYPE_NUMBER){
                    sz = sizeof(i32);
                }else if(hdr->kind == BINSEG_TYPE_NODE){
                    sz = sizeof(i16)*(hdr->total+2);
                }else if(hdr->kind == BINSEG_TYPE_DICTIONARY){
                    sz = sizeof(i16)*(hdr->total*2);
                }else{
                    /* Collection/BytesCollection */
                    sz = sizeof(i16)*hdr->total;
                }

                Str *s = Str_Make(m, sz);
                s->type.state |= STRING_COPY;
                Stream_FillStr(ctx->sm, s, sz);
                Stream_Move(ctx->sm, sz); 
                s->type.state &= ~STRING_COPY;

                if(BinSegCtx_PushLoad(ctx, hdr, s) & END){
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
        ctx->source = (Abstract *)I16_Wrapped(m, -1);
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
