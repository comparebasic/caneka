#include <external.h>
#include <caneka.h>

TemplFunc *TemplFunc_Make(MemCh *m, TFunc func, status flags){
    TemplFunc *tfunc = MemCh_AllocOf(m, sizeof(TemplFunc), TYPE_TEMPL_JUMP_FUNC);
    tfunc->type.of = TYPE_TEMPL_JUMP_FUNC;
    tfunc->type.state = flags;
    tfunc->func = func;
    return tfunc;
}
