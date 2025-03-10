#include <external.h>
#include <caneka.h>

cls Ifc_Get(cls inst){
    if(inst == TYPE_STRING_CHAIN || inst == TYPE_STRING_FIXED || inst == TYPE_STRING_FULL || inst == TYPE_STRING_SLAB){
        return TYPE_STRING;
    }else if(inst == TYPE_SPAN || inst == TYPE_MEMCTX || inst == TYPE_QUEUE_SPAN || 
            inst == TYPE_STRING_SPAN || inst ==  TYPE_SLAB_SPAN || inst == TYPE_MINI_SPAN){
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
        return inst == TYPE_WRAPPED_DO || inst == TYPE_WRAPPED_UTIL || inst == TYPE_WRAPPED_FUNC || inst == TYPE_WRAPPED_PTR;
    }else if(ifc == TYPE_STRING){
        return inst == TYPE_STRING_CHAIN || inst == TYPE_STRING_FIXED || inst == TYPE_STRING_FULL;
    }else if(ifc == TYPE_STRING){
        return inst == TYPE_STRING_CHAIN || inst == TYPE_STRING_FIXED;
    }else if(ifc == TYPE_ROEBLING){
        return inst == TYPE_ROEBLING || inst == TYPE_ROEBLING_BLANK;
    }else if(ifc == TYPE_SPAN){ 
        return (inst == TYPE_SPAN || inst == TYPE_MEMCTX || inst == TYPE_QUEUE_SPAN || 
            inst == TYPE_STRING_SPAN || inst ==  TYPE_SLAB_SPAN || inst == TYPE_MINI_SPAN ||
            inst == TYPE_TABLE || inst == TYPE_ORDERED_TABLE);
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

