#include <external.h>
#include <caneka.h>

Lookup *DebugPrintChain = NULL;

MemCtx *_debugM = NULL;

#include "inline/handle_io.c"

status Debug_Init(MemCtx *m){
    if(_debugM == NULL){
        _debugM = m;
    }
    if(DebugPrintChain == NULL){
        DebugPrintChain = Lookup_Make(m, _TYPE_START, NULL, NULL);
        Mem_DebugInit(m, DebugPrintChain);
        Str_DebugInit(m, DebugPrintChain);
        /*
        StringDebug_Init(m, funcs);
        SequenceDebug_Init(m, funcs);
        UtilDebug_Init(m, funcs);
        ParserDebug_Init(m, funcs);
        */
        /* todo add other debug inits here */
        return SUCCESS;
    }
    m->type.range++;
    return NOOP;
}

void indent_Print(int indent){
    while(indent--){
        printf("  ");
    }
}

void Bits_Print(byte *bt, int length, char *msg, int color, boolean extended){
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

i64 Str_Debug(MemCtx *m, StrVec *v, i32 fd, void *t, cls type, boolean extended){
    if(t == NULL){
        return handleIo(v, fd, Str_Ref(m, (byte *)"NULL", 4, 4));
    }

    Abstract *a = (Abstract *)t;
    if(type == 0){
        a = (Abstract *)t;
        type = a->type.of;
    }

    DebugPrintFunc func = (DebugPrintFunc)Lookup_Get(DebugPrintChain, type);
    if(func != NULL){
        return func(m, v, fd, a, type, extended);
    }else{
        Out(m, "_c:_c unkown_debug(_a)", Type_ToChars(type), t);
        return 0;
    }
}

