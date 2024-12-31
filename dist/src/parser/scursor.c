#include <external.h>
#include <caneka.h>

status SCursor_Init(SCursor *sc, String *s){
    memset(sc, 0, sizeof(SCursor));
    sc->type.of = TYPE_SCURSOR;
    sc->s = s;
    sc->seg = sc->s;
    return SUCCESS;
}

SCursor* SCursor_Make(MemCtx *m, String *s){
    SCursor *sc = (SCursor *) MemCtx_Alloc(m, sizeof(SCursor));
    sc->type.of = TYPE_SCURSOR;
    sc->s = s;
    sc->seg = sc->s;

    return sc;
}

status SCursor_Reset(SCursor *sc){
    String *s = sc->s;
    memset(sc, 0, sizeof(SCursor));
    sc->s = s;
    sc->seg = s;
    return SUCCESS;
}

status SCursor_Incr(SCursor *sc, i64 length){
    if(sc->type.state == END){
        return sc->type.state;
    }

    i64 remaining = length;
    i64 local;
    while(remaining > 0 && sc->seg != NULL && !HasFlag(sc->type.state, END)){
        local = sc->seg->length - sc->position;
        if(local > remaining){
            sc->position += remaining;
            sc->total += remaining;
            remaining = 0;
            break;
        }else{
            String *next = String_Next(sc->seg);
            if(next == NULL){
                sc->type.state = END;
            }else{
                sc->seg = next;
                sc->position = 0;
            }

            remaining -= local;
        }
    }

    if(remaining > 0){
        sc->type.state |= ERROR;
    }

    return sc->type.state;
}
