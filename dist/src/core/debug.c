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
int DEBUG_ROEBLING_COMPLETE = COLOR_CYAN;
int DEBUG_ROEBLING_CONTENT = 0;
int DEBUG_ROEBLING_CURRENT = 0;
int DEBUG_ALLOC = 0;
int DEBUG_BOUNDS_CHECK = 0;
int DEBUG_TABLE = 0;
int DEBUG_SPAN = 0;
int DEBUG_SPAN_GET_SET = 0;
int DEBUG_XML = 0;
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
int DEBUG_REQ_RECV = COLOR_BLUE;
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

char *TypeStrings[] = {
    "_TYPE_START,"
    "TYPE_UNKNOWN",
    "TYPE_BLANK",
    "TYPE_ABSTRACT",
    "TYPE_BUILDER",
    "TYPE_RESERVE",
    "TYPE_WRAPPED",
    "TYPE_WRAPPED_FUNC",
    "TYPE_WRAPPED_DO",
    "TYPE_WRAPPED_UTIL",
    "TYPE_WRAPPED_I64",
    "TYPE_WRAPPED_TIME64",
    "TYPE_WRAPPED_BOOL",
    "TYPE_WRAPPED_PTR",
    "TYPE_UTIL",
    "TYPE_UNIT",
    "TYPE_MEMCTX",
    "TYPE_MEMHANDLE",
    "TYPE_MHABSTRACT",
    "TYPE_MEMLOCAL",
    "TYPE_MESS",
    "TYPE_MAKER",
    "TYPE_MEMSLAB",
    "TYPE_STRING",
    "TYPE_STRING_CHAIN",
    "TYPE_STRING_FIXED",
    "TYPE_STRING_FULL",
    "TYPE_STRING_SLAB",
    "TYPE_STRSNIP",
    "TYPE_STRSNIP_STRING",
    "TYPE_CURSOR",
    "TYPE_TESTSUITE",
    "TYPE_RESULT",
    "TYPE_PARSER",
    "TYPE_ROEBLING",
    "TYPE_MULTIPARSER",
    "TYPE_SCURSOR",
    "TYPE_GUARD",
    "TYPE_PATMATCH",
    "TYPE_PATCHARDEF",
    "TYPE_STRUCTEXP",
    "TYPE_IOCTX",
    "TYPE_ACCESS",
    "TYPE_AUTH",
    "_TYPE_SPAN_START",
    "TYPE_SPAN",
    "TYPE_MINI_SPAN",
    "TYPE_MEM_SPAN",
    "TYPE_QUEUE_SPAN",
    "TYPE_QUEUE_IDX",
    "TYPE_SLAB_SPAN",
    "TYPE_STRING_SPAN",
    "TYPE_NESTED_SPAN",
    "TYPE_TABLE",
    "TYPE_POLL_MAP_SPAN",
    "_TYPE_SPAN_END",
    "TYPE_STRRAY",
    "TYPE_COORDS",
    "TYPE_MEM_KEYED",
    "TYPE_SPAN_STATE",
    "TYPE_SPAN_DEF",
    "TYPE_QUEUE",
    "TYPE_CHAIN",
    "TYPE_SPAN_QUERY",
    "TYPE_SLAB",
    "TYPE_TABLE_CHAIN",
    "TYPE_HASHED",
    "TYPE_HASHED_LINKED",
    "TYPE_MEMPAIR",
    "TYPE_STRINGTABLE",
    "TYPE_FILE",
    "TYPE_SPOOL",
    "TYPE_LOOKUP",
    "TYPE_ITER",
    "TYPE_ITER_STRING",
    "TYPE_SINGLE",
    "TYPE_RBL_MARK",
    "TYPE_OSET_ITEM",
    "TYPE_FMT_ITEM",
    "TYPE_FMT_DEF",
    "TYPE_OSET",
    "TYPE_FMT_CTX",
    "TYPE_OSET_DEF",
    "TYPE_CASH",
    "TYPE_XMLT",
    "TYPE_NESTEDD",
    "TYPE_ENC_PAIR",
    "TYPE_FMT_HTML",
    "TYPE_TRANSP",
    "TYPE_DEBUG_STACK",
    "TYPE_DEBUG_STACK_ENTRY",
    "_TYPE_CORE_END",
    NULL,
};

char *Class_ToString(cls type){
    if(type <= _TYPE_CORE_END){
       return TypeStrings[type]; 
    }else{
        return "TYPE_unknown";
    }
}

boolean SHOW_SERVE_TESTS = FALSE;

MemCtx *DebugM = NULL;

static Abstract *Print(MemCtx *m, Abstract *a){
    Debug_Print(a, 0, "", 0, TRUE);
    return NULL;
}

char *State_ToChars(status state){
    return String_ToChars(DebugM, State_ToString(DebugM, state));
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

void spanState_Print(SpanState *st, int color){
    printf("\x1b[%dmST<%p localIdx:%hu increment:%d offset:%hu dim:%d>\x1b[0m", color, st->slab, st->localIdx, st->increment, st->offset, st->dim);
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

void SpanState_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    SpanState *st = (SpanState *)a;
    printf("\x1b[%dm%s", color, msg);
    spanState_Print(st, color);
}

void SpanQuery_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    SpanQuery *sq = as(a, TYPE_SPAN_QUERY);
    printf("\x1b[%dm%sSQ<%s idx:%d op:%d dims:%hu/%hu", color, msg, State_ToChars(sq->type.state), sq->idx, sq->op, sq->dims, sq->dimsNeeded);
    SpanState *st = sq->stack;
    for(int i = 0; i <= sq->span->dims; i++){
        printf("\n");
        indent_Print(1);
        printf("\x1b[%dm%d: ", color, i);
        spanState_Print(st, color);
        st++;
    }
    printf("\n");
    printf("\x1b[%dm>\x1b[0m", color);
}

static void Slab_Print(void *sl, SpanDef *def, int color, byte dim, int parentIdx, byte indent, boolean extended, byte totalDims, int offset){
    indent_Print(indent);
    printf("%d(%d)= ", parentIdx,offset);
    if(dim == 0){
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
                if(def->typeOf == TYPE_QUEUE_SPAN){
                    QueueIdx *qidx = (QueueIdx *)t;
                    Debug_Print((void *)qidx->item, 0, "", color, TRUE);
                    printf("\x1b[%dm", color);
                }else{
                    if((def->flags & SPAN_INLINE) == 0){
                        t = *((Abstract **)t);
                    }
                    if((def->flags & SPAN_RAW) == 0){
                        i64 n = (util)t;
                        if(def->itemSize == sizeof(int)){
                            printf("%u", (int)n);
                        }else if(t->type.of != 0){
                            Debug_Print((void *)t, 0, "", color, TRUE);
                        }else{
                            printf("0x%lx", *a);
                        }
                    }
                }
                printf(" ");
            }
            ptr += sizeof(void *)*def->slotSize;
        }
        printf("]");
    }else{
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
                if(def->typeOf == TYPE_QUEUE_SPAN){
                    QueueIdx *qidx = (QueueIdx *)a;
                    printf("%d=%ld..%ld!%s/%d", i, pos, pos+(increment-1), QueueFlags_ToChars(qidx->flags), qidx->delayTicks);
                }else{
                    printf("%d=%ld..%ld", i, pos, pos+(increment-1));
                }
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

void SpanDef_Print(SpanDef *def){
    char *flags = "";
    if((def->flags & SPAN_INLINE) != 0){
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
        if(p->def->typeOf == TYPE_QUEUE_SPAN){
            printf("%s ", QueueFlags_ToChars(p->flags));
        }
        SpanDef_Print(p->def);
        printf("\n");
        Slab_Print(p->root, p->def, color, p->dims, 0, 1, TRUE, p->dims, 0);
    }
    printf(">\x1b[0m");
}

void Lookup_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Lookup *lk = (Lookup *)as(a, TYPE_LOOKUP);
    printf("\x1b[%dm%sLk<offset:%d latest_idx:\%d", color, msg, lk->offset, lk->latest_idx);
    Debug_Print((void *)lk->values, 0, "", color, TRUE);
    printf("\x1b[%dm>\x1b[0m", color);
}

status PrintMatchAddr(MemCtx *m, Abstract *a){
    if(a == NULL){
        printf("NULL ");
    }else{
        printf("0x%lx ", (util)(a));
    }
    return SUCCESS;
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

static void StrSnipString_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    String *s = (String *)asIfc(a, TYPE_STRING);
    IterStr it;
    IterStr_Init(&it, s, sizeof(StrSnip));
    printf("\x1b[%dm%s", color, msg);
    while((IterStr_Next(&it) & END) == 0){
        StrSnip *sn = (StrSnip *)IterStr_Get(&it);
        if((sn->type.state & SUCCESS) != 0){
            printf("%d/%d", sn->start, sn->length);
        }else{
            printf("gap(%d/%d)", sn->start, sn->length);
        }
        if((it.type.state & FLAG_ITER_LAST) == 0){
            printf(",");
        }else{
            printf("\x1b[0m");
        }
    }
}

static void Match_PrintPat(Abstract *a, cls type, char *msg, int color, boolean extended){
    Match *mt = (Match *)as(a, TYPE_PATMATCH);
    if(extended){
        printf("\x1b[%dm%sMatch<%s:state=%s:jump=%d:remainig=%d:%d/%d", color, msg,
            State_ToChars(mt->type.state), State_ToChars(mt->type.state), mt->jump, mt->remaining, mt->snip.start, mt->snip.length);
            Debug_Print(mt->backlog, TYPE_STRSNIP_STRING, ":backlog=", color, FALSE);
        printf("\x1b[1;%dm[", color);
        Debug_Print((void *)mt->pat.curDef, TYPE_PATCHARDEF, "", color, FALSE);
        printf("\x1b[1;%dm] \x1b[0;%dm ", color, color);
        Debug_Print((void *)mt->pat.startDef, TYPE_PATCHARDEF, "", color, TRUE);
        printf("\x1b[%dm>\x1b[0m", color);
    }else{
        printf("\x1b[%dm%sMatch<state=%s>\x1b[0m", color, msg, State_ToChars(mt->type.state));
    }
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

static void Cursor_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Cursor *cur = (Cursor *)as(a, TYPE_CURSOR);
    printf("\x1b[%dm%sCursor<%s ptr=%c offset=%ld local=%ld", color, msg,
        State_ToChars(cur->type.state), (cur->ptr != NULL ? *(cur->ptr) : 0), 
        cur->offset, cur->local);
    if(extended){
        Debug_Print((void*)cur->seg, 0, " seg=", color, extended);
    }
    printf("\x1b[%dm>\x1b[0m", color);
}

static void Result_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Result *v = (Result *)a;
    printf("\x1b[%dm%sR<%s:\x1b[1;%dm%d\x1b[0;%dm:", color, msg, State_ToChars(v->type.state), color, v->range, color);
    Debug_Print((void *)v->s, 0, "", color, FALSE);
    printf("\x1b[%dm>\x1b[0m", color);
}

static void StringFixed_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    String *s = (String *)as(a, TYPE_STRING_FIXED);
    String *esc = String_ToEscaped(DebugM, s);
    if(extended){
        char *state = "";
        if(s->type.state != 0){
            state = State_ToChars(s->type.state);
        }
        printf("%s\x1b[%dmSFixed<%s\x1b[0;%dm", msg, color,state, color);
        printf("s/%u=\"\x1b[1;%dm%s\x1b[0;%dm\"", s->length, color, esc->bytes, color);
        printf("\x1b[%dm>\x1b[0m", color);
    }else{
        printf("\x1b[%dm%s\"\x1b[1;%dm%s\x1b[0;%dm\"\x1b[0m", color, msg, color, esc->bytes, color);
    }
}

static void StringFull_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    String *s = (String *)as(a, TYPE_STRING_FULL);
    String *esc = String_ToEscaped(DebugM, s);
    if(extended){
        char *state = "";
        if(s->type.state != 0){
            state = State_ToChars(s->type.state);
        }
        printf("%s\x1b[%dmSFull<%s\x1b[0;%dm", msg, color,state, color);
        printf("s/%u=\"\x1b[1;%dm%s\x1b[0;%dm\"", s->length, color, esc->bytes, color);
        printf("\x1b[%dm>\x1b[0m", color);
    }else{
        printf("\x1b[%dm%s\"\x1b[1;%dm%s\x1b[0;%dm\"\x1b[0m", color, msg, color, esc->bytes, color);
    }
}


static void Roebling_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    Roebling *rbl = (Roebling *) as(a, TYPE_ROEBLING);
    printf("\x1b[%dm%sRbl<%s:source=%u", color, msg, State_ToChars(rbl->type.state), rbl->source != NULL ? rbl->source->type.of: 0);
    printf(":");
    if(extended){
        printf(" idx:%d jump:%d ", rbl->idx, rbl->jump);
        Debug_Print((void *)&(rbl->cursor), 0, "", color, extended);
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
        Debug_Print((void *)&(rbl->cursor), 0, "", color, extended);
    }
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

#ifdef DEBUG_STACK
static void DebugStackEntry_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
   if(a->type.of != TYPE_DEBUG_STACK_ENTRY){
        printf("Error debug stack entry of incorrect cls, have %d\n", a->type.of);
        exit(1);
   }
   DebugStackEntry *e = (DebugStackEntry *)a;
   if((e->type.state & ERROR) != 0){
        color = COLOR_RED;
   }
   printf("\x1b[%dm%sStack<%s", color, msg, State_ToChars(e->type.state));
   Debug_Print((void *)&e->name, 0, ":", color, FALSE);
   Debug_Print((void *)&e->file, 0, ":", color, FALSE);
   printf("\x1b[%dm line:%d\x1b[0m", color, e->line);
   if(e->a != NULL){
        Debug_Print((void *)e->a, 0, "  ref:", color, (e->type.state & ERROR) != 0);
   }
   printf("\x1b[%dm>\x1b[0m", color);
}
#endif

static void String_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    String *s = (String *)as(a, TYPE_STRING_CHAIN);
    if(extended){
        char *state = "";
        if(s->type.state != 0){
            state = State_ToChars(s->type.state);
        }
        printf("%s\x1b[%dmS<%s\x1b[0;%dm", msg, color, state, color);
        do {
            int l = s->length;
            String *_s = s;
            if((s->type.state & FLAG_STRING_BINARY) != 0){
                _s = String_ToHex(DebugM, s);
            }else{
                _s = String_ToEscaped(DebugM, s);
            }
            printf("s/%u=\"\x1b[1;%dm", l, color);
            do {
                printf("%s", _s->bytes);
            } while((_s = String_Next(_s)) != NULL);
            printf("\x1b[0;%dm\"", color);
            s = s->next;
        } while(s != NULL);
        printf("\x1b[%dm>\x1b[0m", color);
    }else{
        printf("\x1b[%dm%s\x1b[1;%dm\"",color, msg, color);
        do {
            printf("%d/%lu'%s'",s->length,strlen((char *)s->bytes),s->bytes);
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
    r |= Lookup_Add(m, lk, TYPE_DEBUG_STACK_ENTRY, (void *)DebugStackEntry_Print);
    r |= Lookup_Add(m, lk, TYPE_FMT_DEF, (void *)FmtDef_Print);
    r |= Lookup_Add(m, lk, TYPE_RESULT, (void *)Result_Print);
    r |= Lookup_Add(m, lk, TYPE_CURSOR, (void *)Cursor_Print);
    r |= Lookup_Add(m, lk, TYPE_STRSNIP_STRING, (void *)StrSnipString_Print);
    
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


void Match_midDebug(char type, word c, PatCharDef *def, Match *mt, boolean matched, boolean extended){
    if(extended){
        printf("    \x1b[%dm%s ", DEBUG_PATMATCH, 
        State_ToChars(mt->type.state));
        Debug_Print((void *)def, TYPE_PATCHARDEF, "", DEBUG_PATMATCH, FALSE);
    }else{
        if(matched){
            printf("\x1b[1;%dmY\x1b[%dm/\x1b[0m", DEBUG_PATMATCH, DEBUG_PATMATCH);
        }else{
            printf("\x1b[%dmN/\x1b[0m", DEBUG_PATMATCH);
        }
        Debug_Print((void *)def, TYPE_PATCHARDEF, "", DEBUG_PATMATCH, FALSE);
        if(matched){
            printf("\x1b[%dm, \x1b[0m", DEBUG_PATMATCH);
        }else{
            printf("\x1b[%dm, \x1b[0m", DEBUG_PATMATCH);
        }
    }
}


