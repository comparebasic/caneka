#include <external.h>
#include <caneka.h>

static Str **nodeLabels = NULL;

static i64 TranspCtx_Print(Stream *sm, Abstract *a, cls type, word flags){
    TranspCtx *tp = (TranspCtx*)as(a, TYPE_TRANSP_CTX);

    if((flags & (MORE|DEBUG)) == 0){
        return ToStream_NotImpl(sm, a, type, flags);
    }

    Abstract *args[] = {
        (Abstract *)StreamTask_Make(sm->m, NULL, (Abstract *)tp, ToS_FlagLabels),
        (Abstract *)tp->sm,
        NULL,
    };

    return Fmt(sm, "Transp<$ @>", args);
}

status Transp_ToSInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_TRANSP_CTX, (void *)TranspCtx_Print);
    return r;
}
