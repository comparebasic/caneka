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

int DEBUG_ROEBLING_NAME = COLOR_GREEN;

static void indent_Print(int indent){
    while(indent--){
        printf("  ");
    }
}

static Abstract *Print(MemCtx *m, Abstract *a){
    Debug_Print(a, 0, "", 0, TRUE);
    return NULL;
}

static Abstract *PrintAddr(MemCtx *m, Abstract *a){
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
    if(extended){
        printf("%s\x1b[%dmSFixed<\x1b[0;%dm", msg, color, color);
        printf("s/%hu=\"\x1b[1;%dm%s\x1b[0;%dm\"", s->length, color, s->bytes, color);
        printf("\x1b[%dm>\x1b[0m", color);
    }else{
        printf("\x1b[%dm%s\"\x1b[1;%dm%s\x1b[0;%dm\"", color, msg, color, s->bytes, color);
    }
}

static void Roebling_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Roebling *rbl = (Roebling *) as(a, TYPE_ROEBLING);
    printf("\x1b[%dm%sRbl<%s:source=%hu", color, msg, State_ToString(rbl->type.state), rbl->source->type.of);
    printf(":");
    Debug_Print((void *)&(rbl->range), 0, "", color, extended);
    printf("\x1b[%dm:Pmk=", color);
    Span_Run(NULL, rbl->parsers_pmk, PrintAddr, NULL);
    printf(">\x1b[0m");
}

static void String_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    String *s = (String *)as(a, TYPE_STRING_CHAIN);
    printf("%s\x1b[%dmS<\x1b[0;%dm", msg, color, color);
    do {
        printf("s/%hu=\"\x1b[1;%dm%s\x1b[0;%dm\"", s->length, color, s->bytes, color);
        s = s->next;
    } while(s != NULL);
    printf("\x1b[%dm>\x1b[0m", color);
}

static void Wrapped_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    String *s = (String *)as(a, TYPE_STRING_CHAIN);
    printf("%s\x1b[%dmS<\x1b[0;%dm", msg, color, color);
    do {
        printf("s/%hu=\"\x1b[1;%dm%s\x1b[0;%dm\"", s->length, color, s->bytes, color);
        s = s->next;
    } while(s != NULL);
    printf("\x1b[%dm>\x1b[0m", color);
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

static void slab_Summarize(Slab *slab, char *msg, int color, boolean extended){
    printf("%s\x1b[0;%dmL<incr%d[%d] ", msg, color, slab->increment, slab->offset);
    boolean first = TRUE;
    for(int i = 0; i < SPAN_DIM_SIZE; i++){
        Abstract *t = slab->items[i];
        if(t != NULL){
            if(!first){
                printf(", ");
            }
            if(first){
                first = FALSE;
            }
            printf("%d=", i);
            if(t->type.of == TYPE_SLAB){
                Slab *slt = (Slab *)t;
                printf("%u", slt->offset);
            }else{
                printf("%p", t);
            }
            printf("\x1b[%dm", color);
        }
    }
    printf("\x1b[1;%dm>\x1b[0m", color);
}

static void Slab_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Slab *slab = (Slab *)a;
    if(slab->increment != SPAN_DIM_SIZE){
        return slab_Summarize(slab, msg, color, extended);
    }

    printf("%s\x1b[%dmL<icr%d[%d] \x1b[%dm", msg, color, slab->increment, slab->offset, color);
    boolean first = TRUE;
    for(int i = 0; i < SPAN_DIM_SIZE; i+= slab->slotSize){
        void *t = NULL;
        if((slab->type.state & RAW) != 0){
            t = Slab_GetPtr(slab, i);
        }else{
            t = slab->items[i];
        }
        if(t != NULL && ((Abstract *)t)->type.of != 0){
            if(!first){
                printf(", ");
            }
            if(first){
                first = FALSE;
            }
            printf("%d=", i);
            Debug_Print((void *)t, 0, "", color, extended); 

            printf("\x1b[%dm", color);
        }
    }
    printf("\x1b[%dm>\x1b[0m", color);
}

static void showSlab(Slab *sl, int color, boolean extended, int indent){
    Slab_Print((Abstract *)sl, TYPE_SLAB, "", color, extended); 
    if(sl->increment > SPAN_DIM_SIZE){
        indent++;
        printf("\n");
        boolean first = TRUE;
        for(int i = 0; i < SPAN_DIM_SIZE; i++){
            Abstract *t = sl->items[i];
            if(t != NULL){
                if(!first){
                    printf("\n");
                }
                if(first){
                    first = FALSE;
                }
                indent_Print(indent);
                printf("\x1b[%dm%d=", color, i);
                showSlab((Slab *)t, color, extended, indent);
            }
        }
    }
}

static void Span_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Span *p = (Span *)as(a, TYPE_SPAN);
    printf("%s\n\x1b[;%dmP<%u items in %u dims of %lu bytes each", msg, color, 
        p->nvalues, p->dims, sizeof(Abstract *)*p->slotSize);
    printf("\n");
    indent_Print(1);
    showSlab(p->slab, color, extended, 1);
    printf("\n\x1b[1;%dm>\x1b[0m\n", color);
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

static void Parser_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Parser *prs = (Parser *)as(a, TYPE_PARSER);
    printf("\x1b[%dm%sPrs<%s", color, msg, State_ToString(prs->type.state));
    /*
    Match *mt = (Match *)prs->match.single;
    Debug_Print((void *)mt, 0, " ", color, extended);
    */
    printf(">\x1b[0m");
}

static void MultiParser_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Parser *prs = (Parser *)as(a, TYPE_MULTIPARSER);
    printf("\x1b[%dm%sPrs<%s", color, msg, State_ToString(prs->type.state));
    Match *mt = NULL;;
    int i = 0;
    /*
    while((mt = prs->match.array[i++]) != NULL){
        Debug_Print((void *)mt, 0, ", ", color, extended);
    }
    */
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
    r |= Lookup_Add(m, lk, TYPE_SLAB, (void *)Slab_Print);
    r |= Lookup_Add(m, lk, TYPE_SPAN, (void *)Span_Print);
    r |= Lookup_Add(m, lk, TYPE_PROTODEF, (void *)ProtoDef_Print);
    r |= Lookup_Add(m, lk, TYPE_ROEBLING, (void *)Roebling_Print);
    r |= Lookup_Add(m, lk, TYPE_PARSER, (void *)Parser_Print);
    r |= Lookup_Add(m, lk, TYPE_MULTIPARSER, (void *)MultiParser_Print);
    r |= Lookup_Add(m, lk, TYPE_HASHED, (void *)Hashed_Print);
    r |= Lookup_Add(m, lk, TYPE_SINGLE, (void *)Single_Print);
    r |= Lookup_Add(m, lk, TYPE_RBL_MARK, (void *)Single_Print);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_UTIL, (void *)WrappedUtil_Print);
    r |= Lookup_Add(m, lk, TYPE_XMLCTX, (void *)XmlCtx_Print);
    r |= Lookup_Add(m, lk, TYPE_MESS, (void *)Mess_Print);
    return r;
}

status Debug_Init(MemCtx *m){
    if(DebugPrintChain == NULL){
        Lookup *funcs = Lookup_Make(m, _TYPE_START, populateDebugPrint, NULL);
        DebugPrintChain = Chain_Make(m, funcs);
        return SUCCESS;
    }
    return NOOP;
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

