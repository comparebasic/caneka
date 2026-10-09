#include <external.h>
#include "base_module.h"

i8 slateInitialSet[SPAN_STRIDE] = {
    15, 14, 13, 12,
    11, 10, 9, 8, 
    7, 6, 5, 4,
    3, 2, 1, 0
};

void *Slate_Get(Slate *slate, i8 idx){
    return slate->slots[idx];
}

i8 Slate_Add(Slate *sl, void *item){
    if(sl->idx.max+1 > SPAN_LOCAL_MAX){
        sl->type.state |= ERROR;
        return -1;
    }
    if(sl->type.state & NOOP){
        sl->type.state &= ~NOOP;
    }else{
        sl->idx.max++;
    }
    sl->slots[sl->idx.max] = item;
    sl->idx.count++;

    if(sl->idx.max == SPAN_LOCAL_MAX){
        sl->type.state |= END;
    }else{
        sl->type.state &= ~END;
    }

    return sl->idx.max;
}

i8 Slate_Insert(Slate *sl, i8 idx, void *item){
    if(idx > SPAN_LOCAL_MAX){
        sl->type.state |= ERROR;
        return -1;
    }

    if(sl->slots[idx] == NULL){
        sl->idx.count++;
    }

    sl->slots[idx] = item;

    return idx;
}

i8 Slate_Enqueue(Slate *slate, void *item){
    return -1;
}

void Slate_Remove(Slate *sl, i8 idx){
    if(idx > SPAN_LOCAL_MAX){ 
        sl->type.state |= ERROR;
        return;
    }

    sl->slots[idx] = NULL;
    if(sl->type.state & TAIL){
        sl->type.state &= ~TAIL;
    }else{
        sl->idx.nextInQueue++;
    }

    sl->queue[sl->idx.nextInQueue] = idx;

    if(sl->idx.nextInQueue == SPAN_LOCAL_MAX){
        sl->type.state |= NOOP;
    }
}

void Slate_Init(Slate *slate){
    slate->type.of = TYPE_SLATE;
    slate->type.state = NOOP;
    memcpy(&slate->queue, &slateInitialSet, sizeof(slateInitialSet));
    slate->idx.nextInQueue = SPAN_LOCAL_MAX;
}

Slate *Slate_Make(MemCh *m){
    Slate *slate = (Slate *)MemCh_Alloc(m, sizeof(Slate));
    Slate_Init(slate);
    return slate;
}
