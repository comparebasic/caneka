#include <external.h>
#include <caneka.h>

static int _chapterIdx = -1;
static MemRange _chapters[16] = {
    {NULL, NULL, NULL}, {NULL, NULL, NULL}, {NULL, NULL, NULL}, {NULL, NULL, NULL},
    {NULL, NULL, NULL}, {NULL, NULL, NULL}, {NULL, NULL, NULL}, {NULL, NULL, NULL},
    {NULL, NULL, NULL}, {NULL, NULL, NULL}, {NULL, NULL, NULL}, {NULL, NULL, NULL},
    {NULL, NULL, NULL}, {NULL, NULL, NULL}, {NULL, NULL, NULL}, {NULL, NULL, NULL}
};

static MemChapter *MemChapter_get(void *addr){
    int idx = _chapterIdx;
    if(addr == NULL){
        return _chapters[idx].chapter;
    }
    while(idx >= 0){
        MemRange *mrange = _chapters+idx;
        if(addr >= mrange->start && addr <= mrange->end){
            return mrange->chapter;
        }
        idx--;
    }
    Fatal("MemChapter not found", TYPE_CHAPTER);
    return NULL;
}

#ifdef INSECURE
MemChapter *MemChapter_Get(void *addr){
    return MemChapter_get(addr);
}
#endif

i64 MemCount(i16 level){
    i64 total = 0;
    Iter it;
    for(int i = 0; i <= _chapterIdx; i++){
        MemChapter *cp = _chapters[i].chapter;
        Iter_Init(&it, &cp->pages);
        while((Iter_Next(&it) & END) == 0){
            MemSlab *sl = (MemSlab *)Iter_Get(&it);
            if(sl != NULL && sl->level >= level){
                total += MemSlab_Taken(sl);
            }
        }
    }
    return total;
}

status MemChapter_FreeSlab(MemCtx *m, MemSlab *sl){
    size_t sz = MemSlab_Taken(sl); 
    memset(sl->bytes+sl->remaining, 0, sz);
    MemChapter *cp = MemChapter_get(m);
    int idx = (((void *)sl) - cp->start)/PAGE_SIZE;
    return Span_Remove(&cp->pages, idx);
}

void *MemChapter_GetBytes(){
    MemChapter *cp = MemChapter_get(NULL);
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
    /*
    printf("it->idx:%d, idx:%d max_idx:%d nvalues:%d\n", cp->it.idx, idx, cp->pages.max_idx, cp->pages.nvalues);
    */
    if(idx == -1 && cp->pages.max_idx+1 < PAGE_COUNT){
        idx = cp->pages.max_idx+1;
    }
    if(idx != -1){
        cp->pages.metrics.selected = idx;
        void *page = cp->start+(idx*PAGE_SIZE);
        memset(page, 0, PAGE_SIZE);
        return page;
    }
    /* make new chapter here as all chapters are full */
    MemChapter_Print((Abstract *)cp, 0, "Chapter: ", 31, TRUE);
    printf("\n");
    Span_Print((Abstract *)&cp->pages, 0, "ChapterSpan: ", 31, TRUE);
    printf("\n");
    Fatal("Next chapter not implemented", TYPE_CHAPTER);

    if(MemChapter_Make(cp) != NULL){
        return MemChapter_GetBytes();
    }
    return NULL;
}

status MemChapter_Claim(MemSlab *sl){
    MemChapter *cp = MemChapter_get(sl);
    if(cp->pages.metrics.selected != -1){
        Span_Set(&cp->pages, cp->pages.metrics.selected, (Abstract *)sl);
        cp->pages.metrics.selected = -1;
        return SUCCESS;
    }
    return ERROR;
}

MemChapter *MemChapter_Make(MemChapter *prev){
    _chapterIdx++;
    if(_chapterIdx >= CHAPTER_MAX){
        Fatal("Chapter already taken", TYPE_CHAPTER);
        return NULL;
    }
    MemRange *mrange = _chapters+_chapterIdx;
    if(mrange->chapter != NULL){
        Fatal("Chapter already taken", TYPE_CHAPTER);
        return NULL;
    }

    void *start = mmap(prev, 
        CHAPTER_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);

    mrange->start = start;
    mrange->end = start+CHAPTER_SIZE-sizeof(void *);

    if(start == MAP_FAILED){
        printf("err:%s\n", strerror(errno));
        Fatal("Unable to map memory", TYPE_CHAPTER);
        return NULL;
    }

    MemSlab sl = {
        .type = {TYPE_MEMSLAB, 0},
        .level = 0,
        .remaining = MEM_SLAB_SIZE,
        .bytes = start,
    };

    MemChapter *cp = MemSlab_Alloc(&sl, sizeof(MemChapter));
    cp->type.of = TYPE_CHAPTER;
    MemCtx_Setup(&cp->m, &sl);

    cp->m.p.type.state |= DEBUG;

    cp->start = start;
    mrange->chapter = cp;

    Span *p = &cp->pages;
    Span_Setup(p);
    p->m = &cp->m;
    p->root = MemSlab_Alloc(&cp->m.first, sizeof(slab));
    Iter_Init(&cp->it, &cp->pages);

    cp->pages.metrics.selected = 0;
    MemChapter_Claim(&cp->m.first);
    return cp;
}
