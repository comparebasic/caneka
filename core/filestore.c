#include <external.h>
#include <filestore.h>

Virtual *Maker_Make(Memctx*m, void *mk, cls type){
    Virtual *v = (Vitrual *)MemCtx_Alloct(m, sizeof(Virtual));
    v->type.of = TYPE_MAKER;
    v->type.state = type;
    v->ptr = (util)mk;
    return v;
}

char *State_ToString(status state){
    if(state == READY){
        return "READY";
    }else if(state == SUCCESS){
        return "SUCCESS/COMPLETE";
    }else if(state == ERROR){
        return "ERROR";
    }else if(state == NOOP){
        return "NOOP";
    }else if(state == INCOMING){
        return "INCOMING";
    }else if(state == PROCESSING){
        return "PROCESSING";
    }else if(state == RESPONDING){
        return "RESPONDING";
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
    }else if(type == TYPE_SLAB){
        return "TYPE_SLAB";
    }else{
        return "TYPE_unknown";
    }
}
