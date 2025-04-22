#include <external.h>
#include <caneka.h>

static const size_t PAT_FLAG_DEBUG_MAX = 17;
static char *matchFlagChars = "EXPMNICGKOSLDTU";

static char *snipChars = "________CGBU____";

static status patFlagStr(word flags, char str[]){
    return FlagStr(flags, str, matchFlagChars);
}

static i64 PatChar_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    ;
    PatCharDef *pat = (PatCharDef *)a;
    char cstr[PAT_FLAG_DEBUG_MAX];
    char ending[2] = {0, 0};
    i64 total = 0;
    patFlagStr(pat->flags, cstr);
    if(pat->flags & PAT_TERM){
        ending[0] = '.';
    }else{
        ending[0] = ',';
    }

    char *to = (char *)&pat->to;
    size_t toLen = 1;
    char *from = (char *)&pat->from;
    size_t fromLen = 1;
    if(*to == '\r'){
        to = "\\r";
        toLen = 2;
    }else if(*to == '\n'){
        to = "\\n";
        toLen = 2;
    }
    if(*from == '\r'){
        from = "\\r";
        fromLen = 2;
    }else if(*from == '\n'){
        from = "\\n";
        fromLen = 2;
    }

    if(pat->flags == PAT_END){
        void *args[] = {cstr,  NULL};
        total += StrVec_Fmt(sm, "_c", args);
    } else if(pat->to == pat->from){
        void *args[] = {cstr, from, (void *)fromLen, ending, NULL};
        total += StrVec_Fmt(sm, "_c^D._C^d._c", args);
    }else{
        void *args[] = {cstr, from, (void *)fromLen,
            to,(void *)toLen,
            ending, NULL};
        total += StrVec_Fmt(sm, "_c^D._C-_C^d._c", args);
    }
    return total;
}

static i64 PatCharDef_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    PatCharDef *pat = (PatCharDef *)a;
    i64 total = 0;
    total += StrVec_Fmt(sm, "Pat<", NULL);
    while(pat->flags != PAT_END){
        total += PatChar_Print(sm, (Abstract *)pat, TYPE_PATCHARDEF, extended);
        pat++;
    }
    total += PatChar_Print(sm, (Abstract *)pat, TYPE_PATCHARDEF, extended);
    total += StrVec_Fmt(sm, ">", NULL);
    return total;
}


static i64 SnipSpan_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    Span *sns = (Span *)as(a, TYPE_SPAN);
    i64 total = 0;
    total += StrVec_Fmt(sm, "Sns<", NULL);
    Iter it;
    Iter_Init(&it, sns);
    while((Iter_Next(&it) & END) == 0){
        Snip *sn = (Snip *)it.value; 

        char flagStr[FLAG_CSTR_LENGTH];
        memset(flagStr, 0, FLAG_CSTR_LENGTH);
        FlagStr(sn->type.state & UPPER_FLAGS, flagStr, snipChars);

        void *args[] = {flagStr, &sn->length, NULL};
        total += StrVec_Fmt(sm, "_c^D._i4^d.,", args);
    }
    total += StrVec_Fmt(sm, ">", NULL);
    return total;
}

/*
static char *flagStrs[] = {
    "PAT_TERM",
    "PAT_OPTIONAL",
    "PAT_MANY",
    "PAT_ANY",
    "PAT_INVERT",
    "PAT_COUNT",
    "PAT_INVERT_CAPTURE",
    "PAT_KO",
    "PAT_KO_TERM",
    "PAT_SINGLE",
    "PAT_LEAVE ",
    "PAT_CMD",
    "PAT_GO_ON_FAIL",
    "PAT_CONSUME",
    NULL,
};

status Match_AddFlagsToStr(MemCtx *m, String *s, word flag){
    status r = READY;
    char *cstr = "";
    boolean first = TRUE;
    if(flag == 0){
        cstr = "PAT_END";
        String_AddBytes(m, s, bytes(cstr), strlen(cstr));
        r |= SUCCESS;
    }
    for(int i = 0; i <= 13; i++){
        if((flag & (1 << i)) != 0){
            if(!first){
                cstr = "|";
                String_AddBytes(m, s, bytes(cstr), strlen(cstr));
            }
            first = FALSE;
            cstr = flagStrs[i];
            String_AddBytes(m, s, bytes(cstr), strlen(cstr));
            r |= SUCCESS;
        }
    }
    if(r == READY){
        r |= NOOP;
    }
    return r;
}
*/


void Roebling_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    DebugStack_Push("Roebling_Print", TYPE_CSTR);
    Roebling *rbl = (Roebling *) as(a, TYPE_ROEBLING);
    /*
    String *sec = Roebling_GetMarkDebug(rbl, rbl->idx);
    printf("\x1b[%dm%sRbl<%s\x1b[1;%dm%s\x1b[0;%dm source=%u", color, msg, State_ToChars(rbl->type.state), color, String_ToChars(DebugM, sec), color, rbl->source != NULL ? rbl->source->type.of: 0);
    printf(":");
    if(extended){
        printf(" idx:%d jump:%d ", rbl->idx, rbl->jump);
        Debug_Print((void *)&(rbl->cursor), 0, "Cursor: ", color, extended);
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
    */
    DebugStack_Pop();
    return;
}


void Match_PrintPat(Stream *sm, Abstract *a, cls type, boolean extended){
    Match *mt = (Match *)as(a, TYPE_PATMATCH);
    /*
    if(extended){
        printf("\x1b[%dm%sMatch<%s:state=%s:jump=%d:remainig=%d:%d/%d", color, msg,
            State_ToChars(mt->type.state), State_ToChars(mt->type.state), mt->jump, mt->remaining, mt->snip.start, mt->snip.length);
        Debug_Print(mt->backlog, TYPE_STRSNIP_STRING, ":backlog=(", color, FALSE);
        printf(") \x1b[1;%dm[", color);
        Debug_Print((void *)mt->pat.curDef, TYPE_PATCHARDEF, "", color, FALSE);
        printf("\x1b[1;%dm] \x1b[0;%dm ", color, color);
        Debug_Print((void *)mt->pat.startDef, TYPE_PATCHARDEF, "", color, TRUE);
        printf("\x1b[%dm>\x1b[0m", color);
    }else{
        printf("\x1b[%dm%sMatch<state=%s ", color, msg, State_ToChars(mt->type.state));
        Debug_Print((void *)mt->pat.startDef, TYPE_PATCHARDEF, "", color, TRUE);
        printf("\x1b[%dm>\x1b[0m", color);
    }
    */
}

status ParserDebug_Init(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_PATMATCH, (void *)Match_PrintPat);
    r |= Lookup_Add(m, lk, TYPE_PATCHARDEF, (void *)PatCharDef_Print);
    r |= Lookup_Add(m, lk, TYPE_PATMATCH, (void *)Match_PrintPat);
    r |= Lookup_Add(m, lk, TYPE_ROEBLING, (void *)Roebling_Print);
    r |= Lookup_Add(m, lk, TYPE_SNIPSPAN, (void *)SnipSpan_Print);
    return r;
}
