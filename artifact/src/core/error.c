#include <external.h>
#include <caneka.h>

static boolean crashing = FALSE;

void *_Fatal(char *msg, cls t, char *func, char *file, int line){
#ifdef CLI    
    RawMode(FALSE);
#endif
    printf("\x1b[%dmFatal Error: \x1b[1;%dm%s\x1b[0;%dm - type(%s/%d) %s:%s:%d\x1b[0m\n" , COLOR_RED, COLOR_RED, msg, COLOR_RED, Class_ToString(t), t, func, file, line);
#ifdef OPENSSL
    char _buff[256];
    unsigned long e = ERR_get_error();
    if(e != 0){
        char *openssl_err = ERR_error_string(e, _buff);
        printf("  \x1b[%dm%s\x1b[0m", COLOR_RED, openssl_err);
    }
#endif
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
