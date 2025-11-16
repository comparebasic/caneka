#include <external.h>
#include <caneka.h>

static status StackEntry_Print(Buff *bf, Abstract *a, cls type, word flags){
    StackEntry *se = (StackEntry*)as(a, TYPE_DEBUG_STACK_ENTRY); 

    if(Ansi_HasColor()){
        Buff_AddBytes(bf, (byte *)"    \x1b[1;33m", 11);
    }else{
        Buff_AddBytes(bf, (byte *)"    ", 4);
    }
    Buff_AddBytes(bf, (byte *)se->funcName, strlen(se->funcName));
    if(Ansi_HasColor()){
        Buff_AddBytes(bf, (byte *)"\x1b[22m:", 6);
    }
    Buff_AddBytes(bf, (byte *)se->fname, strlen(se->fname));
    if(flags & MORE && se->ref != NULL){
        if(Ansi_HasColor()){
            Buff_AddBytes(bf, (byte *)" - \x1b[0;1m", 9);
        }else{
            Buff_AddBytes(bf, (byte *)" - ", 3);
        }
        ToS(bf, se->ref, se->typeOf, MORE);
    }

    if(Ansi_HasColor()){
        Buff_AddBytes(bf, (byte *)"\x1b[0m\n", 5);
    }
    return SUCCESS;
}

status Debug_ToSInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_DEBUG_STACK_ENTRY, (void *)StackEntry_Print);
    return r;
}
