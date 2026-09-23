#include <external.h>
#include "base_module.h"

void *MemPage_Alloc(MemPage *pg, word sz){
    void *ptr = pg;
    ptr += sizeof(MemPage);
    pg->remaining -= sz;
    return ptr+((util)pg->remaining); 
}

MemPage *MemPage_Attach(MemCh *m, i16 level){
    MemPage *pg = MemPage_Make(m, level);
    i32 idx = m->it.p->maxIdx+1;

    Span_Set(m->it.p, idx, pg);
    return pg;
}

MemPage *MemPage_Make(){
    MemPage *pg = MemBook_GetPage();
    if(pg == NULL){
        Fatal(NULL, FUNCNAME, FILENAME, LINENUMBER,
            "Error allocating page bytes are null", NULL);
        return NULL;
    }

    pg->type.of = TYPE_MEMSLAB;
    pg->remaining = MEM_SLAB_SIZE;

    return pg;
}
