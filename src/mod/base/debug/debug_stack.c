#include <external.h>
#include "base_module.h"

static Iter _it;

void _DebugStack_Push(char *cstr, char *fname, void *ref, word typeOf, i32 line, i32 pos){
    Span *stack = _it.p;

    word fl = stack->m->type.state;
    stack->m->type.state |= MEMCH_BASE;

    StackEntry *entry = (StackEntry *)Iter_GetByIdx(&_it, _it.idx+1);
    if(entry == NULL){
        entry = MemCh_Alloc(stack->m, sizeof(StackEntry));
        entry->type.of = TYPE_DEBUG_STACK_ENTRY;
        Iter_Add(&_it, entry);
    }

    entry->funcName = cstr;
    entry->fname = fname;
    entry->ref = NULL; /* TODO: when I can ensure the ref is copied to safe memory we can use this again */
    entry->typeOf = typeOf;
    entry->line = line;
    entry->pos = pos;
    stack->m->type.state = fl;
}

void DebugStack_Pop(){
    if(_it.idx > 0){
        Iter_GetByIdx(&_it, _it.idx-1);
    }
}

void DebugStack_SetRef(void *v, word typeOf){
    StackEntry *entry = Iter_Current(&_it);
    if(entry == NULL){
        return;
    }
    entry->ref = v;
    entry->typeOf = typeOf;
}

StackEntry *DebugStack_Get(){
    return (StackEntry *)Iter_Get(&_it);
}

status DebugStack_Show(Str *style, Str *msg, word flags){
    if(msg == NULL){
        msg = Str_Make(OutStream->m, 0);
    }
    void *args[2];
    args[0] = style;
    args[1] = NULL;
    if(flags & DEBUG){
        Out("$[STACKNAME]:[STACKFILE]: [STACKREF]^0\n", args);
    }else{
        Out("$[STACKNAME]: [STACKREF]^0\n", args);
    }
    return SUCCESS;
}

i32 DebugStack_Print(Buff *bf, word flags){
    i64 total = 0;
    Iter it;
    memcpy(&it, &_it, sizeof(Iter));
    Iter_GetByIdx(&it, it.idx+1);
    it.type.state |= PROCESSING;
    while((Iter_Prev(&it) & END) == 0){
        word fl = flags;
        total += ToS(bf, it.value, 0, fl);
    }
    return total;
}

status DebugStack_Init(MemCh *m){
    Iter_Init(&_it, Span_Make(m));
    return SUCCESS;
}
