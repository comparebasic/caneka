#include <external.h>
#include "base_module.h"

static status StackEntry_Print(Buff *bf, Abstract *a, cls type, word flags){
    StackEntry *se = (StackEntry*)as(a, TYPE_DEBUG_STACK_ENTRY); 
    void *args[2];
    Str s = {
        .type = {TYPE_STR, STRING_CONST},
        .length = 0,
        .alloc = 0,
        .bytes = NULL
    };

    s.length = s.alloc = strlen(se->funcName);
    s.bytes = (byte *)se->funcName;
    args[0] = &s;
    args[1] = NULL;
    Fmt(bf, "    ^y.$", args);

    s.length = s.alloc = strlen(se->fname);
    s.bytes = (byte *)se->fname;
    args[0] = &s;
    args[1] = NULL;
    Fmt(bf, " $", args);

    byte lineNo[MAX_BASE10+1];
    byte *b = lineNo;
    byte *end = b+MAX_BASE10;
    i64 length = Str_I64OnBytes(&b, end, se->line);
    s.alloc = s.length = length;
    s.bytes = b;
    Buff_AddBytes(ErrStream, (byte *)" line ", 6);
    ToS(ErrStream, &s, s.type.of, ZERO);

    if(flags & MORE && se->ref != NULL){
        args[0] = se->ref;
        args[1] = NULL;
        Fmt(bf, " - ^0@", args);
    }

    return Fmt(bf, "^0.\n", NULL);
}

status Debug_ToSInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_DEBUG_STACK_ENTRY, (void *)StackEntry_Print);
    return r;
}
