#include <external.h>
#include <caneka.h>

#define COLOR_BLACK 30
#define COLOR_RED 31
#define COLOR_GREEN 32
#define COLOR_YELLOW 33
#define COLOR_BLUE 34
#define COLOR_PURPLE 35
#define COLOR_CYAN 36
#define COLOR_DARK 37

void *_Fatal(char *msg, cls t, char *func, char *file, int line){
    printf("poo: %s\n", msg);    
    return NULL;
}

void Debug_Print(void *t, cls type, char *msg, int color, boolean extended){
    printf("\x1b[%dmdebug %hu\x1b[0m", color, type);
}

void DPrint(Abstract *a, int color, char *msg, ...){
    printf("\x1b[%dm", color);
	va_list args;
    va_start(args, msg);
    vprintf(msg, args);
    Debug_Print((void *)a, 0,  "", color, TRUE);
    printf("\x1b[0m\n");
}
