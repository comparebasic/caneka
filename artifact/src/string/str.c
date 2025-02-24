#include <external.h>
#include <caneka.h>

Str *Str_Make(MemCtx *m, byte *ptr, i64 length){
    Str *s = MemCtx_Alloc(m, sizeof(Str));
    s->type.of = TYPE_STR;
    s->length = length;
    s->bytes = ptr;
    return s;
}
