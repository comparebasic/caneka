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
        h->item->type.of += HTYPE_LOCAL;
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
        h->value->type.of += HTYPE_LOCAL;
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
        h->next->type.of += HTYPE_LOCAL;
        memcpy(&h->next, &lptr, sizeof(void *));
        r |= SUCCESS;
    }

    DebugStack_Pop();
    return NOOP;
}

status Hashed_FromLocal(MemCtx *m, Abstract *a){
    return NOOP;
}
