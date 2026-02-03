#include <external.h>
#include "base_module.h"

static Str **buffLabels = NULL;
static Str **dirSelLabels = NULL;

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

status StashCoords_Print(Buff *bf, StashCoord *coord, word flags){
    void *args[] = {
        Type_ToStr(bf->m, coord->typeOf),
        I32_Wrapped(bf->m, coord->idx),
        NULL
    };
    Fmt(bf, "$/$/[", args);
    Bits_Print(OutStream, (byte *)&coord->offset, sizeof(quad), DEBUG|MORE);
    return Buff_AddBytes(bf, (byte *)"]", 1);
}

status DirSel_Print(Buff *bf, void *a, cls type, word flags){
    DirSel *sel = (DirSel *)as(a, TYPE_DIR_SELECTOR);
    if(flags & DEBUG){
        void *args[] = {
            Type_StateVec(bf->m, sel->type.of, sel->type.state),
            Time_ToStr(bf->m, &sel->time),
            sel->ext,
            sel->source,
            sel->dest,
            sel->meta,
            NULL
        };
        return Fmt(bf, "DirSel<@ @ ext:@ source:@ files:@ meta:@>", args);
    }else{
        void *args[] = {
            Type_StateVec(bf->m, sel->type.of, sel->type.state),
            Time_ToStr(bf->m, &sel->time),
            I32_Wrapped(bf->m, sel->dest->nvalues),
            sel->meta,
            NULL
        };
        return Fmt(bf, "DirSel<@ @ ^D.$^d.files meta:@>", args);
    }
}

status Buff_Print(Buff *bf, void *a, cls type, word flags){
    Buff *bfObj = (Buff *)as(a, TYPE_BUFF);

    void *args[9];
    args[0] = Type_StateVec(bf->m, bfObj->type.of, bfObj->type.state);
    args[1] = I32_Wrapped(bf->m, bfObj->fd);
    args[2] = I64_Wrapped(bf->m, bfObj->unsent.total);
    args[3] = I64_Wrapped(bf->m, bfObj->v->total);
    args[4] = NULL;
    if(flags & DEBUG){
        Buff_Stat(bf);
        args[4] = args[2];
        args[5] = args[3];

        args[2] = I32_Wrapped(bf->m, bfObj->st.st_size);
        args[3] = I32_Wrapped(bf->m, lseek(bfObj->fd, 0, SEEK_CUR));

        args[6] = I32_Wrapped(bf->m, bfObj->tail.idx);
        args[7] = (bf->unsent.s == NULL ? (void *)bfObj->v : (void *)bfObj->unsent.s);
        args[8] = NULL;
        return Fmt(bf, "Buff<@ $fd/$bytes\\@@ unsent/total=$/$ $tailIdx &>", args);
    }else{
        return Fmt(bf, "Buff<@ $fd unsent/total=$/$>", args);
    }
}

status StashItem_Print(Buff *bf, void *a, cls type, word flags){
    StashItem *item = (StashItem *)as(a, TYPE_STASH_ITEM);
    void *args[] = {
        Util_Wrapped(bf->m, (util)item->ptr),
        NULL
    };
    Fmt(bf, "StashItem<@ ", args);
    StashCoords_Print(bf, &item->coord, flags);
    return Buff_AddBytes(bf, (byte *)">", 1);
}

status ProcDets_Print(Buff *bf, void *a, cls type, word flags){
    ProcDets *pd = (ProcDets *)as(a, TYPE_PROCDETS);
    void *args[] = {
        State_ToStr(bf->m, pd->type.state),
        I32_Wrapped(bf->m, pd->pid), 
        I32_Wrapped(bf->m, pd->code),
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
    if(dirSelLabels == NULL){
        dirSelLabels = (Str **)Arr_Make(m, 17);
        dirSelLabels[9] = Str_CstrRef(m, "MTIME_ALL");
        dirSelLabels[10] = Str_CstrRef(m, "MTIME_LOWEST");
        dirSelLabels[11] = Str_CstrRef(m, "NODIRS");
        dirSelLabels[12] = Str_CstrRef(m, "FILTER");
        dirSelLabels[13] = Str_CstrRef(m, "FILTER_DIRS");
        dirSelLabels[14] = Str_CstrRef(m, "INVERT");
        Lookup_Add(m, lk, TYPE_DIR_SELECTOR, (void *)dirSelLabels);
        r |= SUCCESS;
    }
    return r;
}

status IoTos_Init(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_PROCDETS, (void *)ProcDets_Print);
    r |= Lookup_Add(m, lk, TYPE_STASH_ITEM, (void *)StashItem_Print);
    r |= Lookup_Add(m, lk, TYPE_BUFF, (void *)Buff_Print);
    r |= Lookup_Add(m, lk, TYPE_DIR_SELECTOR, (void *)DirSel_Print);
    r |= Io_InitLabels(m, ToSFlagLookup);
    return r;
}
