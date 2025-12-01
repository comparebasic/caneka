#include <external.h>
#include "base_module.h"

word GLOBAL_flags = 0;
word NORMAL_FLAGS = 0b0000000011111111;
word UPPER_FLAGS = 0b1111111100000000;
word OUTCOME_FLAGS = (SUCCESS|NOOP|ERROR);

Buff *OutStream = NULL;
Buff *ErrStream = NULL;
Buff *RandStream = NULL;

status Core_Direct(MemCh *m, i32 out, i32 err){
    status r = READY;
    r |= Buff_SetFd(ErrStream, err);
    r |= Buff_SetFd(OutStream, out);
    ErrStream->type.state |= BUFF_UNBUFFERED;
    OutStream->type.state |= BUFF_UNBUFFERED;
    r |= Buff_Flush(ErrStream);
    r |= Buff_Flush(OutStream);
    return r;
}

status Core_Init(MemCh *m){
    status r = READY;
    if(OutStream == NULL){
        Buff *bf = Buff_Make(m, ZERO);
        r |= SUCCESS;
        OutStream = bf;
    }
    if(ErrStream == NULL){
        Buff *bf = Buff_Make(m, ZERO);
        r |= SUCCESS;
        ErrStream = bf;
    }
    if(RandStream == NULL){
        Buff *bf = Buff_Make(m, BUFF_UNBUFFERED);
        r |= SUCCESS;
        RandStream = bf;
        File_Open(bf, S(m, "/dev/random"), O_RDONLY);
    }
    return NOOP;
}

i64 Out(char *fmt, void *args[]){
    return Fmt(OutStream, fmt, args);
}
