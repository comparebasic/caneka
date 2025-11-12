#include <external.h>
#include <caneka.h>

static status StackEntry_Print(Buff *bf, Abstract *a, cls type, word flags){
    StackEntry *se = (StackEntry*)as(a, TYPE_DEBUG_STACK_ENTRY); 

    Buff_AddBytes(bf, (byte *)"    \x1b[1;33m", 11);
    Buff_AddBytes(bf, (byte *)se->funcName, strlen(se->funcName));
    Buff_AddBytes(bf, (byte *)"\x1b[22m:", 6);
    Buff_AddBytes(bf, (byte *)se->fname, strlen(se->fname));
    if(flags & MORE && se->ref != NULL){
        Buff_AddBytes(bf, (byte *)" - \x1b[0;1m", 9);
        ToS(bf, se->ref, se->typeOf, MORE);
    }

    return Buff_AddBytes(bf, (byte *)"\x1b[0m\n", 5);
}

status Debug_ToSInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_DEBUG_STACK_ENTRY, (void *)StackEntry_Print);
    return r;
}
