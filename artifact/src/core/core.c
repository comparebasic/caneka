#include <external.h>
#include <caneka.h>

word GLOBAL_flags = 0;
word NORMAL_FLAGS = 0b0000000011111111;
word UPPER_FLAGS = 0b1111111100000000;

static word states[] = {
    READY,
    ERROR,
    SUCCESS,
    NOOP,
    DEBUG,
    MORE,
    LOCAL_PTR,
    END,
    PROCESSING,
};

static char *stateNames[] = {
    "READY",
    "ERROR",
    "SUCCESS",
    "NOOP",
    "DEBUG",
    "MORE",
    "LOCAL_PTR",
    "END",
    "PROCESSING",
    NULL,
};

String *State_ToString(MemCtx *m, status state){
    String *s = String_Init(m, STRING_EXTEND);
    char *name = NULL;
    if(state == 0){
        String_AddBytes(m, s, bytes("ZERO"), strlen("ZERO"));
    }else{
        int i = 0;
        while((name = stateNames[i]) != NULL){
            if((state & states[i]) != 0){
                String_AddBytes(m, s, bytes(stateNames[i]), strlen(stateNames[i]));
                String_AddBytes(m, s, bytes(","), 1);
            }
            i++;
        }

    }
    byte upper = (byte) (state >> 8);
    String_AddBitPrint(m, s, &upper, sizeof(byte), TRUE);
    if(upper != 0){
        String_AddBytes(m, s, bytes("<<"), strlen("<<"));
        if((state & (1 << 8)) != 0){
            String_AddBytes(m, s, bytes("8,"), strlen("8,"));
        }
        if((state &(1 << 9)) != 0){
            String_AddBytes(m, s, bytes("9,"), strlen("9,"));
        }
        if((state & (1 << 10)) != 0){
            String_AddBytes(m, s, bytes("10,"), strlen("10,"));
        }
        if((state & (1 << 11)) != 0){
            String_AddBytes(m, s, bytes("11,"), strlen("11,"));
        }
        if((state & (1 << 12)) != 0){
            String_AddBytes(m, s, bytes("12,"), strlen("12,"));
        }
        if((state & (1 << 13)) != 0){
            String_AddBytes(m, s, bytes("13,"), strlen("13,"));
        }
        if((state & (1 << 14)) != 0){
            String_AddBytes(m, s, bytes("14,"), strlen("14,"));
        }
        if((state & (1 << 15)) != 0){
            String_AddBytes(m, s, bytes("15,"), strlen("15,"));
        }
        String_AddBytes(m, s, bytes(" "), 1);
    }

    return s;
}

cls Ifc_Get(cls inst){
    if(inst == TYPE_STRING_CHAIN || inst == TYPE_STRING_FIXED || inst == TYPE_STRING_FULL || inst == TYPE_STRING_SLAB){
        return TYPE_STRING;
    }else if(inst == TYPE_SPAN || inst == TYPE_QUEUE_SPAN || 
            inst == TYPE_STRING_SPAN || inst ==  TYPE_SLAB_SPAN || inst == TYPE_MINI_SPAN){
        return TYPE_SPAN;
    }

    return inst;
}

status ToStdOut(MemCtx *m, String *s, Abstract *source){
    while(s != NULL){
        printf("%s", ((char *)s->bytes));
        s = String_Next(s);
    }
    return SUCCESS;
}

boolean Ifc_Match(cls inst, cls ifc){
    if(inst == ifc){
        return TRUE;
    }

    if(ifc == TYPE_MEMCTX){
        return inst == TYPE_SPAN || inst == TYPE_REQ || inst == TYPE_SERVECTX 
            || inst == TYPE_ROEBLING;
    if(ifc == TYPE_MEMLOCAL_ITEM){
        return inst >= HTYPE_LOCAL;
    }
    }else if(ifc == TYPE_WRAPPED){
        return inst == TYPE_WRAPPED_DO || inst == TYPE_WRAPPED_UTIL || inst == TYPE_WRAPPED_FUNC || inst == TYPE_WRAPPED_PTR;
    }else if(ifc == TYPE_STRING){
        return inst == TYPE_STRING_CHAIN || inst == TYPE_STRING_FIXED || inst == TYPE_STRING_FULL;
    }else if(ifc == TYPE_STRING){
        return inst == TYPE_STRING_CHAIN || inst == TYPE_STRING_FIXED;
    }else if(ifc == TYPE_ROEBLING){
        return inst == TYPE_ROEBLING || inst == TYPE_ROEBLING_BLANK;
    }else if(ifc == TYPE_SPAN){ 
        return (inst == TYPE_SPAN || inst == TYPE_QUEUE_SPAN || 
            inst == TYPE_STRING_SPAN || inst ==  TYPE_SLAB_SPAN || inst == TYPE_MINI_SPAN ||
            inst == TYPE_TABLE || inst == TYPE_ORDERED_TABLE);
    }else if(ifc == TYPE_TRANSP){ 
        return (inst == TYPE_TRANSP || inst == TYPE_LANG_CNK);
    }else if(ifc == TYPE_TABLE){ 
        return (inst == TYPE_TABLE || inst == TYPE_ORDERED_TABLE);
    }else if(ifc == TYPE_FMT_CTX){ 
        return (inst == TYPE_FMT_CTX || inst == TYPE_LANG_CNK || inst == TYPE_LANG_CDOC);
    }

    return FALSE;
}

