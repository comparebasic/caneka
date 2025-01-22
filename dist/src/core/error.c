#include <external.h>
#include <caneka.h>

void *_Fatal(char *msg, cls t, char *func, char *file, int line){
    Cleanup(NULL);
    printf("\x1b[%dmFatal Error: %s - type(%s/%d) %s:%s:%d\x1b[0m\n" , COLOR_RED, msg, Class_ToString(t), t, func, file, line);
    DebugStack_Print();
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
