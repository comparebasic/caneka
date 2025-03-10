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
    CONTINUED,
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
    "CONTINUED",
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

status ToStdOut(MemCtx *m, String *s, Abstract *source){
    while(s != NULL){
        printf("%s", ((char *)s->bytes));
        s = String_Next(s);
    }
    return SUCCESS;
}
