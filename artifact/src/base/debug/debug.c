#include <external.h>
#include <caneka.h>

Lookup *DebugPrintChain = NULL;
Stream *DebugOut;

MemCh *_debugM = NULL;

status Debug_Init(MemCh *m){
    if(_debugM == NULL){
        _debugM = m;
    }
    if(DebugPrintChain == NULL){
        DebugPrintChain = Lookup_Make(m, _TYPE_START, NULL, NULL);
        Mem_DebugInit(m, DebugPrintChain);
        Str_DebugInit(m, DebugPrintChain);
        /*
        SequenceDebug_Init(m, funcs);
        UtilDebug_Init(m, funcs);
        ParserDebug_Init(m, funcs);
        */
        /* todo add other debug inits here */
        return SUCCESS;
    }
    if(DebugOut == NULL){
        DebugOut = Stream_MakeToFd(m, 0, NULL, 0);
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

i64 Str_Debug(Stream *sm, void *t, cls type, boolean extended){
    if(t == NULL){
        return Stream_To(sm, (byte *)"NULL", 4);
    }

    Abstract *a = (Abstract *)t;
    if(type == 0){
        a = (Abstract *)t;
        type = a->type.of;
    }

    DebugPrintFunc func = (DebugPrintFunc)Lookup_Get(DebugPrintChain, type);
    if(func != NULL){
        return func(sm, a, type, extended);
    }else{
        StrVec_Fmt(DebugOut, "_c: unkown_debug", Type_ToChars(type));
        return 0;
    }
}

