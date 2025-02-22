#include <external.h>
#include <caneka.h>

Chain *DebugPrintChain = NULL;

MemCtx *DebugM = NULL;

status Debug_Init(MemCtx *m){
    DebugM = m;
    m->type.range++;
    if(DebugPrintChain == NULL){
        Lookup *funcs = Lookup_Make(m, _TYPE_START, NULL, NULL);
        DebugPrintChain = Chain_Make(m, funcs);
        StringDebug_Init(m, funcs);
        SequenceDebug_Init(m, funcs);
        /* todo add other debug inits here */
        return SUCCESS;
    }

    return NOOP;
}

void indent_Print(int indent){
    while(indent--){
        printf("  ");
    }
}

void Debug_Print(void *t, cls type, char *msg, int color, boolean extended){
    if(color >= 0){
        printf("\x1b[%dm", color);
    }

    if(t == NULL){
        printf("%s\x1b[%dmNULL\x1b[0m", msg, color);
        return;
    }

    Abstract *a = (Abstract *)t;
    if(type == 0){
        a = (Abstract *)t;
        type = a->type.of;
    }

    DebugPrintFunc func = (DebugPrintFunc)Chain_Get(DebugPrintChain, type);
    if(func != NULL){
        return func(a, type, msg, color, extended);
    }else{
        printf("type: %hu\n", type);
        printf("%s:%s unkown_debug(%p)", msg, Class_ToString(type), t);
    }

    if(color >= 0){
        printf("\x1b[0m");
    }
    fflush(stdout);
}

void Bits_Print(byte *bt, int length, char *msg, int color, boolean extended){
    printf("\x1b[%dm%s", color, msg);
    for(int i = length-1; i >= 0;i--){
        byte b = bt[i];
        if(extended){
            printf("%03hu=", b);
        }
        for(int j = 7; j >= 0;j--){
            printf("%c", (b & (1 << j)) ? '1' : '0');
        }
        if(extended){
            printf(" ");
        }
    }
    printf("\x1b[0m");
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
