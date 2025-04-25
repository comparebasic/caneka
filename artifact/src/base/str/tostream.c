#include <external.h>
#include <caneka.h>

Lookup *StreamPrintChain = NULL;

status StreamTo_Init(MemCh *m){
    if(StreamPrintChain == NULL){
        StreamPrintChain = Lookup_Make(m, _TYPE_START, NULL, NULL);
        Mem_ToStreamInit(m, StreamPrintChain);
        Str_ToStreamInit(m, StreamPrintChain);
        Sequence_ToStreamInit(m, StreamPrintChain);
        UtilDebug_Init(m, StreamPrintChain);
        return SUCCESS;
    }
    return NOOP;
}

void indent_Print(int indent){
    while(indent--){
        printf("  ");
    }
}

i64 Bits_Print(Stream *sm, byte *bt, size_t length, word flags){
    i64 total = 0;
    Single sg = {{TYPE_WRAPPED_I8, 0}, 0};
    for(int i = 0; i < length;i++){
        byte b = bt[i];
        if(flags & MORE){
            sg.val.b = b;
            Abstract *args[] = {(Abstract *)&sg, NULL};
            total += Fmt(sm, "$=", args);
        }
        for(int j = 7; j >= 0;j--){
            total += Stream_To(sm, (byte *)((b & (1 << j)) ? "1" : "0"), 1);
        }
        if(flags & MORE){
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

char *ToStreamChars(word flags){
    if(flags & DEBUG & MORE){
        return "debug-verbose";
    }else if(flags & DEBUG){
        return "debug";
    }else if(flags & MORE){
        return "verbose";
    }else {
        return "standard";
    }
}

static i64 ToStream_NotImpl(Stream *sm, Abstract *a, cls type, word flags){
    void *args[] = {
        Str_CstrRef(m, ToStreamChars(flags)),
        Str_CstrRef(m, Type_ToChars(type)),
        NULL
    };
    Fatal(FUNCNAME, FILENAME, LINENUMBER, 
        "Does not implement $ for type $", args);
    return 0;
}

i64 ToStream(Stream *sm, Abstract *a, cls type, word flags){
    if(a == NULL){
        return Stream_To(sm, (byte *)"NULL", 4);
    }

    if(type == 0){
        a = (Abstract *)t;
        type = a->type.of;
    }

    StreamPrintFunc func = (StreamPrintFunc)Lookup_Get(StreamPrintChain, type);
    if(func != NULL){
        return func(sm, a, type, flag);
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

