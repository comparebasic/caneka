#include <external.h>
#include <caneka.h>

MemCtx *DebugM = NULL;

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


void Debug_Print(void *t, cls type, char *msg, int color, boolean extended){
    if(type == 0 && t != NULL){
        type = ((Abstract *)t)->type.of;
    }
    if(Ifc_Match(type, TYPE_CSTR)){
        printf("\x1b[%dm%s%s", color, msg, (char *)t);
    }else if(Ifc_Match(type, TYPE_STRING)){
        printf("\x1b[%dm%s", color, msg);
        String *s = (String *)t;
        while(s != NULL){
            printf("%s", s->bytes);
            s = String_Next(s);
        }
        printf("\x1b[0m");
    }else if(Ifc_Match(type, TYPE_SPAN)){
        if(extended){
            printf("\x1b[%dm%sT<%s:%s nvalues:%d items: <", color, msg, Class_ToString(type), 
                t != NULL ? State_ToChars(((Abstract *)t)->type.state): "NULL", 
                ((Span *)t)->nvalues);
        }else{
            printf("\x1b[%dm%s", color, msg);
        }
        Iter it;
        Iter_Init(&it, (Span *)t);
        while((Iter_Next(&it) & END) == 0){
            Abstract *a = Iter_Get(&it);
            if(a != NULL){
                Debug_Print((void *)a, 0, "", color, extended);
            }
            if((it.type.state & FLAG_ITER_LAST) == 0){
                printf("\x1b[0m, \x1b[%d", color);
            }
        }
        if(extended){
            printf("\x1b[%dm>>\x1b[0m", color);
        }
    }else{
        printf("\x1b[%dm%sT<%s:%s", color, msg, Class_ToString(type), 
            t != NULL ? State_ToChars(((Abstract *)t)->type.of): "NULL");
        printf(">\x1b[0m");
    }
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
