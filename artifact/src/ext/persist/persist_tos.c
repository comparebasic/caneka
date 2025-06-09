#include <external.h>
#include <caneka.h>

static i64 File_Print(Stream *sm, Abstract *a, cls type, word flags){
    File *file = (File *)as(a, TYPE_FILE);
    Abstract *args[] = {
        (Abstract *)StreamTask_Make(sm->m, NULL, (Abstract *)file, ToS_FlagLabels),
        (Abstract *)file->path,
        (Abstract *)file->sm,
        NULL
    };
    return Fmt(sm, "File<$ $ @>", args);
}

status Persist_ToSInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_FILE, (void *)File_Print);
    return r;
}

