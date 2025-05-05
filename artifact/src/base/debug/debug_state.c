#include <external.h>
#include <caneka.h>

Str *State_ToStr(MemCh *m, status state){
    Str *s = Str_Make(m, STR_DEFAULT);
    Single sg = {.type = {TYPE_SINGLE, state}, .objType = 0, .val.value = 0};
    Stream *sm = Stream_MakeStrVec(m);
    ToS_FlagLabels(sm, (Abstract *)&sg);
    return Span_Get(sm->dest.curs->v->p, 0);
}
