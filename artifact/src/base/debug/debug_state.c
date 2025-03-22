#include <external.h>
#include <caneka.h>

static word states[] = {
    READY,
    ERROR,
    SUCCESS,
    NOOP,
    DEBUG,
    MORE,
    _,
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
    "_",
    "END",
    "PROCESSING",
    NULL,
};

Str *State_ToStr(MemCtx *m, status state){
    Str *s = Str_Make(m, STR_DEFAULT);
    char *name = NULL;
    if(state == 0){
        Str_AddCstr(s, "ZERO");
    }else{
        int i = 0;
        while((name = stateNames[i]) != NULL){
            if((state & states[i]) != 0){
                Str_AddCstr(s, stateNames[i]);
                Str_AddCstr(s, ",");
            }
            i++;
        }

    }
    byte upper = (byte) (state >> 8);
    /*
    String_AddBitPrint(m, s, &upper, sizeof(byte), TRUE);
    */
    if(upper != 0){
        Str_AddCstr(s, "<<");
        if((state & (1 << 8)) != 0){
            Str_AddCstr(s, "8,");
        }
        if((state &(1 << 9)) != 0){
            Str_AddCstr(s, "9,");
        }
        if((state & (1 << 10)) != 0){
            Str_AddCstr(s, "10,");
        }
        if((state & (1 << 11)) != 0){
            Str_AddCstr(s, "11,");
        }
        if((state & (1 << 12)) != 0){
            Str_AddCstr(s, "12,");
        }
        if((state & (1 << 13)) != 0){
            Str_AddCstr(s, "13,");
        }
        if((state & (1 << 14)) != 0){
            Str_AddCstr(s, "14,");
        }
        if((state & (1 << 15)) != 0){
            Str_AddCstr(s, "15,");
        }
        Str_AddCstr(s, " ");
    }

    return s;
}
