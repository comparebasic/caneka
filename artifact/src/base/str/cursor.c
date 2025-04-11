#include <external.h>
#include <caneka.h>

status Cursor_Add(Cursor *curs, Str *s){
    StrVec_Add(curs->v, s);
    while((Iter_Next(&curs->it) & END) == 0 && curs->it.value != s){}
    return SUCCESS;
}

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

Cursor *Cursor_Make(MemCh *m, StrVec *v){
    Cursor *curs = (Cursor *)MemCh_Alloc(m, sizeof(Cursor));
    curs->type.of = TYPE_CURSOR;
    Cursor_Setup(curs, v);
    return curs;
}
