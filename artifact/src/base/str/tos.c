#include <external.h>
#include <caneka.h>

Lookup *ToStreamLookup = NULL;

status StreamTo_Init(MemCh *m){
    if(ToStreamLookup == NULL){
        ToStreamLookup = Lookup_Make(m, _TYPE_START, NULL, NULL);
        Mem_ToSInit(m, ToStreamLookup);
        Str_ToSInit(m, ToStreamLookup);
        Sequence_ToSInit(m, ToStreamLookup);
        Util_ToSInit(m, ToStreamLookup);
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
            total += Stream_Bytes(sm, (byte *)((b & (1 << j)) ? "1" : "0"), 1);
        }
        if(flags & MORE){
            total += Stream_Bytes(sm, (byte *)" ", 1);
        }
    }
    return total;
}

i64 FlagStr(word flag, char *dest, char *map){
    i64 p = 0;
    i32 i = 0;
    if(flag == 0){
        dest[p++] = map[0];
    }else{
        for(i32 i = 0; i < 16; i++){ 
           if((flag & (1 << i)) != 0){
                dest[p++] = map[i+1];
           }
        }
    }
    dest[p] = '\0';
    return p;
}

i64 Str_AddFlags(Str *s, word flags, char *map){
    if(s->alloc - (s->length+FLAG_DEBUG_MAX) < 0){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Not enough room in str", NULL);
        return 0;
    }
    i64 total = FlagStr(flags, (char *)s->bytes+s->length, map);
    s->length += (word)total;
    return total;
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

i64 ToStream_NotImpl(Stream *sm, Abstract *a, cls type, word flags){
    Abstract *args[] = {
        (Abstract *)Str_CstrRef(sm->m, ToStreamChars(flags)),
        (Abstract *)Str_CstrRef(sm->m, Type_ToChars(type)),
        NULL
    };
    Fatal(FUNCNAME, FILENAME, LINENUMBER, 
        "Does not implement $ for type $", args);
    return 0;
}

i64 ToS(Stream *sm, Abstract *a, cls type, word flags){
    if(a == NULL){
        return Stream_Bytes(sm, (byte *)"NULL", 4);
    }

    if(type == 0){
        type = a->type.of;
    }

    ToSFunc func = (ToSFunc)Lookup_Get(ToStreamLookup, type);
    if(func != NULL){
        return func(sm, a, type, flags);
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

