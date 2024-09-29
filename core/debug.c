#include <external.h>
#include <caneka.h>

Chain *DebugPrintChain = NULL;

int DEBUG_SCURSOR = 0;
int DEBUG_MATCH = 0;
int DEBUG_PATMATCH = COLOR_DARK;
int DEBUG_MATCH_COMPLETE = COLOR_YELLOW;
int DEBUG_CURSOR = 0;
int DEBUG_PARSER = 0;
int DEBUG_ROEBLING = COLOR_BLUE;
int DEBUG_ROEBLING_MARK = 0;
int DEBUG_ROEBLING_COMPLETE = COLOR_CYAN;
int DEBUG_ROEBLING_CONTENT = 0;
int DEBUG_ROEBLING_CURRENT = 0;
int DEBUG_ALLOC = 0;
int DEBUG_BOUNDS_CHECK = 0;
int DEBUG_TABLE = 0;
int DEBUG_SPAN = 0;
int DEBUG_XML = COLOR_CYAN;
int DEBUG_ROEBLING_NAME = 0;

static MemCtx *DebugM = NULL;

static Abstract *Print(MemCtx *m, Abstract *a){
    Debug_Print(a, 0, "", 0, TRUE);
    return NULL;
}

static void Slab_Print(void *sl, SpanDef *def, int color, byte dim, int parentIdx, byte indent, boolean extended, byte totalDims, int offset){
    if(dim == 0){
        indent_Print(indent);
        printf("%d(%d)= ", parentIdx,offset);
        if(extended){
            printf("#%p->", sl);
        }
        printf("Values[ ");
        void *ptr = sl;
        boolean any = FALSE;
        for(int i = 0; i < def->stride; i++){
            util *a = (util *)ptr;
            if(*a != 0){
                printf("%d=", i);
                Abstract *t = (Abstract *)ptr;
                if(HasFlag(def->flags, INLINE)){
                    t = *((Abstract **)t);
                }
                if(!HasFlag(def->flags, RAW)){
                    i64 n = (util)t;
                    if(def->itemSize == sizeof(int)){
                        printf("%u", (int)n);
                    }else{
                        printf("%lu", n);
                    }
                }else{
                    if(t->type.of != 0){
                        Debug_Print((void *)t, 0, "", color, TRUE);
                    }else{
                        printf("0x%lx", *a);
                    }
                }
                printf(" ");
            }
            ptr += sizeof(void *)*def->slotSize;
        }
        printf("]");
    }else{
        indent_Print(indent);
        printf("%d=", parentIdx);
        if(extended){
            printf("#%p->", sl);
        }
        printf("Idx(%d/%d)[ ",  dim, Span_availableByDim(dim, def->stride, def->idxStride));
        boolean any = FALSE;
        void *ptr = sl;
        for(int i = 0; i < def->idxStride; i++){
            util *a = (util *)ptr;
            if(*a != 0){
                int increment = Span_availableByDim(dim, def->stride, def->idxStride);
                util pos = offset+increment*i;
                printf("%d=%ld..%ld", i, pos, pos+(increment-1));
                if(i < def->idxStride - 1){
                    printf(" ");
                }
            }
            ptr += sizeof(void *)*def->idxSize;
        }
        ptr = sl;
        any = FALSE;
        for(int i = 0; i < def->idxStride; i++){
            util *a = (util *)ptr;
            if(*a != 0){
                printf("\n");
                offset += Span_availableByDim(dim, def->stride, def->idxStride)*i;
                Slab_Print((void *)*a, def, color, dim-1, i, indent+1, extended, totalDims, offset);
                any = TRUE;
            }
            ptr += sizeof(void *)*def->idxSize;
        }
        if(any){
            printf("\n");
        }
        indent_Print(indent);
        printf("]");
        if(any){
            printf("\n");
        }
    }
}

static void SpanDef_Print(SpanDef *def){
    char *flags = "";
    if(HasFlag(def->flags, INLINE)){
        flags = "(inline)";
    }
    printf("def=[idxStride:%d stride:%d idxSize:%d slotSize:%d%s itemSize:%d, valueHdr:%d", 
         def->idxStride, def->stride, def->idxSize, def->slotSize, flags, def->itemSize, def->valueHdr);
    if(def->dim_lookups[0] != 0){
        printf(" lookups: ");
        for(int i = 0; i < 8; i ++){
            printf("%lu ", def->dim_lookups[i]);
        }
    }
    printf("]");
}

void Span_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Span *p = (Span *)asIfc(a, TYPE_SPAN);
    printf("\x1b[0;%dm%sP<%u items in %u dims ", color, msg,
        p->nvalues, p->dims);
    if(extended){
        SpanDef_Print(p->def);
        printf("\n");
        Slab_Print(p->root, p->def, color, p->dims, 0, 1, TRUE, p->dims, 0);
    }
    printf(">\x1b[0m");
}

status PrintMatchAddr(MemHandle *mh){
    MHAbstract *ma = (MHAbstract *)as(mh, TYPE_MHABSTRACT);
    if(ma->a == NULL){
        printf("NULL ");
    }else{
        printf("0x%lx ", (util)(ma->a));
    }
    return SUCCESS;
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
    if((flags & PAT_CONSUME) != 0){
        str[i++] = 'O';
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
        str[i++] = 'U';
    }
    if((flags & PAT_NO_CAPTURE) != 0){
        str[i++] = 'G';
    }
    if((flags & PAT_SINGLE) != 0){
        str[i++] = 'S';
    }
    if((flags & PAT_KO) != 0){
        str[i++] = 'K';
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
            printf("%s%s=%c/%hu", msg, flag_cstr, (char)def->from, (i8)def->from);
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
    printf("\x1b[%dm%s", color, msg);
    if(extended){
        boolean first = TRUE;
        while(def->flags != PAT_END){
            if(first){
                first = FALSE;
            }else{
                printf(",");
            }
            patCharDef_PrintSingle(def, TYPE_PATCHARDEF, "", color, extended);
            def++;
        }
    }else{
        patCharDef_PrintSingle(def, TYPE_PATCHARDEF, msg, color, extended);
    }
    printf("\x1b[0m");
}

static void Match_PrintPat(Abstract *a, cls type, char *msg, int color, boolean extended){
    Match *mt = (Match *)as(a, TYPE_PATMATCH);
    if(extended){
        printf("\x1b[%dm%sMatch<%s:state=%s:jump=%d:count=%d:remainig=%d ", color, msg, Class_ToString(mt->type.of), State_ToString(mt->type.state), mt->jump, mt->count, mt->remaining);
        printf("\x1b[1;%dm[", color);
        Debug_Print((void *)mt->def.pat.curDef, TYPE_PATCHARDEF, "", color, FALSE);
        printf("\x1b[1;%dm] \x1b[0;%dm ", color, color);
        Debug_Print((void *)mt->def.pat.startDef, TYPE_PATCHARDEF, "", color, TRUE);
        printf("\x1b[%dm>\x1b[0m", color);
    }else{
        printf("\x1b[%dm%sMatch<state=%s>\x1b[0m", color, msg, State_ToString(mt->type.state));
    }
}


static void mess_PrintRecurse(Mess *ms, char *msg, int color, boolean extended, int indent){
    indent_Print(indent);
    printf("\x1b[%dm%sM<%s value=\x1b[0m", color, msg, Class_ToString(ms->type.of));
    Debug_Print((void *)ms->value, 0, "", color, extended);
    Hashed *h = ms->atts;
    printf(" ");
    if(h != NULL){
        printf("\x1b[%dmatts=[", color);
    }
    while(h != NULL){
        Debug_Print((void *)h, 0, "", color, extended);
        h = h->next;
        if(h != NULL){
            printf("\x1b[%dm, ", color);
        }
    }
    if(ms->atts != NULL){
        printf("\x1b[%dm]", color);
    }
    if(ms->body != NULL){
        Debug_Print(ms->body, 0, "body=", color, TRUE);
    }
    Mess *child = ms->firstChild;
    if(child != NULL){
        printf("\x1b[%dm [", color);
        printf("\n");
    }
    while(child != NULL){
        mess_PrintRecurse(child, msg, color, extended, indent+1);
        child = child->next;
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
        printf("%sMatch<%s:state=%s'\x1b[%d;1m%s\x1b[0;%dm'>", msg, Class_ToString(mt->type.of), State_ToString(mt->type.state), color, mt->def.str.s->bytes, color);
    }else{
        char *jump = "";
        printf("%sMatch<state=%s", msg, State_ToString(mt->type.state));
        if(mt->jump >= 0){
            printf(":jmp=%d\n", mt->jump);
        }
        printf("\x1b[%dm>\x1b[0m\n", color);
    }
}

static void Single_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Single *v = (Single *)a;
    printf("\x1b[%dm%sSingle<%s:%u:%ld>\x1b[0m", color, msg, Class_ToString(v->type.of), v->type.state, v->val.value);
}

static void StringMatch_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Match *mt = (Match *)as(a, TYPE_STRINGMATCH);
    if(extended){
        printf("\x1b[%dm%sMatch<%s:state=%s:pos=%d:jmp=%d:count=%d:remainig=%d ", color, msg, Class_ToString(mt->type.of), State_ToString(mt->type.state), mt->def.str.position, mt->jump, mt->count, mt->remaining);
        Debug_Print((void *)mt->def.str.s, 0, "", color, FALSE);
        printf(">\x1b[0m");
    }else{
        printf("\x1b[%dm%sMatch<state=%s:pos=%d ", color, msg, State_ToString(mt->type.state), mt->def.str.position);
        Debug_Print((void *)mt->def.str.s, 0, "", color, FALSE);
        printf(">\x1b[0m");
    }
}

static void StringFixed_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    String *s = (String *)as(a, TYPE_STRING_FIXED);
    String *esc = String_ToEscaped(DebugM, s);
    if(extended){
        char *state = "";
        if(s->type.state != 0){
            state = State_ToString(s->type.state);
        }
        printf("%s\x1b[%dmSFixed<%s\x1b[0;%dm", msg, color,state, color);
        printf("s/%u=\"\x1b[1;%dm%s\x1b[0;%dm\"", s->length, color, esc->bytes, color);
        printf("\x1b[%dm>\x1b[0m", color);
    }else{
        printf("\x1b[%dm%s\"\x1b[1;%dm%s\x1b[0;%dm\"", color, msg, color, esc->bytes, color);
    }
}

static void Roebling_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Roebling *rbl = (Roebling *) as(a, TYPE_ROEBLING);
    printf("\x1b[%dm%sRbl<%s:source=%u", color, msg, State_ToString(rbl->type.state), rbl->source != NULL ? rbl->source->type.of: 0);
    printf(":");
    if(extended){
        printf(" idx:%d jump:%d ", rbl->idx, rbl->jump);
        Debug_Print((void *)&(rbl->range), 0, "", color, extended);
        if(rbl->marks->values->nvalues > 0){
            printf("\n  \x1b[%dmmarks=\n    ", color);
            for(int i = 0; i <= rbl->marks->values->max_idx; i++){
                Debug_Print((void *)Span_Get(rbl->markLabels->values, i), 0, "", color, FALSE);
                printf("=");
                int *n = Span_Get(rbl->marks->values, i);
                if(n != NULL){
                    printf("%u", *n);
                }
                printf(" ");
            }
        }
        printf("\n  \x1b[%dmmatches=\n", color);
        Match *mt = NULL;
        for(int i = 0; i < rbl->matches->nvalues; i++){
            mt = Span_Get(rbl->matches, i);
            Debug_Print((void *)mt, 0, "    ", color, extended);
            if(i < rbl->matches->nvalues-1){
                printf(",\n");
            }
        }
        printf("\n  \x1b[%dmParsers(DoF)=\n    ", color);
        Span_Run(NULL, rbl->parsers_do, PrintMatchAddr);
        printf("\n");
    }else{
        printf(" idx:%d ", rbl->idx);
        Debug_Print((void *)&(rbl->range), 0, "", color, extended);
    }
    printf("\x1b[%dm>\x1b[0m", color);
}

static void String_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    String *s = (String *)as(a, TYPE_STRING_CHAIN);
    if(extended){
        char *state = "";
        if(s->type.state != 0){
            state = State_ToString(s->type.state);
        }
        printf("%s\x1b[%dmS<%s\x1b[0;%dm", msg, color, state, color);
        do {
            String *esc = String_ToEscaped(DebugM, s);
            printf("s/%u=\"\x1b[1;%dm%s\x1b[0;%dm\"", s->length, color, esc->bytes, color);
            s = s->next;
        } while(s != NULL);
        printf("\x1b[%dm>\x1b[0m", color);
    }else{
        printf("\x1b[1;%dm\"", color);
        do {
            String *esc = String_ToEscaped(DebugM, s);
            printf("%s",esc->bytes);
            s = s->next;
        } while(s != NULL);
        printf("\"\x1b[0m");
    }
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
    printf("\x1b[%dm%sC<%s:\x1b[1;%dm%s\x1b[0;%dm[%c@%ld]>", color, msg, State_ToString(sc->type.state), color, sc->seg != NULL ? (char *)(String_ToEscaped(DebugM, sc->seg)->bytes) : "NULL", color, sc->seg != NULL ? sc->seg->bytes[sc->position] : '?',  sc->position);
}

static void Range_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Range *range = (Range *)a;
    printf("%s\x1b[%dmR<", msg, color);
    if(extended){
        printf("s=");
        Debug_Print((void *)range->search, 0, "", color, extended);
        printf(" ");
    }
    String *s = Range_Copy(DebugM, range);
    Debug_Print((void *)s, 0, " current=", color, extended);
    if(extended){
        printf("\n  \x1b[%dmCursors=\n    ", color);
        Debug_Print((void *)&(range->start), TYPE_SCURSOR, "S=", color, extended);
        Debug_Print((void *)&(range->potential), TYPE_SCURSOR, "P=", color, extended);
        Debug_Print((void *)&(range->end), TYPE_SCURSOR, "E=", color, extended);
        printf("\x1b[%dm", color);
    }
    printf("\x1b[%dm>\x1b[0m", color);
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
    r |= Lookup_Add(m, lk, TYPE_SPAN, (void *)Span_Print);
    r |= Lookup_Add(m, lk, TYPE_TABLE, (void *)Span_Print);
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
    DebugM = m;
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


void Match_midDebug(char type, word c, PatCharDef *def, Match *mt, boolean matched){
    if(c == '\n'){
        printf("\x1b[%dm  %c \x1b[0;1m'\\n'\x1b[0;%dm=%s - [count:%d lead:%d tail:%d] %s ", DEBUG_PATMATCH, type,
            DEBUG_PATMATCH, matched ? "matched" : "no-match", mt->count, mt->lead, mt->tail, State_ToString(mt->type.state));
    }else if(c == '\r'){
        printf("\x1b[%dm  %c \x1b[0;1m'\\r'\x1b[0;%dm=%s - [count:%d lead:%d tail:%d] %s ", DEBUG_PATMATCH, type,
            DEBUG_PATMATCH, matched ? "matched" : "no-match", mt->count, mt->lead, mt->tail, State_ToString(mt->type.state));
    }else{
        printf("\x1b[%dm  %c \x1b[0;1m'%c'\x1b[0;%dm=%s - [count:%d lead:%d tail:%d] %s ", DEBUG_PATMATCH, type,
            c, DEBUG_PATMATCH,  matched ? "matched" : "no-match", mt->count, mt->lead, mt->tail, State_ToString(mt->type.state));
    }
    Debug_Print((void *)def, TYPE_PATCHARDEF, "", DEBUG_PATMATCH, FALSE);
    printf("\n");
}


