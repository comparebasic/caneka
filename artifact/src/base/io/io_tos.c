#include <external.h>
#include <caneka.h>

static Str **buffLabels = NULL;

i64 StashCoords_Print(Stream *sm, StashCoord *coord, word flags){
    i64 total = 0;
    Abstract *args[] = {
        (Abstract *)Type_ToStr(sm->m, coord->typeOf),
        (Abstract *)I32_Wrapped(sm->m, coord->idx),
        NULL
    };
    total += Fmt(sm, "$/$/[", args);
    total += Bits_Print(OutStream, (byte *)&coord->offset, sizeof(quad), DEBUG|MORE);
    total += Stream_Bytes(sm, (byte *)"]", 1);
    return total;
}

i64 Buff_Print(Stream *sm, Abstract *a, cls type, word flags){
    i64 total = 0;
    Buff *bf = (Buff *)as(a, TYPE_BUFF);
    Abstract *args[] = {
        (Abstract *)StreamTask_Make(sm->m, NULL, (Abstract *)bf, ToS_FlagLabels),
        (Abstract *)I64_Wrapped(sm->m, bf->unsent.total),
        (Abstract *)I64_Wrapped(sm->m, bf->v->total),
        NULL
    };
    total += Fmt(sm, "Buff<$ unsent/total=$/$>", args);
    return total;
}

i64 StashItem_Print(Stream *sm, Abstract *a, cls type, word flags){
    i64 total = 0;
    StashItem *item = (StashItem *)as(a, TYPE_STASH_ITEM);
    Abstract *args[] = {
        (Abstract *)Util_Wrapped(sm->m, (util)item->ptr),
        NULL
    };
    total += Fmt(sm, "StashItem<@ ", args);
    total += StashCoords_Print(sm, &item->coord, flags);
    total += Stream_Bytes(sm, (byte *)">", 1);
    return total;
}

i64 ProcDets_Print(Stream *sm, Abstract *a, cls type, word flags){
    ProcDets *pd = (ProcDets *)as(a, TYPE_PROCDETS);
    Abstract *args[] = {
        (Abstract *)State_ToStr(sm->m, pd->type.state),
        (Abstract *)I32_Wrapped(sm->m, pd->pid), 
        (Abstract *)I32_Wrapped(sm->m, pd->code),
        NULL
    };
    if(flags & (MORE|DEBUG)){
        return Fmt(sm, "ProcDets<$ $ return:$>", args); 
    }else{
        return ToStream_NotImpl(sm, a, type, flags);
    }
}

status Io_InitLabels(MemCh *m, Lookup *lk){
    status r = READY;
    if(buffLabels == NULL){
        buffLabels = (Str **)Arr_Make(m, 17);
        buffLabels[9] = Str_CstrRef(m, "OUT");
        buffLabels[10] = Str_CstrRef(m, "IN");
        buffLabels[11] = Str_CstrRef(m, "ASYNC");
        buffLabels[12] = Str_CstrRef(m, "FD");
        buffLabels[13] = Str_CstrRef(m, "SOCKET");
        buffLabels[14] = Str_CstrRef(m, "STRVEC");
        buffLabels[15] = Str_CstrRef(m, "FLUSH");
        buffLabels[15] = Str_CstrRef(m, "UNBUFFERED");
        Lookup_Add(m, lk, TYPE_BUFF, (void *)buffLabels);
        r |= SUCCESS;
    }
    return r;
}

status IoTos_Init(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_PROCDETS, (void *)ProcDets_Print);
    r |= Lookup_Add(m, lk, TYPE_STASH_ITEM, (void *)StashItem_Print);
    r |= Lookup_Add(m, lk, TYPE_BUFF, (void *)Buff_Print);
    r |= Io_InitLabels(m, ToSFlagLookup);
    return r;
}
