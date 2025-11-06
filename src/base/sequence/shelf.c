#include <external.h>
#include <caneka.h>

boolean Shelf_Available(Iter *shelf){
    if(shelf->metrics.selected == -1 || 
            shelf->metrics.selected == shelf->p->max_idx){
        return FALSE;
    }else{
        return TRUE;
    }
}

status Shelf_Add(Iter *shelf, Abstract *a){
    shelf->metrics.selected++;
    return Iter_SetByIdx(shelf, shelf->metrics.selected, a);
}

Abstract * Shelf_Get(Iter *shelf){
    if(shelf->metrics.selected >= 0){
        Abstract *a = Iter_GetByIdx(shelf, shelf->metrics.selected);
        shelf->metrics.selected--;
        return a;
    }else{
        return NULL;
    }
}
