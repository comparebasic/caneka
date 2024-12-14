#include <external.h>
#include <caneka.h>

void *_Fatal(char *msg, cls t, char *func, char *file, int line){
    
#ifdef DEBUG_STACK
    printf("\x1b[%dmFatal: %s\x1b[0m\n\n", COLOR_RED, msg);
    DebugStack_Print();
#else
    printf("Fatal Error: %s - type(%s) %s:%s:%d\n" , msg, Class_ToString(t), func, file, line);
#endif
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
    if(!HasFlag(GLOBAL_flags, NO_COLOR)){
        printf("\x1b[31m");
    }
    vprintf(msg, args);
    if(!HasFlag(GLOBAL_flags, NO_COLOR)){
        printf("\x1b[0m");
    }
    printf("\n");
}

void ExitError(int code, char *msg, ...){
	va_list args;
    va_start(args, msg);
    if(!HasFlag(GLOBAL_flags, NO_COLOR)){
        printf("\x1b[31m");
    }
    vprintf(msg, args);
    if(!HasFlag(GLOBAL_flags, NO_COLOR)){
        printf("\x1b[0m");
    }
    printf("\n");
    exit(code);
}
