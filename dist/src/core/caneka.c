#include <external.h>
#include <caneka.h>

word GLOBAL_flags = 0;

static status archChecks(){
    size_t sec = sizeof(i64);
    if(sec != 8){
        Fatal("Unkown architecture with less than 8 bits for a util\n", TYPE_STRING);
    }
    return ERROR; 
}

status Caneka_Init(MemCtx *m){
    status r = READY;
    r |= archChecks();
    r |= SpanDef_Init();
#ifdef DEBUG_STACK
    r |= DebugStack_Init();
#endif
    r |= Clone_Init(m);
    r |= Debug_Init(m);
    r |= Hash_Init(m);
    r |= Oset_Init(m);
    r |= Enc_Init(m);
    r |= AppsDebug_Init(m);
    return r;
}

static word states[] = {
    READY,
    ERROR,
    SUCCESS,
    NOOP,
    DEBUG,
    HASHED,
    LOCAL_PTR,
    END,
    PROCESSING,
};

static char *stateNames[] = {
    "READY",
    "ERROR",
    "SUCCESS",
    "NOOP",
    "DEBUG",
    "HASHED",
    "LOCAL_PTR",
    "END",
    "PROCESSING",
    NULL,
};

String *State_ToString(MemCtx *m, status state){
    String *s = String_Init(m, STRING_EXTEND);
    char *name = NULL;
    if(state == 0){
        String_AddBytes(m, s, bytes("ZERO"), strlen("ZERO"));
    }else{
        int i = 0;
        while((name = stateNames[i]) != NULL){
            if((state & states[i]) != 0){
                String_AddBytes(m, s, bytes(stateNames[i]), strlen(stateNames[i]));
            }
            i++;
        }

    }

    byte upper = (byte) (state >> 8);
    if(upper != 0){
        String_AddBytes(m, s, bytes("<<"), strlen("<<"));
        if((state & 1 << 8) != 0){
            String_AddBytes(m, s, bytes("8,"), strlen("8,"));
        }else if(state == 1 << 9){
            String_AddBytes(m, s, bytes("9,"), strlen("9,"));
        }else if(state == 1 << 10){
            String_AddBytes(m, s, bytes("10,"), strlen("10,"));
        }else if(state == 1 << 11){
            String_AddBytes(m, s, bytes("11,"), strlen("11,"));
        }else if(state == 1 << 12){
            String_AddBytes(m, s, bytes("12,"), strlen("12,"));
        }else if(state == 1 << 13){
            String_AddBytes(m, s, bytes("13,"), strlen("13,"));
        }else if(state == 1 << 14){
            String_AddBytes(m, s, bytes("14,"), strlen("14,"));
        }else if(state == 1 << 15){
            String_AddBytes(m, s, bytes("15,"), strlen("15,"));
        }
        String_AddBytes(m, s, bytes(" "), 1);
    }

    String_AddBitPrint(m, s, &upper, sizeof(byte), TRUE);

    return s;
}

char *Class_ToString(cls type){
    if(type == _TYPE_START){
        return "_TYPE_START";
    }else if(type == TYPE_UNKNOWN){
        return "TYPE_UNKNOWN";
    }else if(type == TYPE_BLANK){
        return "TYPE_BLANK";
    }else if(type == TYPE_ABSTRACT){
        return "TYPE_ABSTRACT";
    }else if(type == TYPE_RESERVE){
        return "TYPE_RESERVE";
    }else if(type == TYPE_WRAPPED){
        return "TYPE_WRAPPED";
    }else if(type == TYPE_WRAPPED_FUNC){
        return "TYPE_WRAPPED_FUNC";
    }else if(type == TYPE_WRAPPED_DO){
        return "TYPE_WRAPPED_DO";
    }else if(type == TYPE_WRAPPED_UTIL){
        return "TYPE_WRAPPED_UTIL";
    }else if(type == TYPE_WRAPPED_I64){
        return "TYPE_WRAPPED_I64";
    }else if(type == TYPE_WRAPPED_TIME64){
        return "TYPE_WRAPPED_TIME64";
    }else if(type == TYPE_WRAPPED_BOOL){
        return "TYPE_WRAPPED_BOOL";
    }else if(type == TYPE_WRAPPED_PTR){
        return "TYPE_WRAPPED_PTR";
    }else if(type == TYPE_UTIL){
        return "TYPE_UTIL";
    }else if(type == TYPE_UNIT){
        return "TYPE_UNIT";
    }else if(type == TYPE_MEMCTX){
        return "TYPE_MEMCTX";
    }else if(type == TYPE_MEMHANDLE){
        return "TYPE_MEMHANDLE";
    }else if(type == TYPE_MHABSTRACT){
        return "TYPE_MHABSTRACT";
    }else if(type == TYPE_MEMLOCAL){
        return "TYPE_MEMLOCAL";
    }else if(type == TYPE_MESS){
        return "TYPE_MESS";
    }else if(type == TYPE_MAKER){
        return "TYPE_MAKER";
    }else if(type == TYPE_MEMSLAB){
        return "TYPE_MEMSLAB";
    }else if(type == TYPE_STRING){
        return "TYPE_STRING";
    }else if(type == TYPE_STRING_CHAIN){
        return "TYPE_STRING_CHAIN";
    }else if(type == TYPE_STRING_FIXED){
        return "TYPE_STRING_FIXED";
    }else if(type == TYPE_STRING_FULL){
        return "TYPE_STRING_FULL";
    }else if(type == TYPE_STRING_SLAB){
        return "TYPE_STRING_SLAB";
    }else if(type == TYPE_TESTSUITE){
        return "TYPE_TESTSUITE";
    }else if(type == TYPE_PARSER){
        return "TYPE_PARSER";
    }else if(type == TYPE_ROEBLING){
        return "TYPE_ROEBLING";
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
    }else if(type == TYPE_IOCTX){
        return "TYPE_IOCTX";
    }else if(type == TYPE_ACCESS){
        return "TYPE_ACCESS";
    }else if(type == TYPE_AUTH){
        return "TYPE_AUTH";
    }else if(type == _TYPE_SPAN_START){
        return "_TYPE_SPAN_START";
    }else if(type == TYPE_SPAN){
        return "TYPE_SPAN";
    }else if(type == TYPE_MINI_SPAN){
        return "TYPE_MINI_SPAN";
    }else if(type == TYPE_MEM_SPAN){
        return "TYPE_MEM_SPAN";
    }else if(type == TYPE_QUEUE_SPAN){
        return "TYPE_QUEUE_SPAN";
    }else if(type == TYPE_QUEUE_IDX){
        return "TYPE_QUEUE_IDX";
    }else if(type == TYPE_SLAB_SPAN){
        return "TYPE_SLAB_SPAN";
    }else if(type == TYPE_STRING_SPAN){
        return "TYPE_STRING_SPAN";
    }else if(type == TYPE_NESTED_SPAN){
        return "TYPE_NESTED_SPAN";
    }else if(type == TYPE_TABLE){
        return "TYPE_TABLE";
    }else if(type == TYPE_POLL_MAP_SPAN){
        return "TYPE_POLL_MAP_SPAN";
    }else if(type == _TYPE_SPAN_END){
        return "_TYPE_SPAN_END";
    }else if(type == TYPE_COORDS){
        return "TYPE_COORDS";
    }else if(type == TYPE_MEM_KEYED){
        return "TYPE_MEM_KEYED";
    }else if(type == TYPE_SPAN_STATE){
        return "TYPE_SPAN_STATE";
    }else if(type == TYPE_SPAN_DEF){
        return "TYPE_SPAN_DEF";
    }else if(type == TYPE_QUEUE){
        return "TYPE_QUEUE";
    }else if(type == TYPE_CHAIN){
        return "TYPE_CHAIN";
    }else if(type == TYPE_SPAN_QUERY){
        return "TYPE_SPAN_QUERY";
    }else if(type == TYPE_SLAB){
        return "TYPE_SLAB";
    }else if(type == TYPE_TABLE_CHAIN){
        return "TYPE_TABLE_CHAIN";
    }else if(type == TYPE_HASHED){
        return "TYPE_HASHED";
    }else if(type == TYPE_HASHED_LINKED){
        return "TYPE_HASHED_LINKED";
    }else if(type == TYPE_MEMPAIR){
        return "TYPE_MEMPAIR";
    }else if(type == TYPE_STRINGTABLE){
        return "TYPE_STRINGTABLE";
    }else if(type == TYPE_FILE){
        return "TYPE_FILE";
    }else if(type == TYPE_LOOKUP){
        return "TYPE_LOOKUP";
    }else if(type == TYPE_ITER){
        return "TYPE_ITER";
    }else if(type == TYPE_SINGLE){
        return "TYPE_SINGLE";
    }else if(type == TYPE_RBL_MARK){
        return "TYPE_RBL_MARK";
    }else if(type == TYPE_OSET_ITEM){
        return "TYPE_OSET_ITEM";
    }else if(type == TYPE_FMT_ITEM){
        return "TYPE_FMT_ITEM";
    }else if(type == TYPE_FMT_DEF){
        return "TYPE_FMT_DEF";
    }else if(type == TYPE_OSET){
        return "TYPE_OSET";
    }else if(type == TYPE_FMT_CTX){
        return "TYPE_FMT_CTX";
    }else if(type == TYPE_OSET_DEF){
        return "TYPE_OSET_DEF";
    }else if(type == TYPE_CASH){
        return "TYPE_CASH";
    }else if(type == TYPE_XMLT){
        return "TYPE_XMLT";
    }else if(type == TYPE_NESTEDD){
        return "TYPE_NESTEDD";
    }else if(type == TYPE_ENC_PAIR){
        return "TYPE_ENC_PAIR";
    }else if(type == TYPE_FMT_HTML){
        return "TYPE_FMT_HTML";
    }else if(type == _TYPE_CORE_END){
        return "_TYPE_CORE_END";
    }else{
        return "TYPE_unknown";
    }
}

boolean Abs_Eq(Abstract *a, void *b){
    if(a == NULL || b == NULL){
        return FALSE;
    }
    if(a->type.of == TYPE_STRING_CHAIN || a->type.of == TYPE_STRING_FIXED){
        return String_Eq(a, b);
    }else{
        Fatal("Not implemented", TYPE_ABSTRACT);
        return FALSE;
    }
}

boolean Caneka_Truthy(Abstract *a){
    if(a == NULL){
        return FALSE;
    }else if(Ifc_Match(a->type.of, TYPE_WRAPPED_I64)){
        return ((Single *)a)->val.value != 0;
    }else if(Ifc_Match(a->type.of, TYPE_STRING)){
        return ((String *)a)->length > 1;
    }else{
        return FALSE;
    }
}

cls Ifc_Get(cls inst){
    if(inst == TYPE_STRING_CHAIN || inst == TYPE_STRING_FIXED || inst == TYPE_STRING_FULL){
        return TYPE_STRING;
    }else if(inst == TYPE_SPAN || inst == TYPE_QUEUE_SPAN || 
            inst == TYPE_STRING_SPAN || inst ==  TYPE_SLAB_SPAN || inst == TYPE_MINI_SPAN){
        return TYPE_SPAN;
    }

    return inst;
}

status ToStdOut(MemCtx *m, String *s, Abstract *source){
    while(s != NULL){
        puts((char *)s->bytes);
        s = String_Next(s);
    }
    return SUCCESS;
}

boolean Ifc_Match(cls inst, cls ifc){
    if(inst == ifc){
        return TRUE;
    }

    if(ifc == TYPE_MEMCTX){
        return inst == TYPE_SPAN || inst == TYPE_REQ || inst == TYPE_SERVECTX 
            || inst == TYPE_ROEBLING;
    }else if(ifc == TYPE_WRAPPED){
        return inst == TYPE_WRAPPED_DO || inst == TYPE_WRAPPED_UTIL || inst == TYPE_WRAPPED_FUNC || inst == TYPE_WRAPPED_PTR;
    }else if(ifc == TYPE_STRING){
        return inst == TYPE_STRING_CHAIN || inst == TYPE_STRING_FIXED || inst == TYPE_STRING_FULL;
    }else if(ifc == TYPE_STRING){
        return inst == TYPE_STRING_CHAIN || inst == TYPE_STRING_FIXED;
    }else if(ifc == TYPE_MEMHANDLE){
        return inst == TYPE_MHABSTRACT || inst == TYPE_MEMHANDLE || inst == TYPE_ROEBLING ||
            inst == TYPE_REQ || inst == TYPE_SERVECTX || inst == TYPE_MEM_KEYED;
    }else if(ifc == TYPE_SPAN){ 
        return (inst == TYPE_SPAN || inst == TYPE_QUEUE_SPAN || 
            inst == TYPE_STRING_SPAN || inst ==  TYPE_SLAB_SPAN || inst == TYPE_MINI_SPAN ||
            inst == TYPE_TABLE);
    }else if(ifc == TYPE_TRANSP){ 
        return (inst == TYPE_TRANSP || inst == TYPE_LANG_CNK);
    }else if(ifc == TYPE_FMT_CTX){ 
        return (inst == TYPE_FMT_CTX || inst == TYPE_LANG_CNK);
    }

    return FALSE;
}
