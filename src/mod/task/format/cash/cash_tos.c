#include <external.h>
#include <caneka.h>

status Jump_Print(Buff *bf, void *a, cls type, word flags){
    MemCh *m = bf->m;
    Jump *jmp = (Jump *)Ifc(bf->m, a, TYPE_CASH_JUMP);
    void *args[] = {
        Type_StateVec(bf->m, jmp->type.of, jmp->type.state),
        Type_ToStr(m, jmp->objType.of),
        I32_Wrapped(m, jmp->idx),
    };
    return Fmt(bf, "Jump<@ @ $>", args);
}

status Cash_ToSInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_CASH_JUMP, (void *)Jump_Print);
    return r;
}
