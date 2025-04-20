#include <external.h>
#include <caneka.h>

word GLOBAL_flags = 0;
word NORMAL_FLAGS = 0b0000000011111111;
word UPPER_FLAGS = 0b1111111100000000;

Stream *OutStream = NULL;

status Core_Init(MemCh *m){
    if(OutStream == NULL){
        OutStream = Stream_MakeToFd(m, 0, NULL, 0);
        return SUCCESS;
    }
    return NOOP;
}

i64 Out(char *fmt, void **args){
    return StrVec_FmtHandle(OutStream, fmt, args);
}
