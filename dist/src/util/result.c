#include <external.h>
#include <caneka.h>

Result *Result_Make(MemCtx *m, int range, String *s, Abstract *source){
    Result *r = MemCtx_Alloc(m, sizeof(Result));
    r->type.of = TYPE_RESULT;
    r->range = range;
    r->s = s;
    r->source = source;
    return r;
}
