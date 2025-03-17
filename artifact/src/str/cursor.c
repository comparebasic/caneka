#include <external.h>
#include <caneka.h>

status Cursor_Setup(Cursor *curs, StrVec *v){
    curs->type.of = TYPE_STRVEC_CURS;
    curs->type.state = READY;
    curs->pos = 0;
    curs->slot = 0;
    curs->v = v;
    curs->ptr = NULL;
    curs->end = NULL;
    Iter_Init(&curs->it, v->p);
    return SUCCESS;
}
