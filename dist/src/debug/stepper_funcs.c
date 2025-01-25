#include <external.h>
#include <caneka.h>

Abstract *Roebling_StepDesc(MemCtx *m, Abstract *a, Abstract *b){
    DebugStack_Push(NULL, 0);
    Roebling *rbl = (Roebling *)asIfc(a, TYPE_ROEBLING);
    String *s = (String *)asIfc(b, TYPE_STRING);
    Cursor *cur = &rbl->cursor;
    String_Reset(s);
    char *cstr = "Rbl at:";
    String_AddBytes(m, s, bytes(cstr), strlen(cstr));
    String_AddInt(m, s, cur->offset);
    String_AddBytes(m, s, bytes("+"), 1);
    String_AddInt(m, s, cur->local);
    String_AddBytes(m, s, bytes("("), 1);
    String_AddInt(m, s, Cursor_Total(cur));
    String_AddBytes(m, s, bytes(") "), 2);
    if(cur->ptr != NULL){
        String_AddBytes(m, s, bytes("'"), 1);
        String_AddBytes(m, s, cur->ptr, 1);
        String_AddBytes(m, s, bytes("'"), 1);
    }else{
        String_AddBytes(m, s, bytes("NULL"), 4);
    }

    DebugStack_Pop();
    return (Abstract *)s;
}

Exch Stepper_GetDescFunc(word type){
    if(Ifc_Match(type, TYPE_ROEBLING)){
        return Roebling_StepDesc;
    }else{
        return NULL;
    }
}
