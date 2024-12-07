#include <external.h>
#include <caneka.h>

Blank *Blank_Make(MemCtx *m){
    Blank *b = MemCtx_Alloc(m, sizeof(Blank);
    b->type = TYPE_BLANK;
    return b;
}
