#include <external.h>
#include <caneka.h>

status String_ToLocal(MemCtx *m, Abstract *a){
    DebugStack_Push("String_ToLocal", TYPE_CSTR);
    status r = READY;
    String *s = asIfc(a, TYPE_STRING);
    while(s != NULL){
        s->type.of += HTYPE_LOCAL;
        if(String_Next(s) != NULL){
            LocalPtr lptr;
            String *_next = s->next;
            r |= MemLocal_GetLocal(m, s->next, &lptr);
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
    if(s->type.of != TYPE_STRING_CHAIN){
        s->type.of -= HTYPE_LOCAL;
        DebugStack_Pop();
        return NOOP;
    }
    while(s != NULL){
        s->type.of -= HTYPE_LOCAL;
        if(s->next != NULL){
            s->type.of -= HTYPE_LOCAL;
            s->next = (String *)MemLocal_GetPtr(m, (LocalPtr *)&s->next);
            s = s->next;
            r |= SUCCESS;
        }else{
            s->type.of -= HTYPE_LOCAL;
            s = NULL;
        }
    }

    if(r == READY){
        r |= NOOP;
    }

    DebugStack_Pop();
    return NOOP;
}
