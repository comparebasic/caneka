#include <external.h>
#include <caneka.h>

Str *State_ToStr(MemCh *m, status state){
    Str *s = Str_Make(m, STR_DEFAULT);
    Single sg = {.type = {TYPE_WRAPPED, state}, .objType = 0, .val.value = 0};
    Buff *bf = Buff_Make(m, BUFF_STRVEC);
    ToS_FlagLabels(bf, (Abstract *)&sg);
    return Span_Get(bf->v->p, 0);
}
