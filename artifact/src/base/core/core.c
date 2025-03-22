#include <external.h>
#include <caneka.h>

word GLOBAL_flags = 0;
word NORMAL_FLAGS = 0b0000000011111111;
word UPPER_FLAGS = 0b1111111100000000;

i64 Out(MemCtx *m, char *fmt, ...){
	va_list args;
    va_start(args, fmt);
    return StrVec_FmtHandle(m, NULL, fmt, args, 0);
}
