#include <external.h>
#include <caneka.h>

status String_ToLocal(MemCtx *m, Abstract *a){
    DebugStack_Push("String_ToLocal", TYPE_CSTR);
    status r = READY;
    String *s = asIfc(a, TYPE_STRING_CHAIN);
    while(s != NULL){
        s->type.of += HTYPE_LOCAL;
        if(s->next != NULL){
            String *_next = s->next;
            r |= MemLocal_SetLocal(m, (Abstract **)&s->next);
            s = _next;
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
    String *s = (String *)asIfcOffset(a, TYPE_STRING_CHAIN, HTYPE_LOCAL);
    while(s != NULL){
        s->type.of -= HTYPE_LOCAL;
        if(s->next != NULL){
            LocalPtr *debugLptr = (LocalPtr *)&s->next;
            MemLocal_UnSetLocal(m, (Abstract **)&s->next);
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
