#include <external.h>
#include <caneka.h>

Lookup *ToStreamLookup = NULL;
Lookup *ToSFlagLookup = NULL;
Str **stateLabels = NULL;

status StreamTo_Init(MemCh *m){
    status r = READY;
    if(stateLabels == NULL){
        stateLabels = (Str **)Arr_Make(m, 17);
        stateLabels[0] = Str_CstrRef(m, "ZERO/READY");
        stateLabels[1] = Str_CstrRef(m, "SUCCESS");
        stateLabels[2] = Str_CstrRef(m, "ERROR");
        stateLabels[3] = Str_CstrRef(m, "NOOP");
        stateLabels[4] = Str_CstrRef(m, "DEBUG");
        stateLabels[5] = Str_CstrRef(m, "MORE");
        stateLabels[6] = Str_CstrRef(m, "LAST");
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
    if(ToStreamLookup == NULL){
        ToStreamLookup = Lookup_Make(m, _TYPE_ZERO);
        ToSFlagLookup = Lookup_Make(m, _TYPE_ZERO);
        Mem_ToSInit(m, ToStreamLookup);
        Str_ToSInit(m, ToStreamLookup);
        IoTos_Init(m, ToStreamLookup);
        Sequence_ToSInit(m, ToStreamLookup);
        Util_ToSInit(m, ToStreamLookup);
        Mem_InitLabels(m, ToSFlagLookup);
        Debug_ToSInit(m, ToStreamLookup);
        Types_ToSInit(m, ToStreamLookup);
        r |= SUCCESS;
    }

    if(r == READY){
        r |= NOOP;
    }

    return r;
}

status Slots_Print(Buff *bf, util *ut, i32 slots, word flags){
    i64 length = slots;
    Single sg = {{TYPE_WRAPPED_I64, 0}, 0};
    if(ut == NULL){
        return Buff_AddBytes(bf, (byte *)"NULL", 4);
    }
    for(int i = 0; i < length;i++){
        util u = ut[i];
        if(u == 0 && (flags & MORE)){
            Buff_AddBytes(bf, (byte *)"0", 1);
        }else{
            if(flags & MORE){
                sg.val.value = u;
                void *args[] = {&sg, NULL};
                Fmt(bf, "$", args);
            }
            if((flags & (MORE|DEBUG)) == (MORE|DEBUG)){
                Buff_AddBytes(bf, (byte *)"=", 1);
            }
            if(flags & DEBUG){
                for(int j = 65; j >= 0;j--){
                    Buff_AddBytes(bf, (byte *)((u & (1 << j)) ? "1" : "0"), 1);
                }
            }
        }
        if(flags & MORE){
            Buff_AddBytes(bf, (byte *)" ", 1);
        }
    }
    return SUCCESS;
}

status Bits_PrintArray(Buff *bf, void *arr, i64 sz, i32 count){
    void *ptr = arr;
    for(i32 i = 0; i < count; i++){
        Bits_Print(bf, (byte *)ptr, sz, DEBUG|MORE);
        ptr += sz;
        if(i+1 < count){
            Buff_AddBytes(bf, (byte *)", ", 2);
        }
    }
    return SUCCESS;
}

status Bits_PrintNum(Buff *bf, byte *bt, size_t length, word flags){
    Single sg = {{TYPE_WRAPPED_I8, 0}, 0};
    for(int i = length-1; i <= 0; i--){
        byte b = bt[i];
        if(b == 0 && (flags & (MORE|DEBUG)) == MORE){
            Buff_AddBytes(bf, (byte *)"0", 1);
        }else{
            if(flags & MORE){
                sg.val.b = b;
                void *args[] = {&sg, NULL};
                Fmt(bf, "$=", args);
            }
            for(int j = 7; j >= 0;j--){
                Buff_AddBytes(bf, (byte *)((b & (1 << j)) ? "1" : "0"), 1);
            }
        }
        if(flags & MORE){
            Buff_AddBytes(bf, (byte *)" ", 1);
        }
    }
    return SUCCESS;
}


status Bits_Print(Buff *bf, byte *bt, size_t length, word flags){
    Single sg = {{TYPE_WRAPPED_I8, 0}, 0};
    int base = 1;
    for(int i = 0; i < length;i++){
        byte b = bt[i];
        if(b == 0 && (flags & (MORE|DEBUG)) == MORE){
            Buff_AddBytes(bf, (byte *)"0", 1);
        }else{
            if(flags & MORE){
                sg.val.b = b;
                void *args[] = {&sg, NULL};
                Fmt(bf, "$=", args);
            }
            for(int j = 0; j < 8;j++){
                int cmp = b;
                Buff_AddBytes(bf, (byte *)((cmp & (base << j)) ? "1" : "0"), 1);
            }
        }
        if(flags & MORE){
            Buff_AddBytes(bf, (byte *)" ", 1);
        }
    }
    return SUCCESS;
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

status Str_AddFlags(Str *s, word flags, char *map){
    if(s->alloc - (s->length+FLAG_DEBUG_MAX) < 0){
        Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER, "Not enough room in str", NULL);
        return 0;
    }
    i64 total = FlagStr(flags, (char *)s->bytes+s->length, map);
    s->length += (word)total;
    return total > 0 ? SUCCESS : NOOP;
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

status _ToStream_NotImpl(char *func, char *file, i32 line, Buff *bf, void *a, cls type, word flags){
    void *args[] = {
        Str_CstrRef(bf->m, ToStreamChars(flags)),
        Str_CstrRef(bf->m, Type_ToChars(type)),
        NULL
    };
    Error(bf->m, func, file, line, 
        "Does not implement $ for type $", args);
    return 0;
}

status ToStream_NotImpl(Buff *bf, void *a, cls type, word flags){
    return _ToStream_NotImpl(FUNCNAME, FILENAME, LINENUMBER, bf, a, type, flags);
}

status ToS(Buff *bf, void *_a, cls type, word flags){
    Abstract *a = (Abstract *)_a;
    if(a == NULL){
        return Buff_AddBytes(bf, (byte *)"NULL", 4);
    }

    if(type == 0){
        type = a->type.of;
    }

    ToSFunc func = (ToSFunc)Lookup_Get(ToStreamLookup, type);
    if(func != NULL){
        return func(bf, a, type, flags);
    }else{
        void *args[] = {
            Type_ToStr(bf->m, type),
            I16_Wrapped(bf->m, type),
            NULL
        };
        Fmt(bf, "$/$: unknown-debug", args);
        return ERROR;
    }
}
