#include <external.h>
#include <caneka.h>

Str **binSegCtxLabels = NULL;
Str **fileLabels = NULL;

static i64 BinSegHeader_Print(Buff *bf, Abstract *a, cls type, word flags){
    i64 total = 0;
    BinSegHeader *hdr = (BinSegHeader *)a;
    Abstract *args[] = {
        (Abstract *)I8_Wrapped(bf->m, hdr->id),
        (Abstract *)BinSegCtx_KindName(hdr->kind),
        (Abstract *)I32_Wrapped(bf->m, hdr->total),
        NULL
    };

    total += Fmt(bf, "BinSegHdr#${$ $total}", args);
    return total;
}

static i64 BinSegCtx_Print(Buff *bf, Abstract *a, cls type, word flags){
    i64 total = 0;
    BinSegCtx *ctx = (BinSegCtx *)as(a, TYPE_BINSEG_CTX);
    Abstract *args[] = {
        (Abstract *)Type_StateVec(bf->m, ctx->type.of, ctx->type.state),
        (Abstract *)ctx->cortext,
        (Abstract *)ctx->bf,
        (Abstract *)ctx->tbl,
        NULL
    };

    total += Fmt(bf, "BinSegCtx<$ @ @ tbl:@>", args);
    return total;
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
    return r;
}
