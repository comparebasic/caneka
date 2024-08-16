#include <external.h>
#include <caneka.h>

Chain *DebugPrintChain = NULL;

int DEBUG_SCURSOR = 0;
int DEBUG_MATCH = 0;
int DEBUG_PATMATCH = 0;
int DEBUG_CURSOR = 0;
int DEBUG_PARSER = 0;
int DEBUG_ROEBLING = 0;
int DEBUG_ROEBLING_MARK = 0;
int DEBUG_ROEBLING_COMPLETE = 0;
int DEBUG_ROEBLING_CONTENT = 0;
int DEBUG_ALLOC = 0;
int DEBUG_BOUNDS_CHECK = 0;
int DEBUG_TABLE = 0;
int DEBUG_ROEBLING_CURRENT = 0;
int DEBUG_SPAN = 0;

int DEBUG_ROEBLING_NAME = COLOR_GREEN;

static MemCtx *DebugM = NULL;

static Abstract *Print(MemCtx *m, Abstract *a){
    Debug_Print(a, 0, "", 0, TRUE);
    return NULL;
}

static Abstract *PrintAddr(MemHandle *_mh, Abstract *a){
    printf("%p ", a);
    return NULL;
}

static status patFlagStr(word flags, char str[]){
    int i = 0;
    if(flags == PAT_END){
        str[i++] = 'E';
    }
    if((flags & PAT_TERM) != 0){
        str[i++] = 'X';
    }
    if((flags & PAT_OPTIONAL) != 0){
        str[i++] = 'P';
    }
    if((flags & PAT_MANY) != 0){
        str[i++] = 'M';
    }
    if((flags & PAT_ANY) != 0){
        str[i++] = 'N';
    }
    if((flags & PAT_INVERT) != 0){
        str[i++] = 'I';
    }
    if((flags & PAT_COUNT) != 0){
        str[i++] = 'C';
    }
    if((flags & PAT_SET_NOOP) != 0){
        str[i++] = 'N';
    }
    if((flags & PAT_IGNORE) != 0){
        str[i++] = 'G';
    }
    if((flags & PAT_WILDCOUNT) != 0){
        str[i++] = 'W';
    }
    if((flags & PAT_ALL) != 0){
        str[i++] = 'A';
    }
    str[i] = '\0';
    return SUCCESS;
}

static void patCharDef_PrintSingle(PatCharDef *def, cls type, char *msg, int color, boolean extended){
    char flag_cstr[12];
    patFlagStr(def->flags, flag_cstr);
    if((def->flags & PAT_COUNT) != 0){
        if(def->from == '\r' || def->from == '\n'){
            printf("%s%s=0x%hux0x%hu", msg, flag_cstr, def->from, def->to);
        }else{
            printf("%s%s=%cx%hu", msg, flag_cstr, (char)def->from, def->to);
        }
    }else if((def->flags & PAT_ALL) != 0 || def->flags == PAT_END){
        printf("%s%s", msg, flag_cstr);
    }else if(def->from == def->to){
        if(def->from == '\r' || def->from == '\n' || def->from == '\t'){
            printf("%s%s=#%hu", msg, flag_cstr, def->from);
        }else{
            printf("%s%s=%c", msg, flag_cstr, (char)def->from);
        }
    }else{
        if((def->from == '\r' || def->from == '\n') || (def->to == '\r' || def->to == '\n') || (def->to < 32 || def->to < 32) || (def->to > 128 || def->to > 127)){
            printf("%s%s=#%hu-#%hu", msg, flag_cstr, def->from, def->to);
        }else{
            printf("%s%s=%c-%c", msg, flag_cstr, (char)def->from, (char)def->to);
        }
    }
    if((def->flags & PAT_TERM) != 0){
        printf(".");
    }
}

static void PatCharDef_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    PatCharDef *def = (PatCharDef *)a;
    char flag_cstr[12];
    if(extended){
        while(def->flags != PAT_END){
            patCharDef_PrintSingle(def, TYPE_PATCHARDEF, "", color, extended);
            def++;
        }
    }else{
        patCharDef_PrintSingle(def, TYPE_PATCHARDEF, "", color, extended);
    }
}

static void Match_PrintPat(Abstract *a, cls type, char *msg, int color, boolean extended){
    Match *mt = (Match *)as(a, TYPE_PATMATCH);
    if(extended){
        printf("\x1b[%dm%sMatch<%s:state=%s:length=%d:pos=%d:jump=%d:count=%d:remainig=%d ", color, msg, Class_ToString(mt->type.of), State_ToString(mt->type.state), mt->length, mt->position, mt->jump, mt->count, mt->remaining);
        Debug_Print((void *)mt->def.pat, TYPE_PATCHARDEF, "", color, FALSE);
        printf(">\x1b[0m");
    }else{
        printf("\x1b[%dm%sMatch<state=%s:pos=%d>\x1b[0m", color, msg, State_ToString(mt->type.state), mt->position);
    }
}

static void Mess_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Mess *ms = (Mess *)a;
    printf("\x1b[%dm%sM<%s value=\x1b[0m", color, msg, Class_ToString(ms->type.of));
    Debug_Print((void *)ms->value, 0, "", color, extended);
    Hashed *h = ms->atts;
    printf(" ");
    while(h != NULL){
        Debug_Print((void *)h, 0, "", color, extended);
        h = h->next;
        printf(", ");
    }
    Mess *child = ms->firstChild;
    printf("\x1b[%dm [", color);
    while(child != NULL){
        Debug_Print((void *)child, 0, "", color, extended);
        child = child->next;
    }
    printf("\x1b[%dm]", color);
    printf(">\x1b[0m");
}

static void XmlCtx_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    XmlCtx *ctx = (XmlCtx *)as(a, TYPE_XMLCTX);
    if(extended){
        printf("\x1b[%dm%sXmlCtx<%s nodes=", color, msg, State_ToString(ctx->type.state));
        Debug_Print((void *)ctx->root, 0, "", color, FALSE);
        printf("\x1b[%dm>\x1b[0m", color);
    }else{
        printf("\x1b[%dm%sXmlCtx<%s>\x1b[0m", color, msg, State_ToString(ctx->type.state));
    }
}

static void Match_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Match *mt = (Match *)as(a, TYPE_MATCH);
    if(extended){
        printf("%sMatch<%s:state=%s:pos=%d:'\x1b[%d;1m%s\x1b[0;%dm'>", msg, Class_ToString(mt->type.of), State_ToString(mt->type.state), mt->position, color, mt->def.s->bytes, color);
    }else{
        printf("%sMatch<state=%s:pos=%d>\n", msg, State_ToString(mt->type.state), mt->position);
    }
}

static void Single_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Single *v = (Single *)a;
    printf("\x1b[%dm%sSingle<%s:%u:%ld>\x1b[0m", color, msg, Class_ToString(v->type.of), v->type.state, v->val.value);
}

static void StringMatch_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Match *mt = (Match *)as(a, TYPE_STRINGMATCH);
    if(extended){
        printf("\x1b[%dm%sMatch<%s:state=%s:pos=%d:jump=%d:count=%d:remainig=%d ", color, msg, Class_ToString(mt->type.of), State_ToString(mt->type.state), mt->position, mt->jump, mt->count, mt->remaining);
        Debug_Print((void *)mt->def.s, 0, "", color, FALSE);
        printf(">\x1b[0m");
    }else{
        printf("\x1b[%dm%sMatch<state=%s:pos=%d ", color, msg, State_ToString(mt->type.state), mt->position);
        Debug_Print((void *)mt->def.s, 0, "", color, FALSE);
        printf(">\x1b[0m");
    }
}

static void StringFixed_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    String *s = (String *)as(a, TYPE_STRING_FIXED);
    String *esc = String_ToEscaped(DebugM, s);
    if(extended){
        printf("%s\x1b[%dmSFixed<\x1b[0;%dm", msg, color, color);
        printf("s/%u=\"\x1b[1;%dm%s\x1b[0;%dm\"", s->length, color, esc->bytes, color);
        printf("\x1b[%dm>\x1b[0m", color);
    }else{
        printf("\x1b[%dm%s\"\x1b[1;%dm%s\x1b[0;%dm\"", color, msg, color, esc->bytes, color);
    }
}

static void Roebling_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    /*
    Roebling *rbl = (Roebling *) as(a, TYPE_ROEBLING);
    printf("\x1b[%dm%sRbl<%s:source=%u", color, msg, State_ToString(rbl->type.state), rbl->source != NULL ? rbl->source->type.of: 0);
    printf(":");
    if(extended){
        printf(" idx:%d ", rbl->idx);
        Debug_Print((void *)&(rbl->range), 0, "", color, extended);
        printf("\n  \x1b[%dmmatches=\n", color);
        Match *mt = NULL;
        for(int i = 0; i < rbl->matches.values->nvalues; i++){
            mt = Span_Get(rbl->matches.values, i);
            Debug_Print((void *)mt, 0, "    ", color, extended);
            if(i < rbl->matches.values->nvalues-1){
                printf(",\n");
            }
        }
        printf("\n  \x1b[%dmko=\n", color);
        for(int i = 0; i < rbl->matches.ko->nvalues; i++){
            mt = Span_Get(rbl->matches.ko, i);
            Debug_Print((void *)mt, 0, "    ", color, extended);
            if(i < rbl->matches.ko->nvalues-1){
                printf(",\n");
            }
        }
        printf("\n  \x1b[%dmDoF=\n", color);
        Span_Run(NULL, rbl->parsers_do, PrintAddr, NULL);
        printf("\n");
    }else{
        printf(" idx:%d", rbl->idx);
    }
    printf(">\x1b[0m");
    */
}

static void String_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    String *s = (String *)as(a, TYPE_STRING_CHAIN);
    printf("%s\x1b[%dmS<\x1b[0;%dm", msg, color, color);
    do {
        String *esc = String_ToEscaped(DebugM, s);
        printf("s/%u=\"\x1b[1;%dm%s\x1b[0;%dm\"", s->length, color, esc->bytes, color);
        s = s->next;
    } while(s != NULL);
    printf("\x1b[%dm>\x1b[0m", color);
}

static void Wrapped_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    String *s = (String *)asIfc(a, TYPE_WRAPPED);
    printf("\x1b[%dm%sW<%s>\x1b[0m", color, msg, Class_ToString(a->type.of));
}

static void WrappedUtil_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Single *sgl = (Single *)as(a, TYPE_WRAPPED_UTIL);
    printf("\x1b[%dm%sWi64<\x1b[1;%dm%lu\x1b[0;%dm>\x1b[0m", color,  msg, color, sgl->val.value, color);
}

static void Req_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    /*
    Req *req = (Req *) as(a, TYPE_REQ);
    printf("%s\x1b[1;%dmReq<%s:%s ",
        msg, color, State_ToString(req->state), Proto_ToChars(req->proto));
    Debug_Print((void *)req->proto, 0, "", color, extended);
    if(req->in.rbl != NULL && extended){
        printf(" ");
        Debug_Print((void *)req->in.rbl, 0, "", color, extended);
    }
    printf(">\x1b[0m");
    */
}

static void Abstract_Print(Abstract *t, cls type, char *msg, int color, boolean extended){
    if(t == NULL){
        printf("u0");
    }else{
        printf("%p", t);
    }
}

static void Hashed_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Hashed *h = (Hashed *)as(a, TYPE_HASHED);
    if(extended){
        printf("\x1b[%dm%sH<%u:%lu itm=", color, msg, h->idx, h->id);
        Debug_Print((void *)h->item, 0, "", color, extended);
        printf("\x1b[%dm v=", color);
        Debug_Print((void *)h->value, 0, "", color, extended);
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


static void SCursor_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    SCursor *sc = (SCursor *)a;
    printf("%s\x1b[%dmCursor<%s:%ld ", msg, color,
        State_ToString(sc->state), 
        sc->position);
    if(extended){
        Debug_Print((void *)sc->seg, 0, "", color, extended);
    }else{
        printf("\x1b[%dm>\x1b[0m", color);
    }
}

static void Range_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Range *range = (Range *)a;
    printf("%s\x1b[%dmRange<%s search=", msg, color, State_ToString(range->state));
    Debug_Print((void *)range->search, 0, "", color, extended);
    printf("\x1b[%dm[", color);
    Debug_Print((void *)&(range->start), TYPE_SCURSOR, "", color, extended);
    Debug_Print((void *)&(range->end), TYPE_SCURSOR, "...", color, extended);
    printf("\x1b[%dm]>\x1b[0m", color);
}


static void ProtoDef_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    ProtoDef *proto = (ProtoDef *)a;
    if(a->type.state != 0){
        Debug_Print((void *)a, a->type.state, msg, color, extended);
    }else{
        printf("\x1b[%dm%sProtoDef<unknown>\x1b[0m", color, msg);
    }
}

static status populateDebugPrint(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_ABSTRACT, (void *)Abstract_Print);
    r |= Lookup_Add(m, lk, TYPE_MATCH, (void *)Match_Print);
    r |= Lookup_Add(m, lk, TYPE_STRINGMATCH, (void *)StringMatch_Print);
    r |= Lookup_Add(m, lk, TYPE_PATMATCH, (void *)Match_PrintPat);
    r |= Lookup_Add(m, lk, TYPE_PATCHARDEF, (void *)PatCharDef_Print);
    r |= Lookup_Add(m, lk, TYPE_PATMATCH, (void *)Match_PrintPat);
    r |= Lookup_Add(m, lk, TYPE_STRING_CHAIN, (void *)String_Print);
    r |= Lookup_Add(m, lk, TYPE_STRING_FIXED, (void *)StringFixed_Print);
    r |= Lookup_Add(m, lk, TYPE_SCURSOR, (void *)SCursor_Print);
    r |= Lookup_Add(m, lk, TYPE_RANGE, (void *)Range_Print);
    r |= Lookup_Add(m, lk, TYPE_REQ, (void *)Req_Print);
    /*
    r |= Lookup_Add(m, lk, TYPE_SLAB, (void *)Slab_Print);
    r |= Lookup_Add(m, lk, TYPE_SPAN, (void *)Span_Print);
    */
    r |= Lookup_Add(m, lk, TYPE_PROTODEF, (void *)ProtoDef_Print);
    r |= Lookup_Add(m, lk, TYPE_ROEBLING, (void *)Roebling_Print);
    r |= Lookup_Add(m, lk, TYPE_HASHED, (void *)Hashed_Print);
    r |= Lookup_Add(m, lk, TYPE_SINGLE, (void *)Single_Print);
    r |= Lookup_Add(m, lk, TYPE_RBL_MARK, (void *)Single_Print);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_UTIL, (void *)WrappedUtil_Print);
    r |= Lookup_Add(m, lk, TYPE_XMLCTX, (void *)XmlCtx_Print);
    r |= Lookup_Add(m, lk, TYPE_MESS, (void *)Mess_Print);
    return r;
}

status Debug_Init(MemCtx *m){
    /*
    DebugM = m;
    if(DebugPrintChain == NULL){
        Lookup *funcs = Lookup_Make(m, _TYPE_START, populateDebugPrint, NULL);
        DebugPrintChain = Chain_Make(m, funcs);
        return SUCCESS;
    }
    */
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

