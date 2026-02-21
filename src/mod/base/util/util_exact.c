#include <external.h>
#include "base_module.h" 

boolean Single_Exact(Single *a, Single *b){
    if(a->type.of != b->type.of || a->type.state != b->type.state ||
            a->objType.of != b->objType.of || a->objType.state != b->objType.state){
        return FALSE;
    }
    if(a->type.of == TYPE_WRAPPED_PTR){
        return Exact(a->val.ptr, b->val.ptr);
    }else{
        return a->val.value == b->val.value;
    }
}
