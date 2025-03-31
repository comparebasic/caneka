#include <external.h>
#include <caneka.h>

void *MemPage_Alloc(MemPage *sl, word sz){
    sl->remaining -= sz;
    return sl->bytes+((size_t)sl->remaining); 
}

MemPage *MemPage_Attach(MemCh *m, i16 level){
    MemPage *sl = MemPage_Make(m, level);
    i32 idx = m->it.span->max_idx+1;

    if(_increments[m->it.span->dims] <(m->it.span->nvalues+1)){
        MemCh_Expand(m);
        MemCh_ReserveSpanExpand(m, sl, idx);
    }

    Span_Set(m->it.span, idx, (Abstract *)sl);
    return sl;
}

MemPage *MemPage_Make(MemCh *m, i16 level){
    void *bytes = MemBook_GetPage(m);
    if(bytes == NULL){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Error allocating page");
        return NULL;
    }
    MemPage _sl = {
        .type = {TYPE_MEMSLAB, 0},
        .level = level,
        .remaining = MEM_SLAB_SIZE,
        .bytes = bytes,
    };
    MemPage *sl = MemPage_Alloc(&_sl, sizeof(MemPage));
    memcpy(sl, &_sl, sizeof(MemPage));
    printf("MemPage_Make page:%lu sl%lu/%d\n", bytes, (util)sl, (i32)sl->type.of);
    return sl;
}
