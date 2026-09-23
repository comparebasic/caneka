#include <external.h>
#include "base_module.h"

SlateSet slateInitialSet = {
    15, 14, 13, 12,
    11, 10, 9, 8, 
    7, 6, 5, 4,
    3, 2, 1, 0
};

i8 Slate_Add(Slate *slate, void *item){
    i8 *ptr = &slate->set[slate->next];
    i8 idx = *ptr;
    *ptr = -1;
    if(idx == 0){
        slate->type.state |= TAIL;
    }else{
        slate->next--;
    }
    slate->slab[idx] = item;
    slate->type.state &= ~END;

    return idx;
}

void Slate_Remove(Slate *slate, i16 idx){
    if(idx > SPAN_LOCAL_MAX){ 
        slate->type.state |= ERROR;
        return;
    }

    slate->slab[idx] = NULL;
    i8 *ptr = &slate->set[slate->next];
    *ptr = idx;
    if(slate->type.state & TAIL){
        slate->type.state &= ~TAIL;
    }else{
        slate->next++;
    }

    if(slate->next == SPAN_LOCAL_MAX){
        slate->type.state |= END;
    }
}

void Slate_Init(Slate *slate){
    slate->type.of = TYPE_SLATE;
    memcpy(&slate->queue, &slateInitialSet, sizeof(SlateSet));
    slate->idx.nextInQueue = SPAN_LOCAL_MAX;
}

Slate *Slate_Make(MemCh *m){
    Slate *slate = (Slate *)MemCh_Alloc(m, sizeof(Slate));
    Slate_Init(slate);
    return slate;
}

