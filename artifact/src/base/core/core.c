#include <external.h>
#include <caneka.h>

word GLOBAL_flags = 0;
word NORMAL_FLAGS = 0b0000000011111111;
word UPPER_FLAGS = 0b1111111100000000;

Stream *OutStream = NULL;
Stream *ErrStream = NULL;

status Core_Init(MemCh *m){
    status r = READY;
    if(OutStream == NULL){
        OutStream = Stream_MakeToFd(m, 0, NULL, 0);
        r |= SUCCESS;
    }
    if(ErrStream == NULL){
        ErrStream = Stream_MakeToFd(m, 0, NULL, 0);
        r |= SUCCESS;
    }
    return NOOP;
}

i64 Out(char *fmt, void **args){
    return StrVec_FmtHandle(OutStream, fmt, args);
}
