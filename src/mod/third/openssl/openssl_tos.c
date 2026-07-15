#include <external.h>
#include <caneka.h>

static status TlsCtx_Print(Buff *bf, void *a, cls type, word flags){
    MemCh *m = bf->m;
    TlsCtx *ctx = (TlsCtx*)a;
    void *ar[] = {
        Type_StateVec(m, ctx->type.of, ctx->type.state),
        NULL
    };
    return Fmt(bf, "TlsCtx<@>", ar);
}

status Tls_ToSInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_TLS_CTX, (void *)TlsCtx_Print);
    return r;
}
