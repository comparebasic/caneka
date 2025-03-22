#include <external.h>
#include <caneka.h>
i64 ProcDets_Print(MemCtx *m, StrVec *v, i32 fd, Abstract *a, cls type, boolean extended){
    ProcDets *pd = (ProcDets *)as(a, TYPE_PROCDETS);
    return StrVec_FmtAdd(m, v, fd, "ProcDets<_t _i8 return:_i4>", 
        State_ToStr(m, pd->type.state), pd->pid, pd->code);
}

status PersistDebug_Init(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_PROCDETS, (void *)ProcDets_Print);
    return r;
}
