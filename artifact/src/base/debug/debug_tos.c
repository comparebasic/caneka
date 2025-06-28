#include <external.h>
#include <caneka.h>

static i64 StackEntry_Print(Stream *sm, Abstract *a, cls type, word flags){
    i64 total = 0;
    StackEntry *se = (StackEntry*)as(a, TYPE_DEBUG_STACK_ENTRY); 

    Stream_Bytes(sm, (byte *)"    \x1b[1;33m", 12);
    Stream_Bytes(sm, (byte *)se->funcName, strlen(se->funcName));
    Stream_Bytes(sm, (byte *)"\x1b[22m:", 6);
    Stream_Bytes(sm, (byte *)se->fname, strlen(se->fname));
    if(flags & MORE && se->ref != NULL){
        Stream_Bytes(sm, (byte *)" - \x1b[1m", 8);
        if(se->typeOf == TYPE_CSTR){
            Str *s = Str_CstrRef(sm->m, (char *)se->ref);
            ToS(sm, (Abstract *)s, s->type.of, MORE);
        }else if((flags & MORE) && se->typeOf != 0){
            ToS(sm, se->ref, se->typeOf, MORE|DEBUG);
        }
    }
    Stream_Bytes(sm, (byte *)"\x1b[0m\n", 5);

    return total;
}

status Debug_ToSInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_DEBUG_STACK_ENTRY, (void *)StackEntry_Print);
    return r;
}
