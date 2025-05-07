#include <external.h>
#include <caneka.h>

static char *matchFlagChars = "EXPMNICGKOSLDTU__";

static char *snipChars = "_________CGBU_____";

static Str **snipLabels = NULL;
static Str **roeblingLabels = NULL;
static Str **matchLabels = NULL;
static Str **tokenizeLabels = NULL;

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
        total += Stream_Bytes(sm, fl->bytes, fl->length);
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

static i64 Snip_Print(Stream *sm, Abstract *a, cls type, word flags){
    Snip *sn = (Snip *)as(a, TYPE_SNIP);
    if((flags & (MORE|DEBUG)) == 0){
        return ToStream_NotImpl(sm, a, type, flags);
    }
    i64 total = 0;
    if(flags & DEBUG){
        total += Fmt(sm, "Sn<", NULL);
    }
    Abstract *args[] = {
        (Abstract *)StreamTask_Make(sm->m, NULL, (Abstract *)sn, ToS_FlagLabels),
        (Abstract *)I32_Wrapped(sm->m, sn->length), 
        NULL
    };
    total += Fmt(sm, "$/$", args);
    if(flags & DEBUG){
        total += Stream_Bytes(sm, (byte *)">", 1);
    }
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
        total += ToS(sm, it.value, 0, flags|MORE);
        if((it.type.state & FLAG_ITER_LAST) == 0){
            total += Stream_Bytes(sm, (byte *)",", 1);
        }
    }
    total += Stream_Bytes(sm, (byte *)">", 1);
    return total;
}

i64 Roebling_Print(Stream *sm, Abstract *a, cls type, word flags){
    Roebling *rbl = (Roebling *) as(a, TYPE_ROEBLING);
    if((flags & (MORE|DEBUG)) == 0){
        return ToStream_NotImpl(sm, a, type, flags);
    }
    i64 total = 0;
    Abstract *args1[] = {
        (Abstract *)StreamTask_Make(sm->m, NULL, (Abstract *)rbl, ToS_FlagLabels),
        (Abstract *)I32_Wrapped(sm->m, rbl->parseIt.idx),
        (Abstract *)rbl->parseIt.value,
        NULL
    };
    total += Fmt(sm, "Rbl<$ idx:$/@ ", args1);
    Abstract *args2[] = {
        (Abstract *)rbl->matchIt.value, 
        NULL
    };
    total += Fmt(sm, " @ \n", args2);
    if(flags & DEBUG){
        Abstract *args3[] = {
            (Abstract *)rbl->parseIt.span,
            NULL
        };
        total += Fmt(sm, "  parsers: @\n", args3);
        Abstract *args5[] = {
            (Abstract *)rbl->marks,
            NULL
        };
        total += Fmt(sm, "  marks: @\n", args5);
        Abstract *args4[] = {
            (Abstract *)rbl->matchIt.span,
            NULL
        };
        word prev = sm->type.state;
        sm->type.state |= DEBUG;
        total += Fmt(sm, "  matches: @\n", args4);
        sm->type.state = prev;
    }
    Abstract *args6[] = {
        (Abstract *)rbl->curs, 
        NULL
    };
    total += Fmt(sm, "  curs:@\n", args6);
    total += Fmt(sm, ">", NULL);
    return total;
}

i64 Match_Print(Stream *sm, Abstract *a, cls type, word flags){
    Match *mt = (Match *)as(a, TYPE_PATMATCH);
    if((flags & (MORE|DEBUG)) == 0){
        return ToStream_NotImpl(sm, a, type, flags);
    }
    i64 total = 0;
    Abstract *args[] = {
        (Abstract *)StreamTask_Make(sm->m, NULL, (Abstract *)mt, ToS_FlagLabels),
        (Abstract *)I32_Wrapped(sm->m, mt->jump),
        NULL
    };
    total += Fmt(sm, "Mt<$ ", args);
    if(mt->type.state & MATCH_JUMP){
        Abstract *args[] = {
            (Abstract *)I16_Wrapped(sm->m, mt->jump),
            NULL
        };
        total += Fmt(sm, " $jump ", args);
    }

    if(flags & (DEBUG|MORE)){
        PatCharDef *pd = mt->pat.startDef;
        while(pd->flags != PAT_END){
            char *_color = "d.";
            if(pd == mt->pat.curDef){
                _color = "D.";
            }
            Str *color = Str_FromAnsi(sm->m, &_color, _color+1);
            total += Stream_Bytes(sm, color->bytes, color->length); 
            total += ToS(sm, (Abstract *)pd, TYPE_PATCHAR, flags|MORE); 
            pd++;
        }
        if(flags & MORE){
            total += Stream_Bytes(sm, (byte *)" ", 1); 
            total += ToS(sm, (Abstract *)mt->backlog, TYPE_SNIPSPAN, MORE);
        }
        total += Fmt(sm, "^d", NULL);
    }

    total += Fmt(sm, ">", NULL);
    return total;
}

status Parser_InitLabels(MemCh *m, Lookup *lk){
    status r = READY;
    if(snipLabels == NULL){
        snipLabels = (Str **)Arr_Make(m, 17);
        snipLabels[0] = Str_CstrRef(m, "ZERO/READY");
        snipLabels[1] = Str_CstrRef(m, "SUCCESS");
        snipLabels[2] = Str_CstrRef(m, "ERROR");
        snipLabels[3] = Str_CstrRef(m, "NOOP");
        snipLabels[4] = Str_CstrRef(m, "DEBUG");
        snipLabels[5] = Str_CstrRef(m, "MORE");
        snipLabels[6] = Str_CstrRef(m, "CONTINUE");
        snipLabels[7] = Str_CstrRef(m, "END");
        snipLabels[8] = Str_CstrRef(m, "PROCESSING");
        snipLabels[9] = Str_CstrRef(m, "SNIP_CONTENT");
        snipLabels[10] = Str_CstrRef(m, "SNIP_GAP");
        snipLabels[11] = Str_CstrRef(m, "SNIP_STR_BOUNDRY");
        snipLabels[12] = Str_CstrRef(m, "SNIP_UNCLAIMED");
        snipLabels[13] = Str_CstrRef(m, "CLS_FLAG_ECHO");
        snipLabels[14] = Str_CstrRef(m, "CLS_FLAG_FOXTROT");
        snipLabels[15] = Str_CstrRef(m, "CLS_FLAG_GOLF");
        snipLabels[16] = Str_CstrRef(m, "CLS_FLAG_HOTEL");
        Lookup_Add(m, lk, TYPE_SNIP, (void *)snipLabels);
        r |= SUCCESS;
    }

    if(roeblingLabels == NULL){
        roeblingLabels = (Str **)Arr_Make(m, 17);
        roeblingLabels[0] = Str_CstrRef(m, "ZERO/READY");
        roeblingLabels[1] = Str_CstrRef(m, "SUCCESS");
        roeblingLabels[2] = Str_CstrRef(m, "ERROR");
        roeblingLabels[3] = Str_CstrRef(m, "NOOP");
        roeblingLabels[4] = Str_CstrRef(m, "DEBUG");
        roeblingLabels[5] = Str_CstrRef(m, "MORE");
        roeblingLabels[6] = Str_CstrRef(m, "CONTINUE");
        roeblingLabels[7] = Str_CstrRef(m, "END");
        roeblingLabels[8] = Str_CstrRef(m, "PROCESSING");
        roeblingLabels[9] = NULL;
        roeblingLabels[10] = NULL;
        roeblingLabels[11] = Str_CstrRef(m, "ROEBLING_NEXT");
        roeblingLabels[12] = Str_CstrRef(m, "ROEBLING_REPEAT");
        roeblingLabels[13] = NULL;
        roeblingLabels[14] = NULL;
        roeblingLabels[15] = NULL;
        roeblingLabels[16] = NULL;
        Lookup_Add(m, lk, TYPE_ROEBLING, (void *)roeblingLabels);
        r |= SUCCESS;
    }

    if(matchLabels == NULL){
        matchLabels = (Str **)Arr_Make(m, 17);
        matchLabels[9] = Str_CstrRef(m, "MATCH_GOTO");
        matchLabels[10] = Str_CstrRef(m, "MATCH_JUMP");
        matchLabels[11] = Str_CstrRef(m, "MATCH_TERM_FOUND");
        matchLabels[12] = Str_CstrRef(m, "MATCH_KO_INVERT");
        matchLabels[13] = Str_CstrRef(m, "MATCH_KO");
        matchLabels[14] = Str_CstrRef(m, "MATCH_SEARCH");
        matchLabels[15] = Str_CstrRef(m, "MATCH_LEAVE");
        matchLabels[16] = Str_CstrRef(m, "MATCH_ACCEPT_EMPTY");
        Lookup_Add(m, lk, TYPE_PATMATCH, (void *)matchLabels);
        r |= SUCCESS;
    }

    if(tokenizeLabels == NULL){
        tokenizeLabels = (Str **)Arr_Make(m, 17);
        tokenizeLabels[9] = Str_CstrRef(m, "TOKEN_CLOSE");
        tokenizeLabels[10] = Str_CstrRef(m, "TOKEN_CLOSE_OUTDENT");
        tokenizeLabels[11] = Str_CstrRef(m, "TOKEN_LAST_ALUE");
        tokenizeLabels[12] = Str_CstrRef(m, "TOKEN_LAST_SEP");
        tokenizeLabels[13] = Str_CstrRef(m, "TOKEN_ATTR_KEY");
        tokenizeLabels[14] = Str_CstrRef(m, "TOKEN_ATTR_VALUE");
        Lookup_Add(m, lk, TYPE_TOKENIZE, (void *)tokenizeLabels);
        r |= SUCCESS;
    }

    if(r == READY){
        r |= NOOP;
    }

    return r;
}

static i64 Tokenize_Print(Stream *sm, Abstract *a, cls type, word flags){
    Tokenize *tk = (Tokenize *)as(a, TYPE_TOKENIZE); 
    if((flags & (DEBUG|MORE)) == 0){
        return ToStream_NotImpl(sm, a, type, flags);
    }else{
        Abstract *args[] = {
            (Abstract *)StreamTask_Make(sm->m, NULL, (Abstract *)tk, ToS_FlagLabels),
            (Abstract *)Type_ToStr(sm->m, tk->captureKey),
            (Abstract *)Type_ToStr(sm->m, tk->typeOf),
            NULL
        };
        return Fmt(sm, "Tk<$ ^D.$^d./$>", args);
    }
}

status Parser_ToSInit(MemCh *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_PATMATCH, (void *)Match_Print);
    r |= Lookup_Add(m, lk, TYPE_PATCHARDEF, (void *)PatCharDef_Print);
    r |= Lookup_Add(m, lk, TYPE_PATCHAR, (void *)PatChar_Print);
    r |= Lookup_Add(m, lk, TYPE_PATCHAR, (void *)PatChar_Print);
    r |= Lookup_Add(m, lk, TYPE_ROEBLING, (void *)Roebling_Print);
    r |= Lookup_Add(m, lk, TYPE_SNIPSPAN, (void *)SnipSpan_Print);
    r |= Lookup_Add(m, lk, TYPE_TOKENIZE, (void *)Tokenize_Print);
    r |= Lookup_Add(m, lk, TYPE_SNIP, (void *)Snip_Print);
    return r;
}
