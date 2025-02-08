#include <external.h>
#include <caneka.h>

status Hashed_ToLocal(MemCtx *m, Abstract *a){
    DebugStack_Push("Hashed_ToLocal", TYPE_CSTR);
    status r = READY;
    Hashed *h = asIfc(a, TYPE_HASHED);
    if(h->item != NULL){
        r |= MemLocal_To(m, h->item);
        if(r & ERROR){
            DebugStack_Pop();
            return ERROR;
        }
        LocalPtr lptr;
        r |=  MemLocal_GetLocal(m, h->item, &lptr);
        memcpy(&h->item, &lptr, sizeof(void *));
        r |= SUCCESS;
    }
    if(h->value != NULL){
        r |= MemLocal_To(m, h->value);
        if(r & ERROR){
            DebugStack_Pop();
            return ERROR;
        }
        LocalPtr lptr;
        r |=  MemLocal_GetLocal(m, h->value, &lptr);
        memcpy(&h->value, &lptr, sizeof(void *));
        r |= SUCCESS;
    }
    if(h->next != NULL){
        r |= MemLocal_To(m, (Abstract *)h->next);
        if(r & ERROR){
            DebugStack_Pop();
            return ERROR;
        }
        LocalPtr lptr;
        r |=  MemLocal_GetLocal(m, h->next, &lptr);
        memcpy(&h->next, &lptr, sizeof(void *));
        r |= SUCCESS;
    }

    h->type.of += HTYPE_LOCAL;

    DebugStack_Pop();
    return NOOP;
}

status Hashed_FromLocal(MemCtx *m, Abstract *a){
    DebugStack_Push("Hashed_FromLocal", TYPE_CSTR);
    status r = READY;
    Hashed *h = asIfcOffset(a, TYPE_HASHED, HTYPE_LOCAL);
    if(h->item != NULL){
        h->item = MemLocal_GetPtr(m, (LocalPtr *)&h->item);
        r |= MemLocal_From(m, h->item);
    }
    if(h->value != NULL){
        h->value = MemLocal_GetPtr(m, (LocalPtr *)&h->value);
        r |= MemLocal_From(m, h->value);
        r |= SUCCESS;
    }
    if(h->next != NULL){
        h->next = (Hashed *)MemLocal_GetPtr(m, (LocalPtr *)&h->next);
        r |= MemLocal_From(m, (Abstract *)h->next);
        r |= SUCCESS;
    }

    h->type.of -= HTYPE_LOCAL;

    DebugStack_Pop();
    return SUCCESS;
}
