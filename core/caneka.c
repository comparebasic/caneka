#include <external.h>
#include <caneka.h>

status Caneka_Init(MemCtx *m){
    status r = READY;
    r |= SpanDef_Init();
    r |= Debug_Init(m);
    r |= Hash_Init(m);
    return r;
}

static word states[] = {
    ERROR,
    OPTIONAL,
    NOOP,
    INLINE,
    BREAK,
    TRACKED,
    INCOMING,
    PROCESSING,
    RESPONDING,
    RAW,
    MISS,
    HASHED,
    INVERTED,
    END,
    NEXT,
    KO,
    SUCCESS,
};

static char *stateNames[] = {
    "ERROR,",
    "OPTIONAL,",
    "NOOP/WHITESPACE,",
    "INLINE/ELASTIC,",
    "BREAK,",
    "TRACKED/TERM_FOUND,",
    "INCOMING,",
    "PROCESSING,",
    "RESPONDING,",
    "RAW,",
    "MISS,",
    "HASHED,",
    "INVERTED,",
    "END,",
    "NEXT,",
    "KO,",
    "SUCCESS,",
    NULL,
};

static char _buff[1024];
char *State_ToString(status state){
    int pos = 0;
    memset(_buff, 0, 1024);
    int i = 0;
    char *name = NULL;
    if(state == 0){
        memcpy(_buff, "ZERO", strlen("ZERO"));
    }else{
        while((name = stateNames[i]) != NULL){
            if((state & states[i]) != 0){
                int l = strlen(stateNames[i]);
                memcpy(_buff+pos, stateNames[i], l);
                pos += l;
            }
            i++;
        }
    }
    return _buff;
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
