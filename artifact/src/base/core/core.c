#include <external.h>
#include <caneka.h>

word GLOBAL_flags = 0;
word NORMAL_FLAGS = 0b0000000011111111;
word UPPER_FLAGS = 0b1111111100000000;
word OUTCOME_FLAGS = (SUCCESS|NOOP|ERROR);

Stream *OutStream = NULL;
Stream *ErrStream = NULL;

status Core_Init(MemCh *m){
    status r = READY;
    if(OutStream == NULL){
        OutStream = Stream_MakeToFd(m, 1, NULL, 0);
        r |= SUCCESS;
    }
    if(ErrStream == NULL){
        ErrStream = Stream_MakeToFd(m, 2, NULL, 0);
        r |= SUCCESS;
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
