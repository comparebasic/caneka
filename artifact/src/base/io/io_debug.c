#include <external.h>
#include <caneka.h>

i64 ProcDets_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    ProcDets *pd = (ProcDets *)as(a, TYPE_PROCDETS);
    Abstract *args[] = {
        (Abstract *)State_ToStr(sm->m, pd->type.state),
        (Abstract *)I32_Wrapped(sm->m, pd->pid), 
        (Abstract *)I32_Wrapped(sm->m, pd->code),
        NULL
    };
    return Fmt(sm, "ProcDets<$ $ return:$>", args); 
}

status PersistDebug_Init(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_PROCDETS, (void *)ProcDets_Print);
    return r;
}
