#include <external.h>
#include <caneka.h>

static MemChapter *_chapterGlobal = NULL;

status MemChapter_Make(MemChapter **cp){
    void *start = mmap(0, CHAPTER_SIZE, PROT_READ|PROT_READ, MAP_ANONYMOUS, -1, 0);

    if(start != NULL){
        MemChapter *cp = (MemChapter *cp)start;
        cp->type.of = TYPE_CHAPTER;
        cp->size = CHAPTER_SIZE;
        cp->start = start;
        Span_Init(cp->pages);
        Iter_Init(&cp->it, cp->pages);
        cp->m = MemCtx_Make();
        return SUCCESS;
    }
    return ERROR;
}

status MemChapter_FreeSlab(MemCtx *m, MemSlab *sl){
    MemChapter *cp = _chapterGlobal;
    size_t sz = MemSlab_Taken(sl); 
    memset(sl->bytes+sl->remaining, 0, sz);
    int idx = (sl - cp->start)/PAGE_SIZE;
    return Span_Remove(cp->pages, idx);
}

void *MemChapter_GetBytes(MemCtx *m, i16 level){
    MemChapter *cp = _chapterGlobal;
    if(_chapterGlobal == NULL){
       MemChapter_Make(&_chapterGlobal); 
    }
    int idx = -1;
    if(cp->pages->metrics.available != -1){
        idx = cp->pages->metrics.available;
        p->metrics.available = -1;
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
}

status MemChapter_Claim(MemSlab *sl){
    MemChapter *cp = _chapterGlobal;
    if(cp->metrics.selected){
        Span_Set(cp, cp->metrics.selected, sl);
        cp->metrics.selected = -1;
        return SUCCESS;
    }
    return ERROR;
}
