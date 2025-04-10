#include <external.h>
#include <caneka.h>

static boolean crashing = FALSE;

void *Fatal(i32 fd, char *func, char *file, int line, char *fmt, ...){
	va_list args;
    va_start(args, fmt);
#ifdef CLI    
    RawMode(FALSE);
#endif
    StrVec_Fmt(DebugOut, "^r.Fatal Error: _c:_c:_i4 ^D", func, file, line);
    StrVec_FmtHandle(DebugOut, fmt, args);
    StrVec_Fmt(DebugOut, "^d");
#ifdef OPENSSL
    char _buff[256];
    unsigned long e = ERR_get_error();
    if(e != 0){
        char *openssl_err = ERR_error_string(e, _buff);
        StrVec_Fmt(DebugOut, "^rD^_T^0");
        StrVec_Fmt(DebugOut, "  ^rD^_T^0", openssl_err, TYPE_CSTR);
    }
#endif
    Str_ToFd(NL, 0);
    if(!crashing){
        crashing = TRUE;
        DebugStack_Print();
    }
    exit(13);
    return NULL;
}

void *Error(char *msg){
    printf("Error: %s\n", msg);
    return NULL;
}

void LogError(char *msg, ...){
	va_list args;
    va_start(args, msg);
    if((GLOBAL_flags & NO_COLOR) == 0){
        printf("\x1b[31m");
    }
    vprintf(msg, args);
    if((GLOBAL_flags & NO_COLOR) == 0){
        printf("\x1b[0m");
    }
    printf("\n");
}

void ExitError(int code, char *msg, ...){
	va_list args;
    va_start(args, msg);
    if((GLOBAL_flags & NO_COLOR) == 0){
        printf("\x1b[31m");
    }
    vprintf(msg, args);
    if((GLOBAL_flags & NO_COLOR) == 0){
        printf("\x1b[0m");
    }
    printf("\n");
    exit(code);
}
