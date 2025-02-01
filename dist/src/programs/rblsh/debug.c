#include <external.h>
#include <caneka.h>
#include <rblsh.h>

char *RblShRange_ToChars(word range){
    if(range == _RBLSH_START){
        return "_RBLSH_START";
    }else if(range == TYPE_RBLSH_SUPER){
        return "TYPE_RBLSH_SUPER";
    }else if(range == TYPE_RBLSH_CTX){
        return "TYPE_RBLSH_CTX";
    }else if(range == RBLSH_MARK_START){
        return "RBLSH_MARK_START";
    }else if(range == RBLSH_MARK_END){
        return "RBLSH_MARK_END";
    }else if(range == RBLSH_TERM){
        return "RBLSH_TERM";
    }else if(range == RBLSH_HUP){
        return "RBLSH_HUP";
    }else if(range == RBLSH_WS){
        return "RBLSH_WS";
    }else if(range == RBLSH_NL){
        return "RBLSH_NL";
    }else if(range == RBLSH_ARG){
        return "RBLSH_ARG";
    }else if(range == RBLSH_FLAG){
        return "RBLSH_FLAG";
    }else if(range == RBLSH_WORDFLAG){
        return "RBLSH_WORDFLAG";
    }else if(range == RBLSH_OP){
        return "RBLSH_OP";
    }else if(range == RBLSH_STRING_LIT){
        return "RBLSH_STRING_LIT";
    }else if(range == RBLSH_STRING_OP){
        return "RBLSH_STRING_OP";
    }else if(range == _RBLSH_END){
        return "_RBLSH_END";
    }else{
        return "UNKNOWN";
    }
}

static void RblShCtx_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    RblShCtx *ctx = (RblShCtx *)as(a, TYPE_RBLSH_CTX);
    printf("\x1b[%dmRblShCtx<>\x1b[0m", color);
}

static status populateDebugPrint(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_RBLSH_CTX, (void *)RblShCtx_Print);
    return r;
}

status RblShDebug_Init(MemCtx *m){
    Lookup *funcs = Lookup_Make(m, _TYPE_APPS_START, populateDebugPrint, NULL);
    return Chain_Extend(m, DebugPrintChain, funcs);
}
