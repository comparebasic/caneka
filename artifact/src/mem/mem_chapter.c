#include <external.h>
#include <caneka.h>

static MemChapter *_chapterGlobal = NULL;

status MemChapter_Init(){
    if(_chapterGlobal == NULL){
       MemChapter_Make(&_chapterGlobal); 
       return SUCCESS;
    }
    return NOOP;
}

i64 MemCount(i16 level){
    i64 total = 0;
    Iter it;
    Iter_Init(&it, &_chapterGlobal->pages);
    while((Iter_Next(&it) & END) == 0){
        MemSlab *sl = (MemSlab *)Iter_Get(&it);
        if(sl != NULL && sl->level >= level){
            total += MemSlab_Taken(sl);
        }
    }
    return total;
}

status MemChapter_FreeSlab(MemCtx *m, MemSlab *sl){
    MemChapter *cp = _chapterGlobal;
    size_t sz = MemSlab_Taken(sl); 
    memset(sl->bytes+sl->remaining, 0, sz);
    int idx = (((void *)sl) - cp->start)/PAGE_SIZE;
    return Span_Remove(&cp->pages, idx);
}

void *MemChapter_GetBytes(){
    MemChapter *cp = _chapterGlobal;
    int idx = -1;
    if(cp->pages.metrics.available != -1){
        idx = cp->pages.metrics.available;
        cp->pages.metrics.available = -1;
    }else{
        while((Iter_Next(&cp->it) & END) == 0){
            MemSlab *sl = (MemSlab *)Iter_Get(&cp->it);
            if(sl == NULL){
                idx = cp->it.idx;
                cp->pages.metrics.selected = idx;
                break;
            }
        }
    }
    if(idx != -1){
        cp->pages.metrics.selected = idx;
        void *page = cp->start+(idx*PAGE_SIZE);
        memset(page, 0, PAGE_SIZE);
        return page;
    }
    /* make new chapter here as all chapters are full */
    Fatal("Next chapter not implemented", TYPE_CHAPTER);
    return NULL;
}

status MemChapter_Claim(MemSlab *sl){
    MemChapter *cp = _chapterGlobal;
    if(cp->pages.metrics.selected != -1){
        Span_Set(&cp->pages, cp->pages.metrics.selected, (Abstract *)sl);
        cp->pages.metrics.selected = -1;
        return SUCCESS;
    }
    return ERROR;
}

status MemChapter_Make(MemChapter **_cp){
    void *start = mmap(NULL, 
        CHAPTER_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);

    if(start == MAP_FAILED){
        printf("err:%s\n", strerror(errno));
        Fatal("Unable to map memory", TYPE_CHAPTER);
        return ERROR;
    }

    MemSlab sl = {
        .type = {TYPE_MEMSLAB, 0},
        .level = 0,
        .remaining = MEM_SLAB_SIZE,
        .bytes = start,
    };

    MemChapter *cp = MemSlab_Alloc(&sl, sizeof(MemChapter));
    MemCtx_Setup(&cp->m, &sl);
    cp->start = start;

    Span *p = &cp->pages;
    Span_Setup(p);
    p->m = &cp->m;
    p->root = MemSlab_Alloc(&cp->m.first, SLOT_BYTE_SIZE*SPAN_STRIDE);
    p->max_idx = PAGE_MAX;
    Iter_Init(&cp->it, &cp->pages);

    *_cp = cp;
    cp->pages.metrics.selected = 0;
    MemChapter_Claim(&cp->m.first);
    return SUCCESS;
}
