#include <external.h>
#include <caneka.h>

i64 MemCh_Used(MemCh *m, i16 level){
    i64 total = 0;
    while((Iter_Next(&m->it) & END) == 0){
        MemPage *sl = (MemPage *)m->it.value;
        if(sl != NULL && (level == 0 || sl->level == level)){
            total += MEM_SLAB_SIZE - sl->remaining;
        }
    }

    return total;
}

i64 MemCh_MemCount(MemCh *m, i16 level){
    i64 total = 0;
    while((Iter_Next(&m->it) & END) == 0){
        MemPage *sl = (MemPage *)m->it.value;
        if(sl != NULL && (level == 0 || sl->level == level)){
            total += MEM_SLAB_SIZE;
        }
    }

    return total;
}

i64 MemCh_Total(MemCh *m, i16 level){
    return MemCh_MemCount(m, level);
}

void *MemCh_GetPage(MemCh *m, void *addr, i32 *idx){
    Iter_Reset(&m->it);
    while((Iter_Next(&m->it) & END) == 0){
        MemPage *pg = (MemPage *)m->it.value;
        if(pg != NULL){
            void *start = (void *)pg;
            void *end = (void *)pg;
            end += sizeof(MemPage)+MEM_SLAB_SIZE;
            if(addr >= start && addr <= end){
                *idx = m->it.idx;
                return pg;
            }
        }
    }

    Iter_Reset(&m->it);
    *idx = -1;
    return NULL;
}
