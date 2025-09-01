#include <external.h>
#include <caneka.h>

static boolean _init = FALSE;

static Str **transpFileLabels = NULL;

static i64 TranspFile_Print(Stream *sm, Abstract *a, cls type, word flags){
    TranspFile *tfile = (TranspFile*)as(a, TYPE_TRANSP_FILE);
    Abstract *args[] = {
        (Abstract *)StreamTask_Make(sm->m, NULL, (Abstract *)tfile, ToS_FlagLabels),
        (Abstract *)tfile->name,
        (Abstract *)tfile->local,
        (Abstract *)tfile->src,
        (Abstract *)tfile->dest,
        NULL
    };
    return Fmt(sm, "TranspFile<$ @ @ $ -> $>", args);
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
    return r;
}

status Transp_Init(MemCh *m){
    status r = READY;
    if(!_init){
        _init = TRUE;
        Lookup *lk = ClassLookup;
        /* cls */
        ClassDef *cls = ClassDef_Make(m);
        TranspFile tp;
        Table_Set(cls->atts, (Abstract *)Str_CstrRef(m, "name"),
            (Abstract *)I16_Wrapped(m, (void *)(&tp.name)-(void *)(&tp)));
        Table_Set(cls->atts, (Abstract *)Str_CstrRef(m, "local"),
            (Abstract *)I16_Wrapped(m, (void *)(&tp.local)-(void *)(&tp)));
        Table_Set(cls->atts, (Abstract *)Str_CstrRef(m, "src"),
            (Abstract *)I16_Wrapped(m, (void *)(&tp.name)-(void *)(&tp)));
        Table_Set(cls->atts, (Abstract *)Str_CstrRef(m, "dest"),
            (Abstract *)I16_Wrapped(m, (void *)(&tp.name)-(void *)(&tp)));
        r |= Lookup_Add(m, lk, TYPE_TRANSP_FILE, (void *)cls);

        /* ToS */
        lk = ToStreamLookup;
        r |= Lookup_Add(m, lk, TYPE_TRANSP_CTX, (void *)TranspCtx_Print);
        r |= Lookup_Add(m, lk, TYPE_TRANSP_FILE, (void *)TranspFile_Print);

        /* labels */
        lk = ToSFlagLookup;
        transpFileLabels = (Str **)Arr_Make(m, 17);
        transpFileLabels[9] = Str_CstrRef(m, "TranspFileINDEX");
        Lookup_Add(m, lk, TYPE_TRANSP_FILE, (void *)transpFileLabels);
        r |= SUCCESS;

    }
    if(r == READY){
        r |= NOOP;
    }
    return r;
}
