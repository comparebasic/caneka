#include <external.h>
#include <caneka.h>

Lookup *DebugPrintChain = NULL;
Stream *DebugOut = NULL;

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
        UtilDebug_Init(m, DebugPrintChain);
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
    Single sg = {{TYPE_WRAPPED_I8, 0}, 0};
    for(int i = 0; i < length;i++){
        byte b = bt[i];
        if(extended){
            sg.val.b = b;
            Abstract *args[] = {(Abstract *)&sg, NULL};
            total += Fmt(sm, "$=", args);
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

i64 FlagStr(word flag, char *dest, char *map){
    i64 p = 0;
    i32 i = 0;
    for(i32 i = 0; i < 16; i++){ 
       if((flag & (1 << i)) != 0){
            dest[p++] = map[i+1];
       }
    }
    dest[p] = '\0';
    return p;
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
        Abstract *args[] = {
            (Abstract *)Str_CstrRef(sm->m, Type_ToChars(type)),
            (Abstract *)I16_Wrapped(sm->m, type),
            NULL
        };
        Fmt(sm, "$/$: unknown-debug", args);
        return 0;
    }
}

