#include <external.h>
#include <caneka.h>

void Type_SetFlag(Abstract *a, word flags){
    a->type.state = (a->type.state & NORMAL_FLAGS) | flags;
}

cls Ifc_Get(cls inst){
    if(inst == TYPE_SPAN || inst == TYPE_TABLE){
        return TYPE_SPAN;
    }

    return inst;
}

boolean Ifc_Match(cls inst, cls ifc){
    if(inst == ifc){
        return TRUE;
    }

    if(ifc == TYPE_MEMCTX){
        return inst == TYPE_SPAN || inst == TYPE_REQ || inst == TYPE_SERVECTX 
            || inst == TYPE_ROEBLING;
    if(ifc == TYPE_MEMLOCAL_ITEM){
        return inst >= HTYPE_LOCAL;
    }
    }else if(ifc == TYPE_WRAPPED){
        return (inst == TYPE_WRAPPED_DO || inst == TYPE_WRAPPED_UTIL ||
            inst == TYPE_WRAPPED_FUNC || inst == TYPE_WRAPPED_PTR ||
            inst == TYPE_WRAPPED_I64 || inst == TYPE_WRAPPED_I32 ||
            inst == TYPE_WRAPPED_I16 || inst == TYPE_WRAPPED_I8);
    }else if(ifc == TYPE_SPAN){ 
        return (inst == TYPE_SPAN || inst == TYPE_TABLE); 
    /*
    }else if(ifc == TYPE_TRANSP){ 
        return (inst == TYPE_TRANSP || inst == TYPE_LANG_CNK);
    */
    }else if(ifc == TYPE_TABLE){ 
        return (inst == TYPE_TABLE || inst == TYPE_ORDERED_TABLE);
    /*
    }else if(ifc == TYPE_FMT_CTX){ 
        return (inst == TYPE_FMT_CTX || inst == TYPE_LANG_CNK || inst == TYPE_LANG_CDOC);
    */
    }

    return FALSE;
}

