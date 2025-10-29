#include <external.h>
#include <caneka.h>

word GLOBAL_flags = 0;
word NORMAL_FLAGS = 0b0000000011111111;
word UPPER_FLAGS = 0b1111111100000000;
word OUTCOME_FLAGS = (SUCCESS|NOOP|ERROR);

Buff *OutStream = NULL;
Buff *ErrStream = NULL;

status Core_Init(MemCh *m){
    status r = READY;
    if(OutStream == NULL){
        Buff *bf = Buff_Make(m, BUFF_UNBUFFERED);
        Buff_SetFd(bf, 1);
        r |= SUCCESS;
        OutStream = bf;
    }
    if(ErrStream == NULL){
        Buff *bf = Buff_Make(m, BUFF_UNBUFFERED);
        Buff_SetFd(bf, 2);
        r |= SUCCESS;
        ErrStream = bf;
    }
    r |= Error_Init(m);
    return NOOP;
}

i64 Out(char *fmt, Abstract *args[]){
    return Fmt(OutStream, fmt, args);
}

i64 Debug(char *fmt, Abstract *args[]){
    i64 total = Fmt(OutStream, fmt, args);
    return total;
}
