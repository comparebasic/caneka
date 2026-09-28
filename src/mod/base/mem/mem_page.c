#include <external.h>
#include "base_module.h"

void *MemPage_Alloc(MemPage *pg, i32 sz){
    void *ptr = pg;
    ptr += sizeof(MemPage);
    pg->remaining -= sz;
    return ptr+((util)pg->remaining); 
}

void MemPage_Init(MemPage *pg){
    pg->type.of = TYPE_MEMSLAB;
    pg->remaining = MEM_SLAB_SIZE;
}

MemPage *MemPage_Make(MemCh *m){
    MemPage *pg = MemBook_GetPage(m);
    if(pg == NULL){
        Fatal(NULL, FUNCNAME, FILENAME, LINENUMBER,
            "Error allocating page bytes are null", NULL);
        return NULL;
    }

    pg->type.of = TYPE_MEMSLAB;
    pg->remaining = MEM_SLAB_SIZE;

    return pg;
}
