#include <external.h>
#include <caneka.h>

status Hashed_ToLocal(MemCtx *m, Abstract *a){
    DebugStack_Push("Hashed_ToLocal", TYPE_CSTR);
    status r = READY;
    Hashed *h = asIfc(a, TYPE_HASHED);
    MemLocal_To(m, h->item);
    r |= MemLocal_SetLocal(m, &h->item);
    MemLocal_To(m, h->value);
    r |= MemLocal_SetLocal(m, &h->value);
    MemLocal_To(m, (Abstract *)h->next);
    r |= MemLocal_SetLocal(m, (Abstract **)&h->next);
    h->type.of += HTYPE_LOCAL;

    DebugStack_Pop();
    return NOOP;
}

status Hashed_FromLocal(MemCtx *m, Abstract *a){
    DebugStack_Push("Hashed_FromLocal", TYPE_CSTR);
    status r = READY;
    Hashed *h = asIfcOffset(a, TYPE_HASHED, HTYPE_LOCAL);
    r |= MemLocal_UnSetLocal(m, (Abstract **)&h->item);
    MemLocal_From(m, h->item);
    r |= MemLocal_UnSetLocal(m, (Abstract **)&h->value);
    MemLocal_From(m, h->value);
    r |= MemLocal_UnSetLocal(m, (Abstract **)&h->next);
    MemLocal_From(m, (Abstract *)h->next);
    h->type.of -= HTYPE_LOCAL;

    DebugStack_Pop();
    return SUCCESS;
}
