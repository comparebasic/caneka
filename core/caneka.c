#include <external.h>
#include <caneka.h>

status Caneka_Init(MemCtx *m){
    status r = READY;
    r |= SpanDef_Init();
    r |= Debug_Init(m);
    r |= Hash_Init(m);
    return r;
}

char *State_ToString(status state){
    if(state ==  READY){
        return "READY";
    }else if((state & ERROR) != 0){
        return "ERROR";
    }else if((state & NOOP) != 0){
        return "NOOP";
    }else if((state & INCOMING) != 0){
        return "INCOMING";
    }else if((state & (PROCESSING|INVERTED)) == (PROCESSING|INVERTED)){
        return "PROCESSING|INVERTED";
    }else if((state & PROCESSING) != 0){
        return "PROCESSING";
    }else if((state & RESPONDING) != 0){
        return "RESPONDING";
    }else if((state & SUCCESS) != 0){
        return "SUCCESS/COMPLETE";
    }else if((state & RAW) != 0){
        return "RAW";
    }else if((state & INLINE) != 0){
        return "INLINE";
    }else if((state & MISS) != 0){
        return "MISS";
    }else if((state & HASHED) != 0){
        return "HASHED";
    }else if((state & INVERTED) != 0){
        return "INVERTED";
    }else if(HasFlag(state, NEXT|KO)){
        return "NEXT+KO";
    }else if((state & END) != 0){
        return "END";
    }else if((state & NEXT) != 0){
        return "NEXT";
    }else if((state & KO) != 0){
        return "KO";
    }else{
        return "UNKNOWN_state";
    }

}

char *Class_ToString(cls type){
    if(type == TYPE_UNIT){
        return "TYPE_UNIT";
    }else if(type == TYPE_MEMCTX){
        return "TYPE_MEMCTX";
    }else if(type == TYPE_MEMSLAB){
        return "TYPE_MEMSLAB";
    }else if(type == TYPE_REQ){
        return "TYPE_REQ";
    }else if(type == TYPE_STRING_CHAIN){
        return "TYPE_STRING_CHAIN";
    }else if(type == TYPE_STRING_FIXED){
        return "TYPE_STRING_FIXED";
    }else if(type == TYPE_SERVECTX){
        return "TYPE_SERVECTX";
    }else if(type == TYPE_TESTSUITE){
        return "TYPE_TESTSUITE";
    }else if(type == TYPE_PARSER){
        return "TYPE_PARSER";
    }else if(type == TYPE_MULTIPARSER){
        return "TYPE_MULTIPARSER";
    }else if(type == TYPE_SCURSOR){
        return "TYPE_SCURSOR";
    }else if(type == TYPE_RANGE){
        return "TYPE_RANGE";
    }else if(type == TYPE_MATCH){
        return "TYPE_MATCH";
    }else if(type == TYPE_STRINGMATCH){
        return "TYPE_STRINGMATCH";
    }else if(type == TYPE_PATMATCH){
        return "TYPE_PATMATCH";
    }else if(type == TYPE_PATCHARDEF){
        return "TYPE_PATCHARDEF";
    }else if(type == TYPE_STRUCTEXP){
        return "TYPE_STRUCTEXP";
    }else if(type == TYPE_SPAN){
        return "TYPE_SPAN";
    }else if(type == TYPE_TABLE){
        return "TYPE_TABLE";
    }else if(type == TYPE_SLAB){
        return "TYPE_SLAB";
    }else if(type == TYPE_RBL_MARK){
        return "TYPE_RBL_MARK";
    }else if(type == TYPE_MESS){
        return "TYPE_MESS";
    }else if(type == TYPE_WRAPPED){
        return "TYPE_WRAPPED";
    }else if(type == TYPE_WRAPPED_FUNC){
        return "TYPE_WRAPPED_FUNC";
    }else if(type == TYPE_WRAPPED_UTIL){
        return "TYPE_WRAPPED_UTIL";
    }else if(type == TYPE_WRAPPED_DO){
        return "TYPE_WRAPPED_DO";
    }else if(type == TYPE_WRAPPED_PTR){
        return "TYPE_WRAPPED_PTR";
    }else{
        return "TYPE_unknown";
    }
}

boolean Abs_Eq(Abstract *a, void *b){
    if(a->type.of == TYPE_STRING_CHAIN || a->type.of == TYPE_STRING_FIXED){
        return String_Eq(a, b);
    }else{
        Fatal("Not implemented", TYPE_ABSTRACT);
        return FALSE;
    }
}

boolean Ifc_Match(cls inst, cls ifc){
    if(ifc == TYPE_MEMCTX){
        return inst == TYPE_SPAN || inst == TYPE_REQ || inst == TYPE_SERVECTX 
            || inst == TYPE_ROEBLING;
    }else if(ifc == TYPE_WRAPPED){
        return inst == TYPE_WRAPPED_DO || inst == TYPE_WRAPPED_UTIL || inst == TYPE_WRAPPED_FUNC || inst == TYPE_WRAPPED_PTR;
    }else if(ifc == TYPE_STRING){
        return inst == TYPE_STRING_CHAIN || inst == TYPE_STRING_FIXED;
    }else if(ifc == TYPE_STRING){
        return inst == TYPE_STRING_CHAIN || inst == TYPE_STRING_FIXED;
    }else if(ifc == TYPE_MEMHANDLE){
        return inst == TYPE_MHABSTRACT || inst == TYPE_MEMHANDLE || inst == TYPE_ROEBLING || inst == TYPE_REQ || inst == TYPE_SERVECTX;
    }else if(ifc == TYPE_SPAN){ 
        return (inst == TYPE_SPAN || inst == TYPE_QUEUE_SPAN || 
            inst == TYPE_STRING_SPAN || inst ==  TYPE_SLAB_SPAN || inst == TYPE_MINI_SPAN ||
            inst == TYPE_TABLE);
    }
    return FALSE;
}
