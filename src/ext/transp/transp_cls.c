#include <external.h>
#include <caneka.h>

static boolean _init = FALSE;

static Str **transpFileLabels = NULL;

static i64 TranspFile_Print(Buff *bf, void *a, cls type, word flags){
    TranspFile *tfile = (TranspFile*)as(a, TYPE_TRANSP_FILE);
    void *args[] = {
        Type_StateVec(bf->m, tfile->type.of, tfile->type.state),
        tfile->name,
        tfile->local,
        tfile->src,
        tfile->dest,
        NULL
    };
    return Fmt(bf, "TranspFile<$ @ @ $ -> $>", args);
}

static i64 TranspCtx_Print(Buff *bf, void *a, cls type, word flags){
    TranspCtx *tp = (TranspCtx*)as(a, TYPE_TRANSP_CTX);
    i64 total = 0;

    if((flags & (MORE|DEBUG)) == 0){
        return ToStream_NotImpl(bf, a, type, flags);
    }

    void *args[] = {
        Type_StateVec(bf->m, tp->type.of, tp->type.state),
        NULL,
    };

    total += Fmt(bf, "Transp<$ ", args);
    total += ToS(bf, tp->bf, 0, MORE);
    if(flags & DEBUG && tp->it.p->nvalues > 0){
        total += Buff_AddBytes(bf, (byte *)"stack:\n", 7);
        Iter it;
        Iter_Init(&it, tp->it.p);
        while((Iter_Prev(&it) & END) == 0){
            void *args[] = {
                I32_Wrapped(bf->m, it.idx),
                it.value,
                NULL
            };
            char *fmt = "  $: @\n";
            if(it.idx == tp->it.idx){
                fmt = "  ^E.$: @^e.\n";
            }
            Fmt(bf, fmt,args);
        }
    }else if(flags & MORE){
        void *args[] = {&tp->it, NULL};
        total += Fmt(bf, " @", args);
    }
    total += Buff_AddBytes(bf, (byte *)">", 1);
    return total;
}

static Map *Transp_FileMap(MemCh *m){
    TranspFile tp;
    i16 size = 4;
    RangeType *atts = (RangeType *)Bytes_Alloc(m,
        sizeof(RangeType)*(size+1), TYPE_RANGE_ARRAY);
    Str **keys = (Str **)Bytes_Alloc(m, sizeof(Str *)*(size+1), TYPE_POINTER_ARRAY);
    atts->of = TYPE_TRANSP_FILE;
    atts->range = size;
    keys[0] = Str_CstrRef(m, "name");
    (atts+1)->of = TYPE_STRVEC;
    (atts+1)->range = (i16)((void *)&tp.name - (void *)&tp);
    keys[1] = Str_CstrRef(m, "src");
    (atts+2)->of = TYPE_STRVEC;
    (atts+2)->range = (i16)((void *)&tp.src - (void *)&tp);
    keys[2] = Str_CstrRef(m, "dest");
    (atts+3)->of = TYPE_STRVEC;
    (atts+3)->range = (i16)((void *)&tp.dest - (void *)&tp);
    keys[3] = Str_CstrRef(m, "local");
    (atts+4)->of = TYPE_STRVEC;
    (atts+4)->range = (i16)((void *)&tp.local - (void *)&tp);

    return Map_Make(m, size, atts, keys);
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

        /* map */
        r |= Lookup_Add(m, MapsLookup, TYPE_TRANSP_FILE, (void *)Transp_FileMap(m));

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
