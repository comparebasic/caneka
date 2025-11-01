#include <external.h>
#include <caneka.h>

static Iter _it;

void _DebugStack_Push(char *cstr, char *fname, void *ref, word typeOf, i32 line, i32 pos){
    Span *stack = _it.p;

    word fl = stack->m->type.state;
    stack->m->type.state |= MEMCH_BASE;

    StackEntry *entry = MemCh_Alloc(stack->m, sizeof(StackEntry));
    entry->type.of = TYPE_DEBUG_STACK_ENTRY;
    entry->funcName = cstr;
    entry->fname = fname;
    entry->ref = ref;
    entry->typeOf = typeOf;
    entry->line = line;
    entry->pos = pos;

    Iter_Add(&_it, (Abstract*)entry);
    stack->m->type.state = fl;
}

void DebugStack_Pop(){
    Iter_Remove(&_it);
    Iter_Prev(&_it);
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
    Abstract *args[3];
    args[0] = (Abstract *)style;
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
    Iter_Init(&it, _it.p);
    while((Iter_Prev(&it) & END) == 0){
        word fl = flags;
        if(it.idx == it.p->max_idx){
            fl |= MORE;
        }
        total += ToS(bf, it.value, 0, fl);
    }
    return total;
}

status DebugStack_Init(MemCh *m){
    Iter_Init(&_it, Span_Make(m));
    return SUCCESS;
}
