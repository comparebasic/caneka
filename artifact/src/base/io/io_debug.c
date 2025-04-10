#include <external.h>
#include <caneka.h>

i64 ProcDets_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    ProcDets *pd = (ProcDets *)as(a, TYPE_PROCDETS);
    return StrVec_Fmt(sm, "ProcDets<_t _i8 return:_i4>", 
        State_ToStr(sm->m, pd->type.state), pd->pid, pd->code);
}

status PersistDebug_Init(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_PROCDETS, (void *)ProcDets_Print);
    return r;
}
