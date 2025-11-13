#include <external.h>
#include <caneka.h>

static boolean _init = FALSE;

static i64 Hashed_Print(Buff *bf, void *a, cls type, word flags){
    Hashed *h = (Hashed *)as(a, TYPE_HASHED);
    if(flags & DEBUG){
        Single *wid = I64_Wrapped(bf->m, h->id);
        wid->type.state |= FMT_TYPE_BITS;
        Single *val = Ptr_Wrapped(bf->m, h->value, 0);
        word typeOf = TYPE_UNKNOWN;
        if(h->value != NULL){
            typeOf = ((Abstract *)h->value)->type.of;
        }
        void *args[] = {
            I32_Wrapped(bf->m, h->orderIdx), 
            I32_Wrapped(bf->m, h->idx), 
            wid, 
            h->key, 
            val, 
            Type_ToStr(bf->m, typeOf),
            NULL
        };
        return Fmt(bf, "H<$,$ $/@ -> $/$>", args);
    }else if(flags & MORE){
        void *args[] = {
            I32_Wrapped(bf->m, h->idx), 
            h->key, 
            h->value, 
            NULL
        };
        return Fmt(bf, "H<$ @ -> @>", args);
    }else{
        return ToStream_NotImpl(bf, a, type, flags);
    }
}

static boolean FetchTarget_Exact(FetchTarget  *a, FetchTarget *b){
    if((a->type.state & UPPER_FLAGS) != (b->type.state & UPPER_FLAGS) ||
            a->idx != b->idx || a->offsetType != b->offsetType || 
            !Exact(a->key, b->key)){
        return FALSE;
    }
    return TRUE;
}

static boolean Fetcher_Exact(Fetcher *a, Fetcher *b){
    if((a->type.state & UPPER_FLAGS) != (b->type.state & UPPER_FLAGS)){
        return FALSE;
    }
    return Exact(a->val.targets, b->val.targets);
}

static status Fetcher_Print(Buff *bf, void *a, cls type, word flags){
    Fetcher *fch = (Fetcher *)as(a, TYPE_FETCHER);
    void *args[] = {
        Type_StateVec(bf->m, fch->type.of, fch->type.state),
        NULL,
    };
    Fmt(bf, "Fetcher<@ ", args);
    void *args1[] = {
        fch->val.targets,
        NULL,
    };
    return Fmt(bf, "@>", args1);
}

static status FetchTargetFunc_Print(Buff *bf, void *a, cls type, word flags){
    return Fmt(bf, "FTFunc<>", NULL);
}

static status FetchTarget_Print(Buff *bf, void *a, cls type, word flags){
    FetchTarget *tg = (FetchTarget *)as(a, TYPE_FETCH_TARGET);
    void *args[] = {
        Type_StateVec(bf->m, tg->type.of, tg->type.state),
        NULL,
    };
    Fmt(bf, "FT<@", args);
    if(tg->type.of != ZERO){
        void *args[] = {
            Type_ToStr(bf->m, tg->type.of),
            NULL
        };
        Fmt(bf, " $ ", args);

    }
    if(tg->key != NULL){
        void *args[] = {
            tg->key,
            NULL
        };
        Fmt(bf, " key=@", args);
    }
    if(tg->type.state & FETCH_TARGET_IDX || tg->idx != -1){
        void *args[] = {
            I32_Wrapped(bf->m, tg->idx),
            NULL
        };
        Fmt(bf, " idx^D.$^d.", args);
    }
    if(tg->type.state & FETCH_TARGET_RESOLVED){
        void *args[] = {
            Type_ToStr(bf->m, tg->type.of),
            I16_Wrapped(bf->m, tg->offsetType->of),
            NULL
        };
        Fmt(bf, " -> $/offset^D.$^d.>", args);
    }
    if(tg->type.state & FETCH_TARGET_FUNC){
        void *args[] = {
            Ptr_Wrapped(bf->m, tg->func, TYPE_FETCH_FUNC),
            NULL
        };
        Fmt(bf, "func/@", args);
    }
    return Buff_AddBytes(bf, (byte *)">", 1);
}

status Inst_Print(Buff *bf, void *a, cls type, word flags){
    static i32 _objIndent = 0;
    Inst *obj = NULL;
    if(((Abstract *)a)->type.of & TYPE_INSTANCE){
        obj = (Inst *)a;
    }else{
        as(a, TYPE_INSTANCE);
    }
    Table *seel = Lookup_Get(SeelLookup, obj->type.of);
    Str *name = Lookup_Get(SeelNameLookup, obj->type.of);
    if(flags & MORE){
        void *args[] = {
            name,
            Type_StateVec(bf->m, obj->type.of, obj->type.state),
            NULL,
        };
        Fmt(bf, "$<@", args);
        Iter it;
        Iter_Init(&it, seel);
        while((Iter_Next(&it) & END) == 0){
            Hashed *h = Iter_Get(&it);;
            ToS(bf, h->key, ZERO, flags); 
            Buff_AddBytes(bf, (byte *)": ", 2);
            ToS(bf, Span_Get(obj, h->orderIdx), ZERO, flags); 
            if((it.type.state & LAST) == 0){
                Buff_AddBytes(bf, (byte *)", ", 2);
            }
        }
        return SUCCESS;
    }else{
        return ToStream_NotImpl(bf, a, type, flags);
    }
}

status Caneka_Init(MemCh *m){
    status r = READY;
    Lookup *lk = NULL;
    if(_init){
        r |= NOOP;
        return r;
    }
    _init = TRUE;

    /* labels */
    lk = ToSFlagLookup;

    Str **fetcherLabels = (Str **)Arr_Make(m, 17);
    fetcherLabels[9] = Str_CstrRef(m, "VAR");
    fetcherLabels[10] = Str_CstrRef(m, "FOR");
    fetcherLabels[11] = Str_CstrRef(m, "WITH");
    fetcherLabels[12] = Str_CstrRef(m, "IF");
    fetcherLabels[13] = Str_CstrRef(m, "IFNOT");
    fetcherLabels[14] = Str_CstrRef(m, "END");
    fetcherLabels[15] = Str_CstrRef(m, "COMMAND");
    fetcherLabels[16] = Str_CstrRef(m, "TEMPL");
    r |= Lookup_Add(m, lk, TYPE_FETCHER, (void *)fetcherLabels);

    Str **fetchTargetLabels = (Str **)Arr_Make(m, 17);
    fetchTargetLabels[9] = Str_CstrRef(m, "ATT");
    fetchTargetLabels[10] = Str_CstrRef(m, "KEY");
    fetchTargetLabels[11] = Str_CstrRef(m, "IDX");
    fetchTargetLabels[12] = Str_CstrRef(m, "ITER");
    fetchTargetLabels[13] = Str_CstrRef(m, "PROP");
    fetchTargetLabels[14] = Str_CstrRef(m, "FUNC");
    fetchTargetLabels[15] = Str_CstrRef(m, "RESOLVED");
    fetchTargetLabels[16] = Str_CstrRef(m, "HASH");
    r |= Lookup_Add(m, lk, TYPE_FETCH_TARGET, (void *)fetchTargetLabels);

    /* ToS */
    lk = ToStreamLookup;
    r |= Lookup_Add(m, lk, TYPE_FETCHER, (void *)Fetcher_Print);
    r |= Lookup_Add(m, lk, TYPE_FETCH_TARGET, (void *)FetchTarget_Print);
    r |= Lookup_Add(m, lk, TYPE_FETCH_FUNC, (void *)FetchTargetFunc_Print);

    /* exact */
    lk = ExactLookup;
    r |= Lookup_Add(m, lk, TYPE_FETCHER, (void *)Fetcher_Exact); 
    r |= Lookup_Add(m, lk, TYPE_FETCH_TARGET, (void *)FetchTarget_Exact); 

    /* overide hashed print */
    r |= Lookup_Add(m, ToStreamLookup, TYPE_HASHED, (void *)Hashed_Print); 

    r |= Caneka_InitBase(m);
    r |= ExtTypeStrings_Init(m);
    r |= Caneka_LicenceInit(m);
    r |= Parser_Init(m);
    r |= Persist_Init(m);
    r |= Format_Init(m);
    r |= Transp_Init(m);
    r |= Sequence_Init(m);
    r |= BinSeg_Init(m);
    r |= Navigate_ClsInit(m);
    r |= Task_Init(m);
    r |= Serve_ClsInit(m);
    r |= NodeObj_ClsInit(m);

    return r;
}
