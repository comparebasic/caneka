#include <external.h>
#include "base_module.h"

Slate *Slate_Make(MemCh *m, i16 count){
    Slate *sl = (Slate *)MemCh_Alloc(m, sizeof(Slate));
    sl->type.of = TYPE_SLATE;
    sl->rangeType.range = count;

    sl->slots = (void **)Bytes_Alloc(m,
        sizeof(void *)*count, TYPE_POINTER_ARRAY);

    sl->available.start = (i16 *)Bytes_Alloc(m,
        sizeof(i16 *)*count, TYPE_BYTES_POINTER);

    for(i16 i = 0; i < count; i++){
        sl->available.start[i] = count - 1 - i;
    }

    sl->available.last = sl->available.start+(count-1);
    sl->available.next = sl->available.last;

    return sl;
}

i16 Slate_Add(MemCh *m, Slate *sl, void *item){
    i16 idx = *sl->available.next;
    *sl->available.next = -1;
    if(sl->available.next == sl->available.start){
        sl->type.state |= LAST;
    }else{
        sl->available.next--;
    }

    sl->slots[idx] = item;
    sl->type.state &= ~END;
    return idx;
}

void Slate_Remove(MemCh *m, Slate *sl, i16 idx){
    if(idx > sl->rangeType.range-1 || sl->slots[idx] == NULL || 
            sl->available.next == sl->available.last){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Out of bounds or already removed", NULL);
        sl->type.state |= ERROR;
        return;
    }
    sl->slots[idx] = NULL;
    if(sl->type.state & LAST){
        sl->type.state &= ~LAST;
    }else{
        sl->available.next++;
    }
    *sl->available.next = idx;
    if(sl->available.next == sl->available.last){
        sl->type.state |= END;
    }
}
