#include <external.h>
#include <caneka.h>

void *Iter_Latest(Iter *it, cls typeOf){
    Iter _it;
    memcpy(&_it, it, sizeof(Iter));
    do {
        Abstract *a = Iter_Get(&_it);
        if(a != NULL && a->type.of == typeOf){
            return a;
        }
    } while((Iter_Prev(&_it) & END) == 0);
    return NULL;
}
