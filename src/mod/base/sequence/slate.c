#include <external.h>
#include "base_module.h"

Slate *Slate_Make(MemCh *m, i16 count){
    Slate *sl = (Slate *)MemCh_Alloc(m, sizeof(Slate));
    sl->type.of = TYPE_SLATE;

    sl->slots = (void **)Bytes_Alloc(m,
        sizeof(void *)*count, TYPE_POINTER_ARRAY);

    sl->available = (void **)Bytes_Alloc(m,
        sizeof(void *)*i16, TYPE_POINTER_ARRAY);

    for(i16 i = 0; i < count; i++){
        sl->available[i] = count - 1 - i;
    }

    sl->next = available+count-1;
}

i16 Slate_Add(MemCh *m, Slate *sl, void *item){
    i16 idx = *sl->next;
    *sl->next = -1;
    if(sl->next <= sl->available){
        sl->type.state |= LAST;
    }else{
        sl->next--;
    }

    sl->slots[idx] = item;
    return idx;
}

void Slate_Remove(MemCh *m, Slate *sl, i16 idx){
    sl->slots[idx] = NULL;
    if(sl->next >= sl->available + sizeof(void *)*sl->rangeType.range){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Next incremented out of bounds", NULL);
        sl->type.state |= ERROR;
        return;
    }
    sl->type.state &= ~LAST;
    sl->next++;
    *sl->next = idx;
}
