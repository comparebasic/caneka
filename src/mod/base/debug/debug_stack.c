#include <external.h>
#include "base_module.h"

void DebugStack_SetRef(MemCh *m, void *_a, const char *funcName, const char *fname, i32 lineno){
    Abstract *a = (Abstract *)_a;

    StackEntry *entry = (StackEntry *)Iter_GetByIdx(&m->debugIt, m->debugIt.idx);
    entry->funcName = funcName;
    entry->fname = fname;
    entry->ref = a;
    entry->typeOf = a->type.of;
    entry->line = lineno;
}

void DebugStack_Push(MemCh *m, void *_a, const char *funcName, const char *fname, i32 lineno){
    Abstract *a = (Abstract *)_a;

    StackEntry *entry = (StackEntry *)Iter_GetByIdx(&m->debugIt, m->debugIt.idx+1);
    if(entry == NULL){
        status fl = m->type.state;
        m->type.state |= MEMCH_BASE;
        entry = MemCh_Alloc(m, sizeof(StackEntry));
        entry->type.of = TYPE_DEBUG_STACK_ENTRY;
        Iter_Add(&m->debugIt, entry);
        m->type.state = fl;
    }

    entry->funcName = funcName;
    entry->fname = fname;
    entry->ref = a;
    entry->typeOf = a->type.of;
    entry->line = lineno;
}

void DebugStack_Pop(MemCh *m){
    if(m->debugIt.idx > 0){
        Iter_GetByIdx(&m->debugIt, m->debugIt.idx-1);
    }
}

void DebugStack_Print(MemCh *m, Buff *bf, word flags){
    while((Iter_Prev(&m->debugIt) & END) == 0){
        ToS(bf, m->debugIt.value, 0, flags);
    }
}
