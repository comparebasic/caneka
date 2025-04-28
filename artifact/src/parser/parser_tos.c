#include <external.h>
#include <caneka.h>

static char *matchFlagChars = "EXPMNICGKOSLDTU__";

static char *snipChars = "_________CGBU_____";

static i64 _PatChar_print(Stream *sm, Abstract *a, cls type, word flags){
    PatCharDef *pat = (PatCharDef *)a;
    if((flags & (MORE|DEBUG)) == 0){
        return ToStream_NotImpl(sm, a, type, flags);
    }

    char ending[2] = {0, 0};
    i64 total = 0;
    if(pat->flags & PAT_TERM){
        ending[0] = '.';
    }else{
        ending[0] = ',';
    }

    Str *fl = Str_Make(sm->m, FLAG_DEBUG_MAX+1);
    Str_AddFlags(fl, pat->flags, matchFlagChars); 
    if(pat->flags == PAT_END){
        Abstract *args[] = {
            (Abstract *)fl,
            NULL
        };
        total += Fmt(sm, "$", args);
    } else if(pat->to == pat->from){
        Str *from = Str_Ref(m, (byte *)&pat->from, 1, 1, DEBUG);
        Abstract *args[] = {
            fl,
            from,
            ending,
            NULL
        };
        total += Fmt(sm, "$^D.$^d.$", args);
    }else{
        Str *from = Str_Ref(m, (byte *)&pat->from, 1, 1, DEBUG);
        Str *to = Str_Ref(m, (byte *)&pat->to, 1, 1, DEBUG);
        Abstract *args[] = {
            fl,
            from,
            to,
            ending,
            NULL
        };
        total += Fmt(sm, "$^D.$-$^d.$", args);
    }

    return total;
}

static i64 PatChar_Print(Stream *sm, Abstract *a, cls type, word flags){
    PatCharDef *pat = (PatCharDef *)a;
    if((flags & (MORE|DEBUG)) == 0){
        return ToStream_NotImpl(sm, a, type, flags);
    }
    i64 total = 0;
    if(flags & DEBUG){
        total += StrVec_Fmt(sm, "P<", NULL);
    }
    total += _PatChar_print(sm, (Abstract *)pat, TYPE_PATCHARDEF, extended);
    if(flags & DEBUG){
        total += StrVec_Fmt(sm, ">", NULL);
    }
    return total;
}

static i64 PatCharDef_Print(Stream *sm, Abstract *a, cls type, word flags){
    PatCharDef *pat = (PatCharDef *)a;
    if((flags & (MORE|DEBUG)) == 0){
        return ToStream_NotImpl(sm, a, type, flags);
    }
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

static i64 SnipSpan_Print(Stream *sm, Abstract *a, cls type, word flags){
    Span *sns = (Span *)as(a, TYPE_SPAN);
    if((flags & (MORE|DEBUG)) == 0){
        return ToStream_NotImpl(sm, a, type, flags);
    }
    i64 total = 0;
    total += StrVec_Fmt(sm, "Sns<", NULL);
    Iter it;
    Iter_Init(&it, sns);
    while((Iter_Next(&it) & END) == 0){
        Snip *sn = (Snip *)it.value; 
        Str *fl = Str_Make(m, FLAG_DEBUG_MAX+1);
        Str_AddFlags(fl, pat->flags, snipChars); 
        Abstract *args[] = {
            (Abstract *)fl,
            (Abstract *)I32_Wrapped(sm->m, sn->length), 
            NULL
        };
        total += Fmt(sm, "$^D.@^d.", args);
        if(it.type.state & FLAG_ITER_LAST){
            total += Stream_Bytes(sm, (byte *)",", 1);
        }
    }
    total += Stream_Bytes(sm, (byte *)">", 1);
    return total;
}

void Roebling_Print(Stream *sm, Abstract *a, cls type, word flags){
    DebugStack_Push(NULL, 0);
    Roebling *rbl = (Roebling *) as(a, TYPE_ROEBLING);
    if((flags & (MORE|DEBUG)) == 0){
        return ToStream_NotImpl(sm, a, type, flags);
    }
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

i64 Match_Print(Stream *sm, Abstract *a, cls type, word flags){
    Match *mt = (Match *)as(a, TYPE_PATMATCH);
    if((flags & (MORE|DEBUG)) == 0){
        return ToStream_NotImpl(sm, a, type, flags);
    }
    i64 total = 0;
    void *args[] = {State_ToStr(_debugM, mt->type.state), NULL};
    total += StrVec_Fmt(sm, "Mt<_t ", args);

    if(flags & DEBUG){
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

status Parser_ToSInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_PATMATCH, (void *)Match_Print);
    r |= Lookup_Add(m, lk, TYPE_PATCHARDEF, (void *)PatCharDef_Print);
    r |= Lookup_Add(m, lk, TYPE_PATCHAR, (void *)PatChar_Print);
    r |= Lookup_Add(m, lk, TYPE_ROEBLING, (void *)Roebling_Print);
    r |= Lookup_Add(m, lk, TYPE_SNIPSPAN, (void *)SnipSpan_Print);
    return r;
}
