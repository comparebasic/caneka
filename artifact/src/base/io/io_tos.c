#include <external.h>
#include <caneka.h>

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

status IoTos_Init(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_PROCDETS, (void *)ProcDets_Print);
    r |= Lookup_Add(m, lk, TYPE_STASH_ITEM, (void *)StashItem_Print);
    return r;
}
