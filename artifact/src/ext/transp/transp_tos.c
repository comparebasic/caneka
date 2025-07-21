#include <external.h>
#include <caneka.h>

static Str **transpFileLabels = NULL;

static i64 TranspFile_Print(Stream *sm, Abstract *a, cls type, word flags){
    TranspFile *tfile = (TranspFile*)as(a, TYPE_TRANSP_FILE);
    Abstract *args[] = {
        (Abstract *)StreamTask_Make(sm->m, NULL, (Abstract *)tfile, ToS_FlagLabels),
        (Abstract *)tfile->name,
        (Abstract *)tfile->keys,
        (Abstract *)tfile->src,
        (Abstract *)tfile->dest,
        NULL
    };
    return Fmt(sm, "TranspFile<$ @/@ $ -> $>", args);
}

static i64 TranspCtx_Print(Stream *sm, Abstract *a, cls type, word flags){
    TranspCtx *tp = (TranspCtx*)as(a, TYPE_TRANSP_CTX);
    i64 total = 0;

    if((flags & (MORE|DEBUG)) == 0){
        return ToStream_NotImpl(sm, a, type, flags);
    }

    Abstract *args[] = {
        (Abstract *)StreamTask_Make(sm->m, NULL, (Abstract *)tp, ToS_FlagLabels),
        NULL,
    };

    total += Fmt(sm, "Transp<$ ", args);
    total += ToS(sm, (Abstract *)tp->sm, 0, MORE);
    if(flags & DEBUG && tp->it.p->nvalues > 0){
        total += Stream_Bytes(sm, (byte *)"stack:\n", 7);
        Iter it;
        Iter_Init(&it, tp->it.p);
        while((Iter_Prev(&it) & END) == 0){
            Abstract *args[] = {
                (Abstract *)I32_Wrapped(sm->m, it.idx),
                (Abstract *)it.value,
                NULL
            };
            char *fmt = "  $: @\n";
            if(it.idx == tp->it.idx){
                fmt = "  ^E.$: @^e.\n";
            }
            Fmt(sm, fmt,args);
        }
    }
    total += Stream_Bytes(sm, (byte *)">", 1);
    return total;
}

status Transp_InitLabels(MemCh *m, Lookup *lk){
    status r = READY;
    if(transpFileLabels == NULL){
        transpFileLabels = (Str **)Arr_Make(m, 17);
        transpFileLabels[9] = Str_CstrRef(m, "TranspFileINDEX");
        Lookup_Add(m, lk, TYPE_TRANSP_FILE, (void *)transpFileLabels);
        r |= SUCCESS;
    }
    return r;
}

status Transp_ToSInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_TRANSP_CTX, (void *)TranspCtx_Print);
    r |= Lookup_Add(m, lk, TYPE_TRANSP_FILE, (void *)TranspFile_Print);
    return r;
}
