#include <external.h>
#include <caneka.h>

Lookup *ToStreamLookup = NULL;
Lookup *ToSFlagLookup = NULL;
Str **stateLabels = NULL;

status StreamTo_Init(MemCh *m){
    status r = READY;
    if(ToStreamLookup == NULL){
        ToStreamLookup = Lookup_Make(m, _TYPE_ZERO, NULL, NULL);
        ToSFlagLookup = Lookup_Make(m, _TYPE_ZERO, NULL, NULL);
        Mem_ToSInit(m, ToStreamLookup);
        Str_ToSInit(m, ToStreamLookup);
        Sequence_ToSInit(m, ToStreamLookup);
        Util_ToSInit(m, ToStreamLookup);
        r |= SUCCESS;
    }
    if(stateLabels == NULL){
        stateLabels = (Str **)Arr_Make(m, 17);
        stateLabels[0] = Str_CstrRef(m, "ZERO/READY");
        stateLabels[1] = Str_CstrRef(m, "SUCCESS");
        stateLabels[2] = Str_CstrRef(m, "ERROR");
        stateLabels[3] = Str_CstrRef(m, "NOOP");
        stateLabels[4] = Str_CstrRef(m, "DEBUG");
        stateLabels[5] = Str_CstrRef(m, "MORE");
        stateLabels[6] = Str_CstrRef(m, "CONTINUE");
        stateLabels[7] = Str_CstrRef(m, "END");
        stateLabels[8] = Str_CstrRef(m, "PROCESSING");
        stateLabels[9] = Str_CstrRef(m, "CLS_FLAG_ALPHA");
        stateLabels[10] = Str_CstrRef(m, "CLS_FLAG_BRAVO");
        stateLabels[11] = Str_CstrRef(m, "CLS_FLAG_CHARLIE");
        stateLabels[12] = Str_CstrRef(m, "CLS_FLAG_DELTA");
        stateLabels[13] = Str_CstrRef(m, "CLS_FLAG_ECHO");
        stateLabels[14] = Str_CstrRef(m, "CLS_FLAG_FOXTROT");
        stateLabels[15] = Str_CstrRef(m, "CLS_FLAG_GOLF");
        stateLabels[16] = Str_CstrRef(m, "CLS_FLAG_HOTEL");
        r |= SUCCESS;
    }

    if(r == READY){
        r |= NOOP;
    }

    return r;
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
        if(b == 0 && (flags & MORE)){
            total += Stream_Bytes(sm, (byte *)"0", 1);
        }else{
            if(flags & MORE){
                sg.val.b = b;
                Abstract *args[] = {(Abstract *)&sg, NULL};
                total += Fmt(sm, "$=", args);
            }
            for(int j = 7; j >= 0;j--){
                total += Stream_Bytes(sm, (byte *)((b & (1 << j)) ? "1" : "0"), 1);
            }
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

i64 ToS_FlagLabels(Stream *sm, Abstract *a){
    Str **labels = Lookup_Get(ToSFlagLookup, a->type.of);
    Str *lbl;
    i64 total = 0;
    boolean first = TRUE;
    word flags = a->type.state;
    if(flags == 0){
        if(labels != NULL && labels[0] != NULL){
            lbl = labels[0];
        }else{
            lbl = stateLabels[0];
        }
        total += Stream_Bytes(sm, lbl->bytes, lbl->length);
    }else{
        for(i32 i = 0; i < 16; i++){ 
           if((flags & (1 << i)) != 0){
                if(first){
                    first = FALSE;
                }else{
                    total += Stream_Bytes(sm, (byte *)"|", 1);
                }
                if(labels != NULL && labels[i+1] != NULL){
                    lbl = labels[i+1];
                }else{
                    lbl = stateLabels[i+1];
                }
                total += Stream_Bytes(sm, lbl->bytes, lbl->length);
           }
        }
    }
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
    Error(sm->m, a, FUNCNAME, FILENAME, LINENUMBER, 
        "Does not implement $ for type $", args);
    return 0;
}

i64 ToS(Stream *sm, Abstract *a, cls type, word flags){
    DebugStack_Push(a, type);
    if(a == NULL){
        DebugStack_Pop();
        return Stream_Bytes(sm, (byte *)"NULL", 4);
    }

    if(type == 0){
        type = a->type.of;
    }

    ToSFunc func = (ToSFunc)Lookup_Get(ToStreamLookup, type);
    if(func != NULL){
        i64 total = func(sm, a, type, flags);
        DebugStack_Pop();
        return total;
    }else{
        Abstract *args[] = {
            (Abstract *)Str_CstrRef(sm->m, Type_ToChars(type)),
            (Abstract *)I16_Wrapped(sm->m, type),
            NULL
        };
        Fmt(sm, "$/$: unknown-debug", args);
        DebugStack_Pop();
        return 0;
    }
}

