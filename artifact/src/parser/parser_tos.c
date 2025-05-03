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
        Str *from = Str_Ref(sm->m, (byte *)&pat->from, 1, 1, DEBUG);
        Abstract *args[] = {
            (Abstract *)fl,
            (Abstract *)from,
            (Abstract *)Str_CstrRef(sm->m, ending),
            NULL
        };
        total += Fmt(sm, "$^D.$^d.$", args);
    }else{
        Str *from = Str_Ref(sm->m, (byte *)&pat->from, 1, 1, DEBUG);
        Str *to = Str_Ref(sm->m, (byte *)&pat->to, 1, 1, DEBUG);
        Abstract *args[] = {
            (Abstract *)fl,
            (Abstract *)from,
            (Abstract *)to,
            (Abstract *)Str_CstrRef(sm->m, ending),
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
        total += Fmt(sm, "P<", NULL);
    }
    total += _PatChar_print(sm, (Abstract *)pat, TYPE_PATCHARDEF, flags);
    if(flags & DEBUG){
        total += Fmt(sm, ">", NULL);
    }
    return total;
}

static i64 PatCharDef_Print(Stream *sm, Abstract *a, cls type, word flags){
    PatCharDef *pat = (PatCharDef *)a;
    if((flags & (MORE|DEBUG)) == 0){
        return ToStream_NotImpl(sm, a, type, flags);
    }
    i64 total = 0;
    total += Fmt(sm, "Pat<", NULL);
    while(pat->flags != PAT_END){
        total += _PatChar_print(sm, (Abstract *)pat, TYPE_PATCHARDEF, flags);
        pat++;
    }
    total += _PatChar_print(sm, (Abstract *)pat, TYPE_PATCHARDEF, flags);
    total += Fmt(sm, ">", NULL);
    return total;
}

static i64 SnipSpan_Print(Stream *sm, Abstract *a, cls type, word flags){
    Span *sns = (Span *)as(a, TYPE_SPAN);
    if((flags & (MORE|DEBUG)) == 0){
        return ToStream_NotImpl(sm, a, type, flags);
    }
    i64 total = 0;
    total += Fmt(sm, "Sns<", NULL);
    Iter it;
    Iter_Init(&it, sns);
    while((Iter_Next(&it) & END) == 0){
        Snip *sn = (Snip *)it.value; 
        Str *fl = Str_Make(sm->m, FLAG_DEBUG_MAX+1);
        Str_AddFlags(fl, sn->type.state, snipChars); 
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

i64 Roebling_Print(Stream *sm, Abstract *a, cls type, word flags){
    DebugStack_Push(NULL, 0);
    Roebling *rbl = (Roebling *) as(a, TYPE_ROEBLING);
    if((flags & (MORE|DEBUG)) == 0){
        return ToStream_NotImpl(sm, a, type, flags);
    }
    i64 total = 0;
    Abstract *args1[] = {
        (Abstract *)State_ToStr(sm->m, rbl->type.state),
        (Abstract *)I32_Wrapped(sm->m, rbl->parseIt.idx),
        (Abstract *)rbl->parseIt.value,
        NULL
    };
    total += Fmt(sm, "Rbl<$ idx:$/@\n", args1);
    Abstract *args2[] = {
        (Abstract *)rbl->parseIt.span, NULL
    };
    total += Fmt(sm, "  parsers:@\n", args2);
    Abstract *args3[] = {
        (Abstract *)rbl->matchIt.span, NULL
    };
    total += Fmt(sm, "  matches:@\n", args3);
    Abstract *args4[] = {
        (Abstract *)rbl->marks, NULL
    };
    total += Fmt(sm, "  marks:@\n", args4);
    Abstract *args5[] = {
        (Abstract *)rbl->curs, NULL
    };
    total += Fmt(sm, "  curs:@\n", args5);
    total += Fmt(sm, ">", NULL);
    DebugStack_Pop();
    return total;
}

i64 Match_Print(Stream *sm, Abstract *a, cls type, word flags){
    Match *mt = (Match *)as(a, TYPE_PATMATCH);
    if((flags & (MORE|DEBUG)) == 0){
        return ToStream_NotImpl(sm, a, type, flags);
    }
    i64 total = 0;
    Abstract *args[] = {
        (Abstract *)State_ToStr(_debugM, mt->type.state),
        NULL
    };
    total += Fmt(sm, "Mt<_t ", args);

    if(flags & DEBUG){
        PatCharDef *pd = mt->pat.startDef;
        while(pd->flags != PAT_END){
            char *_color = "p.";
            if(pd == mt->pat.curDef){
                _color = "y.";
            }
            Str *color = Str_FromAnsi(sm->m, &_color, _color+1);
            total += Stream_Bytes(sm, color->bytes, color->length); 
            total += ToS(sm, (Abstract *)pd, TYPE_PATCHAR, flags); 
            pd++;
        }
        Fmt(sm, "^d", NULL);
    }

    total += Fmt(sm, "^p.>", NULL);
    return total;
}

status Parser_ToSInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_PATMATCH, (void *)Match_Print);
    r |= Lookup_Add(m, lk, TYPE_PATCHARDEF, (void *)PatCharDef_Print);
    r |= Lookup_Add(m, lk, TYPE_PATCHAR, (void *)PatChar_Print);
    r |= Lookup_Add(m, lk, TYPE_PATCHAR, (void *)PatChar_Print);
    r |= Lookup_Add(m, lk, TYPE_ROEBLING, (void *)Roebling_Print);
    r |= Lookup_Add(m, lk, TYPE_SNIPSPAN, (void *)SnipSpan_Print);
    Abstract *args[] = {
        (Abstract *)lk,
        NULL
    };
    Out("Parser_ToSInit: @\n", args);
    return r;
}
