#include <external.h>
#include <caneka.h>

MemCtx *DebugM = NULL;

void Debug_Print(void *t, cls type, char *msg, int color, boolean extended){
    printf("\x1b[%dm%sT<%s:%s>\x1b[0m", color, msg, Class_ToString(type), 
        t != NULL ? State_ToChars(((Abstract *)t)->type.of): "NULL");
}

void DPrint(Abstract *a, int color, char *msg, ...){
    DebugStack_Push(msg, TYPE_CSTR);
    printf("\x1b[%dm", color);
	va_list args;
    va_start(args, msg);
    vprintf(msg, args);
    Debug_Print((void *)a, 0,  "", color, TRUE);
    printf("\x1b[0m\n");
    DebugStack_Pop();
}

status Debug_Init(MemCtx *m){
    DebugM = m;
    return NOOP;
}
