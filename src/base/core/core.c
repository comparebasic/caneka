#include <external.h>
#include <caneka.h>

word GLOBAL_flags = 0;
word NORMAL_FLAGS = 0b0000000011111111;
word UPPER_FLAGS = 0b1111111100000000;
word OUTCOME_FLAGS = (SUCCESS|NOOP|ERROR);

Buff *OutStream = NULL;
Buff *ErrStream = NULL;

status Stream_Init(MemCh *m, i32 out, i32 err){
    status r = READY;
    if(OutStream == NULL){
        Buff *bf = Buff_Make(m, BUFF_UNBUFFERED);
        Buff_SetFd(bf, out);
        r |= SUCCESS;
        OutStream = bf;
    }
    if(ErrStream == NULL){
        Buff *bf = Buff_Make(m, BUFF_UNBUFFERED);
        Buff_SetFd(bf, err);
        r |= SUCCESS;
        ErrStream = bf;
    }
    return NOOP;
}

i64 Out(char *fmt, void *args[]){
    return Fmt(OutStream, fmt, args);
}
