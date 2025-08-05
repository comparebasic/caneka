#include <external.h>
#include <caneka.h>

static Str **fetcherLabels = NULL;
static Str **fetcherOpLabels = NULL;

static status Fetcher_Print(Stream *sm, Abstract *a, cls type, word flags){
    i64 total = 0;
    Fetcher *fch = (Fetcher *)as(a, TYPE_FETCHER);
    Abstract *args[] = {
        (Abstract *)StreamTask_Make(sm->m, NULL, (Abstract *)fch, ToS_FlagLabels),
        (Abstract *)fch->path,
        (Abstract *)fch->key,
        (Abstract *)I32_Wrapped(sm->m, fch->idx),
        NULL,
    }
    total += Fmt(sm, "Fetcher<$ ", args);

    Str **labels = Lookup_Get(ToSFlagLookup, fch->objType.of);
    word flags = a->type.state;
    total +=  ToS_LabelsFromFlag(sm, a, flags, labels);

    Abstract *args2[] = {
        (Abstract *)fch->path,
        (Abstract *)fch->key,
        (Abstract *)I32_Wrapped(sm->m, fch->idx),
        NULL,
    }
    total += Fmt(sm, " @ $/$", args2);

    return total;
}

status ExtTypes_ToSInit(MemCh *m, Lookup *lk){
    status r = READY;

    if(fetcherLabels == NULL){
        fetchTargetLabels = (Str **)Arr_Make(m, 17);
        fetchTargetLabels[9] = Str_CstrRef(m, "ATT");
        fetchTargetLabels[10] = Str_CstrRef(m, "KEY");
        fetchTargetLabels[11] = Str_CstrRef(m, "VALUE");
        fetchTargetLabels[12] = Str_CstrRef(m, "IDX");
        fetchTargetLabels[13] = Str_CstrRef(m, "NEST");
        fetchTargetLabels[14] = Str_CstrRef(m, "ITER");
        fetchTargetLabels[15] = Str_CstrRef(m, "FUNC");
        Lookup_Add(m, lk, TYPE_FETCH_TARGET, (void *)fetchTargetLabels);
        r |= SUCCESS;
    }

    if(fetcherOpLabels == NULL){
        fetcherLabels = (Str **)Arr_Make(m, 17);
        fetcherLabels[9] = Str_CstrRef(m, "FOR");
        fetcherLabels[10] = Str_CstrRef(m, "WITH");
        fetcherLabels[11] = Str_CstrRef(m, "IF");
        fetcherLabels[12] = Str_CstrRef(m, "IFNOT");
        fetcherLabels[13] = Str_CstrRef(m, "END");
        fetcherLabels[14] = Str_CstrRef(m, "JUMP");
        Lookup_Add(m, lk, TYPE_FETCHER, (void *)fetcherLabels);
        r |= SUCCESS;
    }

    r |= Lookup_Add(m, lk, TYPE_FETCHER, (void *)Fetcher_Print);
    return r;
}


