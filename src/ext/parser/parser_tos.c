#include <external.h>
#include <caneka.h>

static char *matchFlagChars = "EXPMNICGKOSLDTU__";

static char *snipChars = "_________CGBU_____";

static Str **snipLabels = NULL;
static Str **roeblingLabels = NULL;
static Str **matchLabels = NULL;
static Str **tokenizeLabels = NULL;

static status _PatChar_print(Buff *bf, void *a, cls type, word flags){
    PatCharDef *pat = (PatCharDef *)a;
    if((flags & (MORE|DEBUG)) == 0){
        return ToStream_NotImpl(bf, a, type, flags);
    }

    char ending[2] = {0, 0};
    if(pat->flags & PAT_TERM){
        ending[0] = '.';
    }else{
        ending[0] = ',';
    }

    Str *fl = Str_Make(bf->m, FLAG_DEBUG_MAX+1);
    Str_AddFlags(fl, pat->flags, matchFlagChars);
    if(pat->flags == PAT_END){
        Buff_AddBytes(bf, fl->bytes, fl->length);
    } else if(pat->to == pat->from){
        Str *from = Str_Ref(bf->m, (byte *)&pat->from, 1, 1, DEBUG);
        void *args[] = {
            fl,
            from,
            Str_CstrRef(bf->m, ending),
            NULL
        };
        Fmt(bf, "$^D.$^d.$", args);
    }else{
        Str *from = Str_Ref(bf->m, (byte *)&pat->from, 1, 1, DEBUG);
        Str *to = Str_Ref(bf->m, (byte *)&pat->to, 1, 1, DEBUG);
        void *args[] = {
            fl,
            from,
            to,
            Str_CstrRef(bf->m, ending),
            NULL
        };
        Fmt(bf, "$^D.$-$^d.$", args);
    }

    return SUCCESS;
}

static status PatChar_Print(Buff *bf, void *a, cls type, word flags){
    PatCharDef *pat = (PatCharDef *)a;
    if((flags & (MORE|DEBUG)) == 0){
        return ToStream_NotImpl(bf, a, type, flags);
    }
    if(flags & DEBUG){
        Fmt(bf, "P<", NULL);
    }
    _PatChar_print(bf, pat, TYPE_PATCHARDEF, flags);
    if(flags & DEBUG){
        Fmt(bf, ">", NULL);
    }
    return SUCCESS;
}

static status PatCharDef_Print(Buff *bf, void *a, cls type, word flags){
    PatCharDef *pat = (PatCharDef *)a;
    if((flags & (MORE|DEBUG)) == 0){
        return ToStream_NotImpl(bf, a, type, flags);
    }
    Fmt(bf, "Pat<", NULL);
    while(pat->flags != PAT_END){
        _PatChar_print(bf, pat, TYPE_PATCHARDEF, flags);
        pat++;
    }
    _PatChar_print(bf, pat, TYPE_PATCHARDEF, flags);
    Fmt(bf, ">", NULL);
    return SUCCESS;
}

static status Snip_Print(Buff *bf, void *a, cls type, word flags){
    Snip *sn = (Snip *)as(a, TYPE_SNIP);
    if((flags & (MORE|DEBUG)) == 0){
        return ToStream_NotImpl(bf, a, type, flags);
    }
    if(flags & DEBUG){
        Fmt(bf, "Sn<", NULL);
    }
    void *args[] = {
        Type_StateVec(bf->m, sn->type.of, sn->type.state),
        I32_Wrapped(bf->m, sn->length), 
        NULL
    };
    Fmt(bf, "$/$", args);
    if(flags & DEBUG){
        Buff_AddBytes(bf, (byte *)">", 1);
    }
    return SUCCESS;
}

static status SnipSpan_Print(Buff *bf, void *a, cls type, word flags){
    Span *sns = (Span *)as(a, TYPE_SPAN);
    if((flags & (MORE|DEBUG)) == 0){
        return ToStream_NotImpl(bf, a, type, flags);
    }
    Fmt(bf, "Sns<", NULL);
    Iter it;
    Iter_Init(&it, sns);
    while((Iter_Next(&it) & END) == 0){
        ToS(bf, it.value, 0, flags|MORE);
        if((it.type.state & LAST) == 0){
            Buff_AddBytes(bf, (byte *)",", 1);
        }
    }
    return Buff_AddBytes(bf, (byte *)">", 1);
}

status Roebling_Print(Buff *bf, void *a, cls type, word flags){
    Roebling *rbl = (Roebling *) as(a, TYPE_ROEBLING);
    void *args[4];
    if((flags & (MORE|DEBUG)) == 0){
        return ToStream_NotImpl(bf, a, type, flags);
    }
    args[0] = Type_StateVec(bf->m, rbl->type.of, rbl->type.state);
    args[1] = I32_Wrapped(bf->m, rbl->parseIt.idx);
    args[2] = rbl->parseIt.value;
    args[3] = NULL;
    Fmt(bf, "Rbl<$ idx:$/@ ", args);
    args[0] = rbl->matchIt.value;
    args[1] = NULL;
    Fmt(bf, " @", args);
    if(flags & DEBUG){
        Buff_AddBytes(bf, (byte *)" \n", 2);
        args[0] = rbl->parseIt.p;
        args[1] = NULL;
        Fmt(bf, "  parsers: @\n", args);

        args[0] = rbl->marks;
        args[1] = NULL;
        Fmt(bf, "  marks: @\n", args);

        args[0] = rbl->matchIt.p;
        args[1] = NULL;
        Fmt(bf, "  matches: @\n", args);

        word prev = bf->type.state;
        args[0] =  rbl->curs;
        args[1] = NULL;
        Fmt(bf, "  curs:@\n", args);
        bf->type.state = prev;
    }
    return Fmt(bf, ">", NULL);
}

status Match_Print(Buff *bf, void *a, cls type, word flags){
    Match *mt = (Match *)as(a, TYPE_PATMATCH);
    if((flags & (MORE|DEBUG)) == 0){
        return ToStream_NotImpl(bf, a, type, flags);
    }
    void *args[] = {
        Type_StateVec(bf->m, mt->type.of, mt->type.state),
        I32_Wrapped(bf->m, mt->jump),
        NULL
    };
    Fmt(bf, "Mt<$ ", args);
    if(mt->type.state & MATCH_JUMP){
        void *args[] = {
            I16_Wrapped(bf->m, mt->jump),
            NULL
        };
        Fmt(bf, " $jump ", args);
    }

    if(flags & (DEBUG|MORE)){
        PatCharDef *pd = mt->pat.startDef;
        while(pd <= mt->pat.endDef){
            /*
            char *_color = "E.";
            if(pd == mt->pat.curDef){
                Str *color = Str_FromAnsi(bf->m, &_color, _color+1);
                Buff_AddBytes(bf, color->bytes, color->length); 
            }
            */
            ToS(bf, pd, TYPE_PATCHAR, MORE); 
            /*
            _color = "e.";
            if(pd == mt->pat.curDef){
                Str *color = Str_FromAnsi(bf->m, &_color, _color+1);
                Buff_AddBytes(bf, color->bytes, color->length); 
            }
            */
            pd++;
        }
        if(flags & MORE){
            Buff_AddBytes(bf, (byte *)" ", 1); 
            ToS(bf, mt->backlog, TYPE_SNIPSPAN, MORE);
            Buff_AddBytes(bf, (byte *)" ", 1); 
            ToS(bf, &mt->snip, TYPE_SNIP, MORE);
        }
        Fmt(bf, "^d", NULL);
    }

    return Fmt(bf, ">", NULL);
}

status Parser_InitLabels(MemCh *m, Lookup *lk){
    status r = READY;
    if(snipLabels == NULL){
        snipLabels = (Str **)Arr_Make(m, 17);
        snipLabels[9] = Str_CstrRef(m, "SNIP_CONTENT");
        snipLabels[10] = Str_CstrRef(m, "SNIP_GAP");
        snipLabels[11] = Str_CstrRef(m, "SNIP_STR_BOUNDRY");
        snipLabels[12] = Str_CstrRef(m, "SNIP_UNCLAIMED");
        snipLabels[13] = Str_CstrRef(m, "SNIP_SKIPPED");
        Lookup_Add(m, lk, TYPE_SNIP, (void *)snipLabels);
        r |= SUCCESS;
    }

    if(roeblingLabels == NULL){
        roeblingLabels = (Str **)Arr_Make(m, 17);
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
        matchLabels[9] = Str_CstrRef(m, "MATCH_LAST_TERM");
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
        tokenizeLabels[9] = Str_CstrRef(m, "TOKEN_SEPERATE");
        tokenizeLabels[10] = Str_CstrRef(m, "TOKEN_OUTDENT");
        tokenizeLabels[11] = Str_CstrRef(m, "TOKEN_INLINE");
        tokenizeLabels[12] = Str_CstrRef(m, "TOKEN_BY_TYPE");
        tokenizeLabels[13] = Str_CstrRef(m, "TOKEN_ATTR_KEY");
        tokenizeLabels[14] = Str_CstrRef(m, "TOKEN_ATTR_VALUE");
        tokenizeLabels[15] = Str_CstrRef(m, "TOKEN_NO_COMBINE");
        tokenizeLabels[16] = Str_CstrRef(m, "TOKEN_NO_CONTENT");
        Lookup_Add(m, lk, TYPE_TOKENIZE, (void *)tokenizeLabels);
        r |= SUCCESS;
    }

    if(r == READY){
        r |= NOOP;
    }

    return r;
}

static status Tokenize_Print(Buff *bf, void *a, cls type, word flags){
    Tokenize *tk = (Tokenize *)as(a, TYPE_TOKENIZE); 
    if((flags & (DEBUG|MORE)) == 0){
        return ToStream_NotImpl(bf, a, type, flags);
    }else{
        void *args[] = {
            Type_StateVec(bf->m, tk->type.of, tk->type.state),
            Type_ToStr(bf->m, tk->captureKey),
            Type_ToStr(bf->m, tk->typeOf),
            NULL
        };
        return Fmt(bf, "Tk<$ ^D.$^d./$>", args);
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
