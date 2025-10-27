#include <external.h>
#include <caneka.h>

Lookup *BinSegNames = NULL;
Lookup *BinSegLookup = NULL;

static status BinSegCtx_PushLoad(BinSegCtx *ctx, BinSegHeader *hdr, Str *s){
    DebugStack_Push(ctx, ctx->type.of);
    status r = READY;
    MemCh *m = ctx->bf->m;
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
                Error(m, FUNCNAME, FILENAME, LINENUMBER,
                    "StrVec child item not found", NULL);
                DebugStack_Pop();
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
                Error(m, FUNCNAME, FILENAME, LINENUMBER,
                    "Span child item not found at $ @", args);
                DebugStack_Pop();
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
                Error(m, FUNCNAME, FILENAME, LINENUMBER,
                    "Table child item(s) not found", NULL);
                DebugStack_Pop();
                return ERROR;
            }else{
                Table_Set(tbl, key, item);
            }
        }
        a = (Abstract *)tbl;
    }else if(hdr->kind == BINSEG_TYPE_NODE){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Node binseg not yet implemented ", NULL);
        DebugStack_Pop();
        return ERROR;
    }

    Single *id = I16_Wrapped(m, hdr->id);
    Table_Set(ctx->cortext, (Abstract *)id, a);
    if(ctx->keys != NULL){
        Str *key = (Str *)Table_Get(ctx->keys, (Abstract *)id);
        if(key != NULL){
            if(Table_Get(ctx->tbl, (Abstract *)key) == NULL){ 
                r |= Table_Set(ctx->tbl, (Abstract *)key, a);
            }
            if(ctx->tbl->nvalues == ctx->keys->nvalues){
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

    DebugStack_Pop();
    return r;
}

Str *BinSegCtx_KindName(i8 kind){
    return Lookup_Get(BinSegNames, (i16)kind);;
}

i16 BinSegCtx_IdxCounter(BinSegCtx *ctx, Abstract *arg){
    ctx->latestId++;
    return ctx->latestId;
 }

i64 BinSegCtx_ToBuff(BinSegCtx *ctx, BinSegHeader *hdr, Str *entry){
    DebugStack_Push(ctx, ctx->type.of);
    i64 total = 0;
    MemCh *m = ctx->bf->m;
    if(ctx->type.state & BINSEG_VISIBLE){
        entry = Str_ToHex(m, entry);
    }
    if(Buff_AddSend(ctx->bf, entry) & SUCCESS){
        total += entry->length;
    }
    if(ctx->type.state & DEBUG){
        Abstract *args[5];
        args[0] = (Abstract *)StreamTask_Make(m, NULL, (Abstract *)ctx, ToS_FlagLabels),
        args[1] = (Abstract *)Ptr_Wrapped(m, (byte *)hdr, TYPE_BINSEG_HEADER);
        args[2] = (Abstract *)I16_Wrapped(m, entry->length);
        args[3] = (Abstract *)ctx;
        args[4] = NULL;
        Out("^p.ToBuff($ &)/$ -> @^0\n", args);
    }
    DebugStack_Pop();
    return total;
}

i64 BinSegCtx_Send(BinSegCtx *ctx, Abstract *a, i16 id){
    DebugStack_Push(ctx, ctx->type.of);
    Abstract *args[2];
    BinSegFunc func = Lookup_Get(BinSegLookup, a->type.of);
    if(func == NULL){
        args[0] = (Abstract *)Type_ToStr(ctx->bf->m, a->type.of);
        args[1] = NULL;
        Error(ctx->bf->m, FUNCNAME, FILENAME, LINENUMBER,
            "Unable to find BinSegFunc for type $", args);
        DebugStack_Pop();
        return 0;
    }
    DebugStack_Pop();
    return func(ctx, a, id);
}

status BinSegCtx_Load(BinSegCtx *ctx){
    DebugStack_Push(ctx, ctx->type.of);
    Abstract *args[4];
    if(ctx->type.state & DEBUG){
        args[0] = (Abstract *)ctx;    
        args[1] = NULL;
        Out("^p.Load &\n", args);
    }
    MemCh *m = ctx->bf->m;
    ctx->type.state &= ~(SUCCESS|ERROR|NOOP);
    if(ctx->type.state & BINSEG_REVERSED){
        Buff_PosEnd(ctx->bf);
    }else{
        Buff_PosAbs(ctx->bf, 0);
    }
    while((ctx->bf->type.state & END) == 0 &&
            (ctx->type.state & (SUCCESS|ERROR|NOOP)) == 0){

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
                args[0] = (Abstract *)s;
                args[1] = NULL;
                Out("^p.Hdr &\n", args);
            }
            BinSegHeader *hdr = (BinSegHeader*)s->bytes;
            if(ctx->type.state & DEBUG){
                args[0] = (Abstract *)Ptr_Wrapped(m, hdr, TYPE_BINSEG_HEADER);
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

BinSegCtx *BinSegCtx_Make(Buff *bf, BinSegIdxFunc func, Abstract *source, word flags){
    MemCh *m = bf->m;
    BinSegCtx *ctx = (BinSegCtx *)MemCh_AllocOf(m, sizeof(BinSegCtx), TYPE_BINSEG_CTX);
    ctx->type.of = TYPE_BINSEG_CTX;
    ctx->type.state = flags;
    ctx->bf = bf;

    if(func == NULL){
        ctx->func = BinSegCtx_IdxCounter;
        ctx->latestId = -1;
    }else{
        ctx->func = func;
    }

    ctx->cortext = Table_Make(m);
    ctx->tbl = Table_Make(m);

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
