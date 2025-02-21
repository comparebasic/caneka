#include <external.h>
#include <caneka.h>

Chain *DebugPrintChain = NULL;

MemCtx *DebugM = NULL;

static Abstract *Print(MemCtx *m, Abstract *a){
    Debug_Print(a, 0, "", 0, TRUE);
    return NULL;
}

static status populateDebugPrint(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_HTTP_PROTO, (void *)HttpProto_Print);
    r |= Lookup_Add(m, lk, TYPE_XMLCTX, (void *)XmlCtx_Print);
    r |= Lookup_Add(m, lk, TYPE_HTTP_PROTODEF, (void *)HttpProtoDef_Print);
    r |= Lookup_Add(m, lk, TYPE_HTTP_PROTO, (void *)HttpProto_Print);
    r |= Lookup_Add(m, lk, TYPE_PROTODEF, (void *)ProtoDef_Print);
    r |= Lookup_Add(m, lk, TYPE_REQ, (void *)Req_Print);
    r |= Lookup_Add(m, lk, TYPE_SERVECTX, (void *)Serve_Print);
    r |= Lookup_Add(m, lk, TYPE_HANDLER, (void *)Handler_Print);
    r |= Lookup_Add(m, lk, TYPE_ABSTRACT, (void *)Abstract_Print);
    r |= Lookup_Add(m, lk, TYPE_PATMATCH, (void *)Match_PrintPat);
    r |= Lookup_Add(m, lk, TYPE_PATCHARDEF, (void *)PatCharDef_Print);
    r |= Lookup_Add(m, lk, TYPE_PATMATCH, (void *)Match_PrintPat);
    r |= Lookup_Add(m, lk, TYPE_STRING_CHAIN, (void *)String_Print);
    r |= Lookup_Add(m, lk, TYPE_STRING_FIXED, (void *)StringFixed_Print);
    r |= Lookup_Add(m, lk, TYPE_STRING_FULL, (void *)StringFull_Print);
    r |= Lookup_Add(m, lk, TYPE_SPAN, (void *)Span_Print);
    r |= Lookup_Add(m, lk, TYPE_QUEUE_SPAN, (void *)Span_Print);
    r |= Lookup_Add(m, lk, TYPE_MINI_SPAN, (void *)Span_Print);
    r |= Lookup_Add(m, lk, TYPE_TABLE, (void *)Span_Print);
    r |= Lookup_Add(m, lk, TYPE_TABLE_CHAIN, (void *)TableChain_Print);
    r |= Lookup_Add(m, lk, TYPE_ROEBLING, (void *)Roebling_Print);
    r |= Lookup_Add(m, lk, TYPE_HASHED, (void *)Hashed_Print);
    r |= Lookup_Add(m, lk, TYPE_SINGLE, (void *)Single_Print);
    r |= Lookup_Add(m, lk, TYPE_RBL_MARK, (void *)Single_Print);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_UTIL, (void *)WrappedUtil_Print);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_I64, (void *)WrappedI64_Print);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_TIME64, (void *)WrappedTime64_Print);
    r |= Lookup_Add(m, lk, TYPE_MESS, (void *)Mess_Print);
    r |= Lookup_Add(m, lk, TYPE_LOOKUP, (void *)Lookup_Print);
    r |= Lookup_Add(m, lk, TYPE_SPAN_QUERY, (void *)SpanQuery_Print);
    r |= Lookup_Add(m, lk, TYPE_SPAN_STATE, (void *)SpanState_Print);
    r |= Lookup_Add(m, lk, TYPE_QUEUE, (void *)Queue_Print);
    r |= Lookup_Add(m, lk, TYPE_ITER, (void *)Iter_Print);
    r |= Lookup_Add(m, lk, TYPE_NESTEDD, (void *)NestedD_Print);
    r |= Lookup_Add(m, lk, TYPE_ENC_PAIR, (void *)EncPair_Print);
    r |= Lookup_Add(m, lk, TYPE_GUARD, (void *)Guard_Print);
    r |= Lookup_Add(m, lk, TYPE_FMT_DEF, (void *)FmtDef_Print);
    r |= Lookup_Add(m, lk, TYPE_RESULT, (void *)Result_Print);
    r |= Lookup_Add(m, lk, TYPE_CURSOR, (void *)Cursor_Print);
    r |= Lookup_Add(m, lk, TYPE_STRSNIP_STRING, (void *)StrSnipString_Print);
    r |= Lookup_Add(m, lk, TYPE_CSTR, (void *)Cstr_Print);
    r |= Lookup_Add(m, lk, TYPE_TRANSP, (void *)Transp_Print);
    r |= Lookup_Add(m, lk, TYPE_MEMCTX, (void *)MemCtx_Print);
    r |= Lookup_Add(m, lk, TYPE_PROCDETS, (void *)ProcDets_Print);
    
    return r;
}

status Debug_Init(MemCtx *m){
    DebugM = m;
    m->type.range++;
    if(DebugPrintChain == NULL){
        Lookup *funcs = Lookup_Make(m, _TYPE_START, populateDebugPrint, NULL);
        DebugPrintChain = Chain_Make(m, funcs);
        return SUCCESS;
    }

    return NOOP;
}

void indent_Print(int indent){
    while(indent--){
        printf("  ");
    }
}

void Debug_Print(void *t, cls type, char *msg, int color, boolean extended){
    if(color >= 0){
        printf("\x1b[%dm", color);
    }

    if(t == NULL){
        printf("%s\x1b[%dmNULL\x1b[0m", msg, color);
        return;
    }

    Abstract *a = (Abstract *)t;
    if(type == 0){
        a = (Abstract *)t;
        type = a->type.of;
    }

    DebugPrintFunc func = (DebugPrintFunc)Chain_Get(DebugPrintChain, type);
    if(func != NULL){
        return func(a, type, msg, color, extended);
    }else{
        printf("type: %hu\n", type);
        printf("%s:%s unkown_debug(%p)", msg, Class_ToString(type), t);
    }

    if(color >= 0){
        printf("\x1b[0m");
    }
    fflush(stdout);
}

void Bits_Print(byte *bt, int length, char *msg, int color, boolean extended){
    printf("\x1b[%dm%s", color, msg);
    for(int i = length-1; i >= 0;i--){
        byte b = bt[i];
        if(extended){
            printf("%03hu=", b);
        }
        for(int j = 7; j >= 0;j--){
            printf("%c", (b & (1 << j)) ? '1' : '0');
        }
        if(extended){
            printf(" ");
        }
    }
    printf("\x1b[0m");
}


void DPrint(Abstract *a, int color, char *msg, ...){
    DebugStack_Push(msg, TYPE_CSTR);
    printf("\x1b[%dm", color);
	va_list args;
    va_start(args, msg);
    vprintf(msg, args);
    Debug_Print((void *)a, 0,  "", color, TRUE);
    printf("\x1b[0m\n");
    DebugStack_Pop();
}
