#include <external.h>
#include <caneka.h>

static boolean _init = FALSE;

static boolean FetchTarget_Exact(FetchTarget  *a, FetchTarget *b){
    if((a->type.state & UPPER_FLAGS) != (b->type.state & UPPER_FLAGS) ||
            a->idx != b->idx || a->offset != b->offset || 
            !Exact((Abstract *)a->key, (Abstract *)b->key)){
        return FALSE;
    }
    return TRUE;
}

static boolean Fetcher_Exact(Fetcher *a, Fetcher *b){
    if((a->type.state & UPPER_FLAGS) != (b->type.state & UPPER_FLAGS)){
        return FALSE;
    }
    if(a->type.state & FETCHER_JUMP){
        return Exact((Abstract *)a->val.jump, (Abstract *)b->val.jump);
    }else{
        return Exact((Abstract *)a->val.targets, (Abstract *)b->val.targets);
    }
}

static status Fetcher_Print(Stream *sm, Abstract *a, cls type, word flags){
    i64 total = 0;
    Fetcher *fch = (Fetcher *)as(a, TYPE_FETCHER);
    Abstract *args[] = {
        (Abstract *)StreamTask_Make(sm->m, NULL, (Abstract *)fch, ToS_FlagLabels),
        NULL,
    };
    total += Fmt(sm, "Fetcher<$ ", args);
    if(fch->type.state & FETCHER_JUMP){
        Abstract *args[] = {
            (Abstract *)fch->val.jump,
            NULL,
        };
        total += Fmt(sm, "@>", args);
    }else{
        Abstract *args[] = {
            (Abstract *)fch->val.targets,
            NULL,
        };
        total += Fmt(sm, "@>", args);
    }

    return total;
}

static status FetchTarget_Print(Stream *sm, Abstract *a, cls type, word flags){
    i64 total = 0;
    FetchTarget *tg = (FetchTarget *)as(a, TYPE_FETCH_TARGET);
    Abstract *args[] = {
        (Abstract *)StreamTask_Make(sm->m, NULL, (Abstract *)tg, ToS_FlagLabels),
        NULL,
    };
    total += Fmt(sm, "FT<$", args);
    if(tg->key != NULL){
        Abstract *args[] = {
            (Abstract *)tg->key,
            NULL
        };
        total += Fmt(sm, " @", args);
    }
    if(tg->type.state & FETCH_TARGET_IDX){
        Abstract *args[] = {
            (Abstract *)I32_Wrapped(sm->m, tg->idx),
            NULL
        };
        total += Fmt(sm, " idx^D.$^d.", args);
    }
    if(tg->type.state & FETCH_TARGET_RESOLVED){
        Abstract *args[] = {
            (Abstract *)Type_ToStr(sm->m, tg->objType.of),
            (Abstract *)I16_Wrapped(sm->m, tg->offset),
            NULL
        };
        total += Fmt(sm, " -> $/offset^D.$^d.>", args);
    }
    if(tg->type.state & FETCH_TARGET_FUNC){
        Abstract *args[] = {
            (Abstract *)Ptr_Wrapped(sm->m, tg->func, FETCH_TARGET_FUNC),
            NULL
        };
        total += Fmt(sm, "func/@", args);
    }
    total += Stream_Bytes(sm, (byte *)">", 1);

    return total;
}

status Types_ClsInit(MemCh *m){
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
    fetcherLabels[15] = Str_CstrRef(m, "JUMP");
    r |= Lookup_Add(m, lk, TYPE_FETCHER, (void *)fetcherLabels);

    Str **fetchTargetLabels = (Str **)Arr_Make(m, 17);
    fetchTargetLabels[9] = Str_CstrRef(m, "ATT");
    fetchTargetLabels[10] = Str_CstrRef(m, "KEY");
    fetchTargetLabels[11] = Str_CstrRef(m, "IDX");
    fetchTargetLabels[12] = Str_CstrRef(m, "SELF");
    fetchTargetLabels[13] = Str_CstrRef(m, "ITER");
    fetchTargetLabels[14] = Str_CstrRef(m, "FUNC");
    fetchTargetLabels[15] = Str_CstrRef(m, "RESOLVED");
    r |= Lookup_Add(m, lk, TYPE_FETCH_TARGET, (void *)fetchTargetLabels);

    /* ToS */
    lk = ToStreamLookup;
    r |= Lookup_Add(m, lk, TYPE_FETCHER, (void *)Fetcher_Print);
    r |= Lookup_Add(m, lk, TYPE_FETCH_TARGET, (void *)FetchTarget_Print);

    /* exact */
    lk = ExactLookup;
    r |= Lookup_Add(m, lk, TYPE_FETCHER, (void *)Fetcher_Exact); 
    r |= Lookup_Add(m, lk, TYPE_FETCH_TARGET, (void *)FetchTarget_Exact); 
    return r;
}

