#include <external.h>
#include <caneka.h>

static MemChapter *_chapterGlobal = NULL;

i64 MemCount(i16 level){
    i64 total = 0;
    Iter it;
    Iter_Init(&it, _chapterGlobal->pages);
    while((Iter_Next(&it) & END) == 0){
        MemSlab *sl = (MemSlab *)Iter_Get(&it);
        if(sl != NULL && sl->level >= level){
            total += MemSlab_Taken(sl);
        }
    }
    return total;
}

status MemChapter_Make(MemChapter **_cp){
    void *start = mmap(0, CHAPTER_SIZE, PROT_READ|PROT_READ, MAP_ANONYMOUS, -1, 0);

    if(start != NULL){
        MemSlab sl = {
            .type = {TYPE_MEMSLAB, 0},
            .level = 0,
            .remaining = MEM_SLAB_SIZE,
            .bytes = start,
        };
        MemCtx *m = (MemCtx *)MemSlab_Alloc(&sl, sizeof(MemCtx));
        Span_Setup(m);
        m->slotSize = sizeof(MemSlab);
        m->ptrSlot = 1;
        Span_Set(m, 0, (Abstract *)&sl);

        MemChapter *cp = (MemChapter *)MemSlab_Alloc(&sl, sizeof(MemChapter));
        cp->type.of = TYPE_CHAPTER;
        cp->size = CHAPTER_SIZE;
        cp->m = m;
        cp->start = start;
        Span_Setup(cp->pages);
        Iter_Init(&cp->it, cp->pages);
        *_cp = cp;

        MemChapter_Claim((MemSlab *)start);
        return SUCCESS;
    }
    return ERROR;
}

status MemChapter_FreeSlab(MemCtx *m, MemSlab *sl){
    MemChapter *cp = _chapterGlobal;
    size_t sz = MemSlab_Taken(sl); 
    memset(sl->bytes+sl->remaining, 0, sz);
    int idx = (((void *)sl) - cp->start)/PAGE_SIZE;
    return Span_Remove(cp->pages, idx);
}

void *MemChapter_GetBytes(){
    MemChapter *cp = _chapterGlobal;
    if(_chapterGlobal == NULL){
       MemChapter_Make(&_chapterGlobal); 
    }
    int idx = -1;
    if(cp->pages->metrics.available != -1){
        idx = cp->pages->metrics.available;
        cp->pages->metrics.available = -1;
    }else{
        while((Iter_Next(&cp->it) & END) == 0){
            MemSlab *sl = (MemSlab *)Iter_Get(&cp->it);
            if(sl == NULL){
                idx = cp->it.idx;
                cp->pages->metrics.selected = idx;
                break;
            }
        }
    }
    if(idx != -1){
        return cp->start+(idx*PAGE_SIZE);
    }
    /* make new chapter here as all chapters are full */
    Fatal("Next chapter not implemented", TYPE_CHAPTER);
    return NULL;
}

status MemChapter_Claim(MemSlab *sl){
    MemChapter *cp = _chapterGlobal;
    if(cp->pages->metrics.selected){
        Span_Set(cp->pages, cp->pages->metrics.selected, (Abstract *)sl);
        cp->pages->metrics.selected = -1;
        return SUCCESS;
    }
    return ERROR;
}
