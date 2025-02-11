#include <external.h>
#include <caneka.h>

Chain *DebugPrintChain = NULL;

int DEBUG_MATCH = 0;
int DEBUG_PATMATCH = 0;
int DEBUG_MATCH_COMPLETE = 0;
int DEBUG_CURSOR = 0;
int DEBUG_PARSER = 0;
int DEBUG_ROEBLING = 0;
int DEBUG_ROEBLING_MARK = 0;
int DEBUG_ROEBLING_COMPLETE = 0;
int DEBUG_ROEBLING_CONTENT = 0;
int DEBUG_ROEBLING_CURRENT = 0;
int DEBUG_ALLOC = 0;
int DEBUG_BOUNDS_CHECK = 0;
int DEBUG_TABLE = 0;
int DEBUG_SPAN = 0;
int DEBUG_SPAN_GET_SET = 0;
int DEBUG_XML = 0;
int DEBUG_XML_CAPTURE = 0;
int DEBUG_XML_TEMPLATE = 0;
int DEBUG_XML_TEMPLATE_NESTING = 0;
int DEBUG_XML_TEMPLATE_OUT = 0;
int DEBUG_ROEBLING_NAME = 0;
int DEBUG_HTTP = 0;
int DEBUG_SERVE = 0;
int DEBUG_SERVE_ACCEPT = 0;
int DEBUG_SERVE_POLLING = 0;
int DEBUG_SERVE_ROUNDS = 0;
int DEBUG_REQ = 0;
int DEBUG_REQ_RECV = 0;
int DEBUG_EXAMPLE_HANDLERS = 0;
int DEBUG_QUEUE = 0;
int DEBUG_FILE = 0;
int DEBUG_OSET = 0;
int DEBUG_OSET_COMPLETE = 0;
int DEBUG_OSET_DEF = 0;
int DEBUG_NESTED = 0;
int DEBUG_CASH = 0;
int DEBUG_USER = 0;
int DEBUG_LANG_TRANSP = 0;
int DEBUG_LANG_CNK = 0;
int DEBUG_LANG_CNK_RBL = 0;
int DEBUG_LANG_CNK_OUT = 0;
int DEBUG_SUBPROCESS = 0;
int DEBUG_STRSNIP = 0;
int DEBUG_CDOC = 0;
int DEBUG_MEMLOCAL = 0;
int DEBUG_IOCTX = 0;

boolean SHOW_SERVE_TESTS = FALSE;

MemCtx *DebugM = NULL;

static Abstract *Print(MemCtx *m, Abstract *a){
    Debug_Print(a, 0, "", 0, TRUE);
    return NULL;
}

char *QueueFlags_ToChars(word flags){
    String *s = String_Init(DebugM, 64);
    if((flags & SLAB_ACTIVE) != 0){
        String_AddBytes(DebugM, s, bytes("A"), 1);
    }
    if((flags & SLAB_FULL) != 0){
        String_AddBytes(DebugM, s, bytes("F"), 1);
    }

    if(s->length == 0){
        return "ZERO";
    }

    return (char *)s->bytes;
}

void NestedD_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    NestedD *nd = (NestedD *)as(a, TYPE_NESTEDD);
    printf("\x1b[%dmND<%s: ", color, State_ToChars(nd->type.state));
    Debug_Print((void *)nd->current_tbl, 0, "", color, extended);
    Iter it;
    Iter_InitReverse(&it, nd->stack);
    printf("\x1b[%dm stack/%d=(", color, nd->stack->nvalues);
    while((Iter_Next(&it) & END) == 0){
        NestedState *ns = (NestedState *)Iter_Get(&it);
        printf("\x1b[%dm%s -> ", color, NestedD_opToChars(ns->flags));
        Debug_Print((void *) ns->t, 0, "", color, FALSE);
        if((it.type.state & FLAG_ITER_LAST) == 0){
            printf("\x1b[%dm,", color);
        }
    }
    printf("\x1b[%dm)>\x1b[0m", color);
}

void FmtDef_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    FmtDef *def = (FmtDef *)as(a, TYPE_FMT_DEF);
    printf("\x1b[%dm%sFmtDef<\x1b[1;%dm%s\x1b[%dm/%s id:%d %s>\x1b[0m", color, msg,
        color,
        String_ToChars(DebugM, def->name),
        color,
        String_ToChars(DebugM, def->alias),
        def->id,
        State_ToChars(def->type.state));
}

void Queue_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Queue *q = as(a, TYPE_QUEUE);
    printf("\x1b[%dm%sQ<%s\x1b[0m", color, msg, State_ToChars(q->type.state));
    if(extended){
        Debug_Print((void *)q->span, 0, "", color, TRUE);
        printf("\n");
        Debug_Print((void *)&q->current, 0, "current=", color, FALSE);
        printf("\n");
        Debug_Print((void *)&q->available, 0, "available=", color, FALSE);
        printf("\n");
        printf("\x1b[%dm>\x1b[0m", color);
    }else{
        printf("\x1b[%dm values:%d current:%d available:%d>\x1b[0m", color, q->span->nvalues, q->current.idx, q->available.idx);
    }
}

static void ProtoDef_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    ProtoDef *proto = (ProtoDef *)a;
    printf("\x1b[%dm%sProtoDef:\x1b[0m", color, msg);
    Debug_Print((void *)proto, proto->reqType, msg, color, extended);
}

static void Req_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Req *req = (Req *) as(a, TYPE_REQ);
    printf("%s\x1b[1;%dmReq(%d/%p)<%s ", msg, color, req->fd, req, State_ToChars(req->type.state));
    Debug_Print((void *)req->sctx->def->source, 0, "", color, extended);
    if(req->in.rbl != NULL && extended){
        printf(" ");
        Debug_Print((void *)req->in.rbl, 0, "", color, extended);
    }
    printf(">\x1b[0m");
}

static void Serve_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Serve *sctx = (Serve *) as(a, TYPE_SERVECTX);
    printf("\x1b[%dm%sServe<%s", color, msg, State_ToChars(sctx->type.state));
    if(extended){
        Debug_Print((void *)&sctx->queue, 0, " next=", color, extended);
    }else{
        printf(" count:%d", sctx->queue.count);
    }
    printf(">\x1b[0m");
}

static void XmlCtx_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    XmlCtx *ctx = (XmlCtx *)as(a, TYPE_XMLCTX);
    if(extended){
        printf("\x1b[%dm%sXmlCtx<%s nodes=", color, msg, State_ToChars(ctx->type.state));
        Debug_Print((void *)ctx->root, 0, "", color, FALSE);
        printf("\x1b[%dm>\x1b[0m", color);
    }else{
        printf("\x1b[%dm%sXmlCtx<%s>\x1b[0m", color, msg, State_ToChars(ctx->type.state));
    }
}

static void HttpProto_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    HttpProto *proto = (HttpProto *)as(a, TYPE_HTTP_PROTO);
    printf("\x1b[%dmHttpProto<%s path='%s' ", color, HttpProto_MethodToChars(proto->method), proto->path != NULL ? (char *)String_ToEscaped(DebugM, proto->path)->bytes : "");
    Debug_Print(proto->body, 0, "body=", color, FALSE);
    Debug_Print(proto->headers_tbl, 0, " headers=", color, FALSE);
    printf("\x1b[%dm>\x1b[0m", color);
}

static void HttpProtoDef_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    ProtoDef *proto = (ProtoDef *)a;
    printf("\x1b[%dm%sHTTP<>\x1b[0m", color, "");
}

static word matchFlags[] = {
    PAT_END, /* E */
    PAT_TERM, /* X */
    PAT_OPTIONAL, /* P */
    PAT_MANY, /* M */
    PAT_ANY, /* N */
    PAT_INVERT, /* I */
    PAT_COUNT, /* C */
    PAT_INVERT_CAPTURE, /* G */
    PAT_KO, /* K */
    PAT_KO_TERM, /* O */
    PAT_SINGLE, /* S */
    PAT_LEAVE , /* L */
    PAT_CMD, /* D */
    PAT_GO_ON_FAIL, /* T */
    PAT_CONSUME, /* U */
};

static char *matchFlagChars = "EXPMNICGKOSLDTU";

static status patFlagStr(word flags, char str[]){
    int p = 0;
    int i = 0;
    /* 0 and & x != 0 dont mix well */
    if(flags == PAT_END){
        str[p++] = 'E';
    }
    int l = strlen(matchFlagChars);
    while(i < l){
       if((flags &matchFlags[i]) != 0){
            str[p++] = matchFlagChars[i];
       }
       i++;
    }
    str[p] = '\0';
    return SUCCESS;
}

static void patCharDef_PrintSingle(PatCharDef *def, cls type, char *msg, int color, boolean extended){
    char flag_cstr[12];
    patFlagStr(def->flags, flag_cstr);
    if((def->flags & PAT_COUNT) != 0){
        if(def->from == '\r' || def->from == '\n'){
            printf("%s=0x%hux0x%hu", flag_cstr, def->from, def->to);
        }else{
            printf("?");
            printf("%s=%cx%hu", flag_cstr, (char)def->from, def->to);
        }
    }else if(def->flags == PAT_END){
        printf("%s", flag_cstr);
    }else if(def->from == def->to){
        if(def->from == '\r' || def->from == '\n' || def->from == '\t'){
            printf("%s=#%hu", flag_cstr, def->from);
        }else{
            printf("%s=%c/%hu", flag_cstr, (char)def->from, (i8)def->from);
        }
    }else{
        if((def->from == '\r' || def->from == '\n') || (def->to == '\r' || def->to == '\n') || (def->to < 32 || def->to < 32) || (def->to > 128 || def->to > 127)){
            printf("%s=#%hu-#%hu", flag_cstr, def->from, def->to);
        }else{
            printf("%s=%c-%c", flag_cstr, (char)def->from, (char)def->to);
        }
    }
    if((def->flags & PAT_TERM) != 0){
        printf(".");
    }
}

static void PatCharDef_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    PatCharDef *def = (PatCharDef *)a;
    printf("\x1b[%dm%s", color, msg);
    if(extended){
        boolean first = TRUE;
        if(def->flags == PAT_END){
            patCharDef_PrintSingle(def, TYPE_PATCHARDEF, "", color, extended);
        }else{
            while(def->flags != PAT_END){
                if(first){
                    first = FALSE;
                }else{
                    printf(",");
                }
                patCharDef_PrintSingle(def, TYPE_PATCHARDEF, "", color, extended);
                def++;
            }
        }
    }else{
        patCharDef_PrintSingle(def, TYPE_PATCHARDEF, msg, color, extended);
    }
    printf("\x1b[0m");
}

static void mess_PrintRecurse(Mess *ms, char *msg, int color, boolean extended, int indent){
    indent_Print(indent);
    printf("\x1b[%dm%sM<%s value=\x1b[0m", color, msg, Class_ToString(ms->type.of));
    Debug_Print((void *)ms->name, 0, "", color, extended);
    printf(" ");
    if(ms->atts != NULL){
        printf("\x1b[%dmatts=[", color);
    }
    Iter it;
    Iter_Init(&it, ms->atts);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = (Hashed *)Iter_Get(&it);
        if(h != NULL){
            Debug_Print((void *)h, 0, "", color, extended);
        }
    }
    printf("\x1b[%dm, ", color);
    if(ms->atts != NULL){
        printf("\x1b[%dm]", color);
    }
    Debug_Print(ms->body, 0, "body=", color, TRUE);
    Mess *child = ms->firstChild;
    if(child != NULL){
        printf("\x1b[%dm [", color);
        printf("\n");
    }
    while(child != NULL){
        mess_PrintRecurse(child, "", color, extended, indent+1);
        child = child->next;
    }
    if(ms->next != NULL){
        printf("\x1b[%dmnext=%p", color, ms->next);
    }
    if(ms->firstChild != NULL){
        indent_Print(indent);
        printf("\x1b[%dm]", color);
        printf(">\x1b[0m");
    }
    printf("\n");
}

static void Mess_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Mess *ms = (Mess *)a;
    mess_PrintRecurse(ms, msg, color, extended, 0);
}

static void Single_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Single *v = (Single *)a;
    printf("\x1b[%dm%sSingle<%s:%u:%ld>\x1b[0m", color, msg, Class_ToString(v->type.of), v->type.state, v->val.value);
}

static void Result_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Result *v = (Result *)a;
    printf("\x1b[%dm%sR<%s:\x1b[1;%dm%d\x1b[0;%dm:", color, msg, State_ToChars(v->type.state), color, v->range, color);
    Debug_Print((void *)v->s, 0, "", color, FALSE);
    printf("\x1b[%dm>\x1b[0m", color);
}

static void Guard_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
   Guard *g = (Guard *)as(a, TYPE_GUARD);
   if((g->type.state & ERROR) != 0){
        color = COLOR_RED;
   }
   printf("\x1b[%dm%sG<count %d of max %d ", color, msg, g->count, g->max);
   Debug_Print((void *)&g->msg, 0, "", color, FALSE);
   printf(" \x1b[%dm%s:%d>\x1b[0m", color, g->file, g->line);
}

static void Wrapped_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    String *s = (String *)asIfc(a, TYPE_WRAPPED);
    printf("\x1b[%dm%sW<%s>\x1b[0m", color, msg, Class_ToString(a->type.of));
}

static void WrappedUtil_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Single *sgl = (Single *)as(a, TYPE_WRAPPED_UTIL);
    printf("\x1b[%dm%sWi64<\x1b[1;%dm%lu\x1b[0;%dm>\x1b[0m", color,  msg, color, sgl->val.value, color);
}

static void WrappedI64_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Single *sgl = (Single *)as(a, TYPE_WRAPPED_I64);
    printf("\x1b[%dm%sWi64<\x1b[1;%dm%lu\x1b[0;%dm>\x1b[0m", color,  msg, color, sgl->val.value, color);
}

static void WrappedTime64_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Single *sgl = (Single *)as(a, TYPE_WRAPPED_TIME64);
    String *s = Time64_Present(DebugM, a);
    printf("\x1b[%dm%sT64<\x1b[1;%dm%s\x1b[0;%dm>\x1b[0m", color, msg, color, s->bytes, color);
}

static void Abstract_Print(Abstract *t, cls type, char *msg, int color, boolean extended){
    if(t == NULL){
        printf("u0");
    }else{
        printf("%p", t);
    }
}

static void Iter_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Iter *it = (Iter *)as(a, TYPE_ITER);
    printf("\x1b[%dm%sI<%s:%d of %d>\x1b[0m", color, msg,
        State_ToChars(it->type.state), it->idx, it->values->nvalues);
}

static void Transp_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Transp *p = (Transp*)as(a, TYPE_TRANSP);
    printf("\x1b[%dm%sTransp<", color, msg);
    if(p->source != NULL){
        Debug_Print((void *)p->source->path, 0, "", color, FALSE);
    }
    Target *t = Span_GetSelected(p->targets->values);
    if(t != NULL){
        Debug_Print((void *)t->path, 0, " -> ", color, FALSE);
    }
    printf("\x1b[%dm>\x1b[0m\n", color);
}

static void Hashed_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Hashed *h = (Hashed *)as(a, TYPE_HASHED);
    if(extended){
        printf("\x1b[%dm%sH<%u:%lu itm=", color, msg, h->idx, h->id);
        if((h->type.state & LOCAL_PTR) != 0){
            printf("\x1b[%dm LOCAL_PTR", color);
        }else{
            Debug_Print((void *)h->item, 0, "", color, extended);
            printf("\x1b[%dm v=", color);
            Debug_Print((void *)h->value, 0, "", color, extended);
            if(h->next != NULL){
                printf("\x1b[%dm next=%p", color, h);
            }
        }
        printf("\x1b[%dm>\x1b[0m", color);
    }else{
        printf("\x1b[%dm%sH<", color, msg);
        Debug_Print((void *)h->item, 0, "", color, FALSE);
        if(h->value != NULL){
            printf("\x1b[%dm v=", color);
            Debug_Print((void *)h->value, 0, "", color, FALSE);
        }
        printf("\x1b[%dm>\x1b[0m", color);
    }
}

static void EncPair_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    EncPair *pair = (EncPair *)as(a, TYPE_ENC_PAIR);
    printf("\x1b[%dm%sEnc<%s %s:",
        color, msg, State_ToChars(pair->type.state), pair->keyId->bytes);
    String *enc = String_Present(DebugM, (Abstract *)pair->enc);
    while(enc != NULL){
        printf("%s", enc->bytes);
        enc = String_Next(enc);
    }
    printf("->");
    String *dec = String_Present(DebugM, (Abstract *)pair->dec);
    while(dec != NULL){
        printf("%s", dec->bytes);
        dec = String_Next(dec);
    }
    printf(">\x1b[0m");
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
    printf("\x1b[%dm", color);
	va_list args;
    va_start(args, msg);
    vprintf(msg, args);
    Debug_Print((void *)a, 0,  "", color, TRUE);
    printf("\x1b[0m\n");
}
