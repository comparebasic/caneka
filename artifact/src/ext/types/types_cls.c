#include <external.h>
#include <caneka.h>

static boolean _init = FALSE;

static boolean Fetcher_Exact(Fetcher *a, Fetcher *b){
    return FALSE;
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
    Str **fetchTargetLabels = (Str **)Arr_Make(m, 17);
    fetchTargetLabels[9] = Str_CstrRef(m, "ATT");
    fetchTargetLabels[10] = Str_CstrRef(m, "KEY");
    fetchTargetLabels[11] = Str_CstrRef(m, "VALUE");
    fetchTargetLabels[12] = Str_CstrRef(m, "IDX");
    fetchTargetLabels[13] = Str_CstrRef(m, "NEST");
    fetchTargetLabels[14] = Str_CstrRef(m, "ITER");
    fetchTargetLabels[15] = Str_CstrRef(m, "FUNC");
    Lookup_Add(m, lk, TYPE_FETCH_TARGET, (void *)fetchTargetLabels);
    r |= SUCCESS;

    Str **fetcherLabels = (Str **)Arr_Make(m, 17);
    fetcherLabels[9] = Str_CstrRef(m, "FOR");
    fetcherLabels[10] = Str_CstrRef(m, "WITH");
    fetcherLabels[11] = Str_CstrRef(m, "IF");
    fetcherLabels[12] = Str_CstrRef(m, "IFNOT");
    fetcherLabels[13] = Str_CstrRef(m, "END");
    fetcherLabels[14] = Str_CstrRef(m, "JUMP");
    Lookup_Add(m, lk, TYPE_FETCHER, (void *)fetcherLabels);
    r |= SUCCESS;

    /* ToS */
    lk = ToStreamLookup;
    r |= Lookup_Add(m, lk, TYPE_FETCHER, (void *)Fetcher_Print);


    /* exact */
    lk = ExactLookup;
    r |= Lookup_Add(m, lk, TYPE_FETCHER, (void *)Fetcher_Exact); 
    return r;
}

