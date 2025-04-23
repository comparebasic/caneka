#include <external.h>
#include <caneka.h>

static char *matchFlagChars = "EXPMNICGKOSLDTU__";

static char *snipChars = "________CGBU____";

static i64 _PatChar_print(Stream *sm, Abstract *a, cls type, boolean extended){
    PatCharDef *pat = (PatCharDef *)a;
    char cstr[FLAG_DEBUG_MAX];
    char ending[2] = {0, 0};
    i64 total = 0;
    PatFlagCstr(pat->flags, cstr);
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
    }else if(*to == '\t'){
        to = "\\t";
        toLen = 2;
    }else if(*to < 32 || *to > 126){
        Str *s = Str_Make(_debugM, MAX_BASE10+2);
        Str_Add(s, (byte *)"\\", 1);
        Str_AddI64(s, *to);
        to = (char *)s->bytes;
        toLen = s->length;
    }
    if(*from == '\r'){
        from = "\\r";
        fromLen = 2;
    }else if(*from == '\n'){
        from = "\\n";
        fromLen = 2;
    }else if(*from == '\t'){
        from = "\\t";
        fromLen = 2;
    }else if(*from < 32 || *from > 126){
        Str *s = Str_Make(_debugM, MAX_BASE10+2);
        Str_Add(s, (byte *)"\\", 1);
        Str_AddI64(s, *from);
        from = (char *)s->bytes;
        fromLen = s->length;
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

static i64 PatChar_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    i64 total = 0;
    PatCharDef *pat = (PatCharDef *)a;
    if(extended){
        total += StrVec_Fmt(sm, "P<", NULL);
    }
    total += _PatChar_print(sm, (Abstract *)pat, TYPE_PATCHARDEF, extended);
    if(extended){
        total += StrVec_Fmt(sm, ">", NULL);
    }
    return total;
}

static i64 PatCharDef_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    PatCharDef *pat = (PatCharDef *)a;
    i64 total = 0;
    total += StrVec_Fmt(sm, "Pat<", NULL);
    while(pat->flags != PAT_END){
        total += _PatChar_print(sm, (Abstract *)pat, TYPE_PATCHARDEF, extended);
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

        char flagStr[FLAG_DEBUG_MAX];
        memset(flagStr, 0, FLAG_DEBUG_MAX);
        FlagStr(sn->type.state & UPPER_FLAGS, flagStr, snipChars);

        void *args[] = {flagStr, &sn->length, NULL};
        total += StrVec_Fmt(sm, "_c^D._i4^d.,", args);
    }
    total += StrVec_Fmt(sm, ">", NULL);
    return total;
}

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

i64 Match_Print(Stream *sm, Abstract *a, cls type, boolean extended){
    i64 total = 0;
    Match *mt = (Match *)as(a, TYPE_PATMATCH);
    void *args[] = {State_ToStr(_debugM, mt->type.state), NULL};
    total += StrVec_Fmt(sm, "Mt<_t ", args);

    if(extended){
        PatCharDef *pd = mt->pat.startDef;
        while(pd->flags != PAT_END){
            char *color = "p.";
            if(pd == mt->pat.curDef){
                color = "y.";
            }
            void *args[] = {color, pd, (void *)((i64)TYPE_PATCHAR), NULL};
            Out("_^_T", args);
            pd++;
        }
        StrVec_Fmt(sm, "^d", NULL);
    }

    total += StrVec_Fmt(sm, "^p.>", NULL);
    return total;
}

i64 PatFlagCstr(word flags, char *cstr){
    return FlagStr(flags, cstr,  matchFlagChars);
}

status ParserDebug_Init(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_PATMATCH, (void *)Match_Print);
    r |= Lookup_Add(m, lk, TYPE_PATCHARDEF, (void *)PatCharDef_Print);
    r |= Lookup_Add(m, lk, TYPE_PATCHAR, (void *)PatChar_Print);
    r |= Lookup_Add(m, lk, TYPE_ROEBLING, (void *)Roebling_Print);
    r |= Lookup_Add(m, lk, TYPE_SNIPSPAN, (void *)SnipSpan_Print);
    return r;
}
