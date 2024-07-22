#include <external.h>
#include <caneka.h>

boolean Mark_Eq(Abstract *a, void *b){
    if(b == NULL){
        return FALSE;
    }
    Single *sgl = (Single *) as(a, TYPE_RBL_MARK); 
    return sgl->value == *((word *)b);
}

Single *Mark_Make(MemCtx *m, word mark){
    Single *mrk = (Single *)MemCtx_Alloc(m, sizeof(Single));
    mrk->type.of = TYPE_RBL_MARK;
    mrk->value = mark;
    return mrk;
}
