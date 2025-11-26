#include <external.h>
#include <caneka.h>

Str **binSegCtxLabels = NULL;
Str **fileLabels = NULL;
Lookup *BinSegNames = NULL;

Str *BinSegCtx_KindName(i8 kind){
    return Lookup_Get(BinSegNames, (i16)kind);;
}

static status BinSegHeader_Print(Buff *bf, void *a, cls type, word flags){
    i64 total = 0;
    BinSegHeader *hdr = (BinSegHeader *)a;
    void *args[] = {
        I8_Wrapped(bf->m, hdr->id),
        BinSegCtx_KindName(hdr->kind),
        I32_Wrapped(bf->m, hdr->total),
        NULL
    };

    total += Fmt(bf, "BinSegHdr#${$ $total}", args);
    return total;
}

static status BinSegCtx_Print(Buff *bf, void *a, cls type, word flags){
    BinSegCtx *ctx = (BinSegCtx *)as(a, TYPE_BINSEG_CTX);
    void *args[] = {
        Type_StateVec(bf->m, ctx->type.of, ctx->type.state),
        ctx->cortext,
        ctx->bf,
        ctx->tbl,
        NULL
    };

    return Fmt(bf, "BinSegCtx<$ @ @ tbl:@>", args);
}

static status persistInitLabels(MemCh *m, Lookup *lk){
    status r = READY;
    if(binSegCtxLabels == NULL){
        binSegCtxLabels = (Str **)Arr_Make(m, 17);
        binSegCtxLabels[9] = Str_CstrRef(m, "REVERSED");
        binSegCtxLabels[10] = Str_CstrRef(m, "VISIBLE");
        Lookup_Add(m, lk, TYPE_BINSEG_CTX, (void *)binSegCtxLabels);
        r |= SUCCESS;
    }
    if(fileLabels == NULL){
        fileLabels = (Str **)Arr_Make(m, 17);
        fileLabels[9] = Str_CstrRef(m, "STRVEC");
        fileLabels[10] = Str_CstrRef(m, "APPEND");
        fileLabels[11] = Str_CstrRef(m, "FROM_FD");
        fileLabels[12] = Str_CstrRef(m, "TO_FD");
        fileLabels[13] = Str_CstrRef(m, "ASYNC");
        fileLabels[14] = Str_CstrRef(m, "SOCKET");
        fileLabels[15] = Str_CstrRef(m, "BUFFER");
        fileLabels[16] = Str_CstrRef(m, "CREATE");
        Lookup_Add(m, lk, TYPE_FILE, (void *)fileLabels);
        r |= SUCCESS;
    }
    return r;
}

status Persist_ToSInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_BINSEG_CTX, (void *)BinSegCtx_Print);
    r |= Lookup_Add(m, lk, TYPE_BINSEG_HEADER, (void *)BinSegHeader_Print);
    r |= persistInitLabels(m, ToSFlagLookup);

    if(BinSegLookup == NULL){
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
