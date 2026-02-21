/* Base.bytes.Shelf
 *
 * Utility functions which set values of an Iter objects *metrics.selected*
 * attribute.
 *
 * It serves the purpose of weather an available slot exists in an Iter managed Span
 * or if a new slot should be ocupied by the next object.
 *
 */

#include <external.h>
#include "base_module.h"

boolean Shelf_Available(Iter *shelf){
    if(shelf->metrics.selected == -1 || 
            shelf->metrics.selected == shelf->p->max_idx){
        return FALSE;
    }else{
        return TRUE;
    }
}

status Shelf_Add(Iter *shelf, void *a){
    shelf->metrics.selected++;
    return Iter_SetByIdx(shelf, shelf->metrics.selected, a);
}

void * Shelf_Get(Iter *shelf){
    if(shelf->metrics.selected >= 0){
        void *a = Iter_GetByIdx(shelf, shelf->metrics.selected);
        shelf->metrics.selected--;
        return a;
    }else{
        return NULL;
    }
}
