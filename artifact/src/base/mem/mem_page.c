#include <external.h>
#include <caneka.h>

void *MemPage_Alloc(MemPage *pg, word sz){
    void *ptr = pg;
    ptr += sizeof(MemPage);
    pg->remaining -= sz;
    return ptr+((util)pg->remaining); 
}

MemPage *MemPage_Attach(MemCh *m, i16 level){
    MemPage *pg = MemPage_Make(m, level);
    i32 idx = m->it.span->max_idx+1;

    Span_Set(m->it.span, idx, (Abstract *)pg);
    return pg;
}

MemPage *MemPage_Make(MemCh *m, i16 level){
    void *bytes = MemBook_GetPage(m);
    if(bytes == NULL){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER,
            "Error allocating page", NULL);
        return NULL;
    }
    MemPage *pg = (MemPage *)bytes;
    pg->type.of = TYPE_MEMSLAB;
    pg->remaining = MEM_SLAB_SIZE;
    pg->level = level;
    return pg;
}
