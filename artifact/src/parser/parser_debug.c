#include <external.h>
#include <caneka.h>

static char *matchFlagChars = "EXPMNICGKOSLDTU__";

static char *snipChars = "_________CGBU_____";

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
    i64 total = 0;
    void *args1[] = {
        State_ToStr(_debugM, rbl->type.state),
        &rbl->parseIt.idx,
        rbl->parseIt.value,
        NULL
    };
    total += StrVec_Fmt(sm, "Rbl<_t idx:_i4/_D\n", args1);
    void *args2[] = {rbl->parseIt.span, NULL};
    total += StrVec_Fmt(sm, "  parsers:_D\n", args2);
    void *args3[] = {rbl->matchIt.span, NULL};
    total += StrVec_Fmt(sm, "  matches:_D\n", args3);
    void *args4[] = {rbl->marks, NULL};
    total += StrVec_Fmt(sm, "  marks:_D\n", args4);
    void *args5[] = {rbl->curs, NULL};
    total += StrVec_Fmt(sm, "  curs:_D\n", args5);
    total += StrVec_Fmt(sm, ">", NULL);
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
