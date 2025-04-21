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
        SequenceDebug_Init(m, DebugPrintChain);
        /*
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

i64 Bits_Print(Stream *sm, byte *bt, size_t length, boolean extended){
    i64 total = 0;
    for(int i = length-1; i >= 0;i--){
        byte b = bt[i];
        if(extended){
            void *args[] = {&b, NULL};
            total += StrVec_Fmt(sm, "_i1=", args);
        }
        for(int j = 7; j >= 0;j--){
            total += Stream_To(sm, (byte *)((b & (1 << j)) ? "1" : "0"), 1);
        }
        if(extended){
            total += Stream_To(sm, (byte *)" ", 1);
        }
    }
    return total;
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
        void *args[] = {Type_ToChars(type), NULL};
        StrVec_Fmt(DebugOut, "_c: unknown-debug", args);
        return 0;
    }
}

