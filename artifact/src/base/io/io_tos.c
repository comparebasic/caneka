#include <external.h>
#include <caneka.h>

static Str **buffLabels = NULL;

char *Buff_WhenceChars(i64 whence){
    if(whence == SEEK_SET){
        return "SEEK_SET";
    }else if(whence == SEEK_END){
        return "SEEK_END";
    }else if(whence == SEEK_CUR){
        return "SEEK_CUR";
    }else{
        return "SEEK_unknown";
    }
}

i64 StashCoords_Print(Buff *bf, StashCoord *coord, word flags){
    i64 total = 0;
    Abstract *args[] = {
        (Abstract *)Type_ToStr(bf->m, coord->typeOf),
        (Abstract *)I32_Wrapped(bf->m, coord->idx),
        NULL
    };
    total += Fmt(bf, "$/$/[", args);
    total += Bits_Print(OutStream, (byte *)&coord->offset, sizeof(quad), DEBUG|MORE);
    total += Buff_Bytes(bf, (byte *)"]", 1);
    return total;
}

i64 Buff_Print(Buff *bf, Abstract *a, cls type, word flags){
    i64 total = 0;
    Buff *bfObj = (Buff *)as(a, TYPE_BUFF);

    Abstract *args[8];
    args[0] = (Abstract *)Type_StateVec(bf->m, bfObj->type.of, bfObj->type.state);
    args[1] = (Abstract *)I32_Wrapped(bf->m, bfObj->fd);
    args[2] = (Abstract *)I64_Wrapped(bf->m, bfObj->unsent.total);
    args[3] = (Abstract *)I64_Wrapped(bf->m, bfObj->v->total);
    args[4] = NULL;
    if(flags & DEBUG){
        Buff_Stat(bf);
        args[4] = args[2];
        args[5] = args[3];

        args[2] = (Abstract *)I32_Wrapped(bf->m, bfObj->st.st_size);
        args[3] = (Abstract *)I32_Wrapped(bf->m, lseek(bfObj->fd, 0, SEEK_CUR));

        args[6] = (bf->unsent.s == NULL ? (Abstract *)bfObj->v : (Abstract *)bfObj->unsent.s);
        args[7] = NULL;
        total += Fmt(bf, "Buff<@ $fd/$bytes\\@@ unsent/total=$/$ &>", args);
    }else{
        total += Fmt(bf, "Buff<@ $fd unsent/total=$/$>", args);
    }
    return total;
}

i64 StashItem_Print(Buff *bf, Abstract *a, cls type, word flags){
    i64 total = 0;
    StashItem *item = (StashItem *)as(a, TYPE_STASH_ITEM);
    Abstract *args[] = {
        (Abstract *)Util_Wrapped(bf->m, (util)item->ptr),
        NULL
    };
    total += Fmt(bf, "StashItem<@ ", args);
    total += StashCoords_Print(bf, &item->coord, flags);
    total += Buff_Bytes(bf, (byte *)">", 1);
    return total;
}

i64 ProcDets_Print(Buff *bf, Abstract *a, cls type, word flags){
    ProcDets *pd = (ProcDets *)as(a, TYPE_PROCDETS);
    Abstract *args[] = {
        (Abstract *)State_ToStr(bf->m, pd->type.state),
        (Abstract *)I32_Wrapped(bf->m, pd->pid), 
        (Abstract *)I32_Wrapped(bf->m, pd->code),
        NULL
    };
    if(flags & (MORE|DEBUG)){
        return Fmt(bf, "ProcDets<$ $ return:$>", args); 
    }else{
        return ToStream_NotImpl(bf, a, type, flags);
    }
}

status Io_InitLabels(MemCh *m, Lookup *lk){
    status r = READY;
    if(buffLabels == NULL){
        buffLabels = (Str **)Arr_Make(m, 17);
        buffLabels[9] = Str_CstrRef(m, "ASYNC");
        buffLabels[10] = Str_CstrRef(m, "FD");
        buffLabels[11] = Str_CstrRef(m, "SOCKET");
        buffLabels[12] = Str_CstrRef(m, "STRVEC");
        buffLabels[13] = Str_CstrRef(m, "FLUSH");
        buffLabels[14] = Str_CstrRef(m, "UNBUFFERED");
        buffLabels[15] = Str_CstrRef(m, "SLURP");
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
