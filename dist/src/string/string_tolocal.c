#include <external.h>
#include <caneka.h>

status String_ToLocal(MemCtx *m, Abstract *a){
    DebugStack_Push("String_ToLocal", TYPE_CSTR);
    status r = READY;
    String *s = asIfc(a, TYPE_STRING);
    while(s != NULL){
        if(String_Next(s) != NULL){
            LocalPtr lptr;
            String *_next = s->next;
            r |= MemLocal_GetLocal(m, s->next, &lptr);
            s->type.of += HTYPE_LOCAL;
            memcpy(&s->next, &lptr, sizeof(void *));
            s = _next;
            r |= SUCCESS;
        }else{
            s = NULL;
        }
    }
    
    if(r == READY){
        r |= NOOP;
    }

    DebugStack_Pop();
    return r;
}

status String_FromLocal(MemCtx *m, Abstract *a){
    DebugStack_Push("String_FromLocal", TYPE_CSTR);
    status r = READY;
    String *s = (String *)asIfcOffset(a, TYPE_STRING, HTYPE_LOCAL);
    if(s->type.of != TYPE_STRING_CHAIN+HTYPE_LOCAL){
        DebugStack_Pop();
        return NOOP;
    }
    while(s != NULL){
        if(s->next != NULL){
            s->next = (String *)MemLocal_GetPtr(m, (LocalPtr *)&s->next);
            s->type.of -= HTYPE_LOCAL;
            s = s->next;
            r |= SUCCESS;
        }else{
            s = NULL;
        }
    }

    if(r == READY){
        r |= NOOP;
    }

    DebugStack_Pop();
    return NOOP;
}
