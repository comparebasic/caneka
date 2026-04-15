#include <external.h>
#include <caneka.h>

static PatCharDef textToTemplDef[] = {
    {PAT_KO|PAT_KO_TERM|PAT_INVERT_CAPTURE,'\n' ,'\n'},
    {PAT_KO|PAT_KO_TERM|PAT_INVERT_CAPTURE,';',';'},
    patText,
    {PAT_END, 0, 0}
};

static PatCharDef textToVarDef[] = {
    {PAT_KO|PAT_KO_TERM|PAT_INVERT_CAPTURE,'{','{'},
    patText,
    {PAT_END, 0, 0}
};

static PatCharDef toTemplDef[] = {
    {PAT_TERM, '\n' ,'\n'},
    {PAT_TERM, ';' ,';'},
    {PAT_END, 0, 0}
};

static PatCharDef indentDef[] = {
    {PAT_TERM, '^' ,'^'},
    {PAT_END, 0, 0}
};

static PatCharDef templTokenDef[] = {
    {PAT_KO|PAT_INVERT_CAPTURE, ' ', ' '}, {PAT_KO|PAT_INVERT_CAPTURE, '\n', '\n'}, 
    {PAT_KO|PAT_INVERT_CAPTURE, '\t', '\t'}, {PAT_KO|PAT_INVERT_CAPTURE, '!', '!'},
    {PAT_KO|PAT_INVERT_CAPTURE, '#', '#'}, {PAT_KO|PAT_INVERT_CAPTURE, '?', '?'},
    {PAT_KO|PAT_INVERT_CAPTURE, '|', '|'}, {PAT_KO|PAT_INVERT_CAPTURE, ':', ':'},
    {PAT_KO|PAT_INVERT_CAPTURE, '.', '.'}, {PAT_KO|PAT_INVERT_CAPTURE, '@', '@'},
        {PAT_KO|PAT_KO_TERM|PAT_INVERT_CAPTURE, '*', '*'},
    patText,
    {PAT_END, 0, 0}
};

static PatCharDef templContinueDef[] = {
    {PAT_TERM, '\n', '\n'},
    {PAT_TERM, ';', ';'},
    {PAT_END, 0, 0}
};

static PatCharDef templEndDef[] = {
    {PAT_TERM, '\n', '\n'},
    {PAT_TERM|PAT_INVERT|PAT_INVERT_CAPTURE, ';', ';'},
    {PAT_END, 0, 0}
};

static PatCharDef wsDef[] = {
    {PAT_MANY, ' ' ,' '},{PAT_MANY|PAT_TERM, '\t' ,'\t'},
    {PAT_END, 0, 0}
};

static PatCharDef keyDef[] = {
    {PAT_TERM, '@' ,'@'},
    {PAT_END, 0, 0}
};

static PatCharDef attSepDef[] = {
    {PAT_TERM, '*' ,'*'},
    {PAT_END, 0, 0}
};

static PatCharDef propSepDef[] = {
    {PAT_TERM, '#' ,'#'},
    {PAT_END, 0, 0}
};

static PatCharDef pathSepDef[] = {
    {PAT_KO|PAT_INVERT_CAPTURE, '}', '}'},
    {PAT_TERM, '.' ,'.'},
    {PAT_DROPOUT, ' ' ,' '},
    {PAT_DROPOUT, '\t' ,'\t'},
    {PAT_DROPOUT, '\n' ,'\n'},
    {PAT_TERM|PAT_DROPOUT, '.' ,'.'},
    {PAT_END, 0, 0}
};

static PatCharDef varIfDef[] = {
    {PAT_TERM, '?' ,'?'},
    {PAT_END, 0, 0}
};

static PatCharDef varElseDef[] = {
    {PAT_TERM, '|' ,'|'},
    {PAT_END, 0, 0}
};

static PatCharDef toVarDef[] = {
    {PAT_TERM, '{' ,'{'},
    {PAT_END, 0, 0}
};

static PatCharDef iterDef[] = {
    {PAT_TERM, '.' ,'.'},
    {PAT_TERM, '.' ,'.'},
    {PAT_TERM, '.' ,'.'},
    {PAT_ANY|PAT_TERM,' ',' '},
    {PAT_END, 0, 0}
};

static PatCharDef greaterDef[] = {
    {PAT_TERM, '>' ,'>'},
    {PAT_END, 0, 0}
};

static PatCharDef equalDef[] = {
    {PAT_TERM, '=' ,'='},
    {PAT_END, 0, 0}
};

static PatCharDef activeDef[] = {
    {PAT_TERM, '_' ,'_'},
    {PAT_END, 0, 0}
};

static PatCharDef withDef[] = {
    {PAT_TERM, ':' ,':'},
    {PAT_END, 0, 0}
};

static PatCharDef varDef[] = {
    {PAT_TERM, '{' ,'{'},
    {PAT_END, 0, 0}
};

static PatCharDef varNumBodyDef[] = {
    {PAT_KO, '}', '}'},
    {PAT_KO|PAT_INVERT_CAPTURE, '#', '#'}, {PAT_KO|PAT_INVERT_CAPTURE, '.', '.'},
        {PAT_KO|PAT_KO_TERM|PAT_INVERT_CAPTURE, '*', '*'},
    {PAT_MANY|PAT_TERM, '0', '9'},
    {PAT_END, 0, 0}
};

static PatCharDef varTokenDef[] = {
    {PAT_KO|PAT_INVERT_CAPTURE, '#', '#'}, {PAT_KO|PAT_INVERT_CAPTURE, '.', '.'},
    {PAT_KO|PAT_INVERT_CAPTURE, '*', '*'},
        {PAT_KO|PAT_INVERT_CAPTURE|PAT_KO_TERM, '}', '}'},
    patText,
    {PAT_END, 0, 0}
};

static PatCharDef varItemDef[] = {
    {PAT_KO, '_', '_'},
    {PAT_END, 0, 0}
};

static PatCharDef varEndDef[] = {
    {PAT_TERM, '}' ,'}'},
    {PAT_END, 0, 0}
};

static status text(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        toVarDef, FORMAT_TEMPL_VAR, FORMAT_TEMPL_VAR);
    r |= Roebling_SetPattern(rbl,
        toTemplDef, FORMAT_TEMPL_TEMPL, FORMAT_TEMPL_TEMPL);
    r |= Roebling_SetPattern(rbl,
        textToTemplDef, FORMAT_TEMPL_TEXT, FORMAT_TEMPL_TEXT);
    r |= Roebling_SetPattern(rbl,
        textToVarDef, FORMAT_TEMPL_TEXT, FORMAT_TEMPL_TEXT);

    return r;
}

static status templ(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        wsDef, FORMAT_TEMPL_WHITESPACE, FORMAT_TEMPL_TEMPL);
    r |= Roebling_SetPattern(rbl,
        templTokenDef, FORMAT_TEMPL_TOKEN, FORMAT_TEMPL_TEMPL);
    r |= Roebling_SetPattern(rbl,
        templContinueDef, FORMAT_TEMPL_CONTINUED, FORMAT_TEMPL_TEMPL);
    r |= Roebling_SetPattern(rbl,
        templEndDef, FORMAT_TEMPL_TEMPL_END, FORMAT_TEMPL_TEXT);
    r |= Roebling_SetPattern(rbl,
        propSepDef, FORMAT_TEMPL_PROP_SEP, FORMAT_TEMPL_TEMPL);
    r |= Roebling_SetPattern(rbl,
        attSepDef, FORMAT_TEMPL_ATT_SEP, FORMAT_TEMPL_TEMPL);
    r |= Roebling_SetPattern(rbl,
        keyDef, FORMAT_TEMPL_KEY_SEP, FORMAT_TEMPL_TEMPL);
    r |= Roebling_SetPattern(rbl,
        pathSepDef, FORMAT_TEMPL_PATH_SEP, FORMAT_TEMPL_TEMPL);
    r |= Roebling_SetPattern(rbl,
        activeDef, FORMAT_TEMPL_ACTIVE, FORMAT_TEMPL_TEMPL);
    r |= Roebling_SetPattern(rbl,
        equalDef, FORMAT_TEMPL_CURRENT, FORMAT_TEMPL_TEMPL);
    r |= Roebling_SetPattern(rbl,
        greaterDef, FORMAT_TEMPL_LEVEL, FORMAT_TEMPL_TEMPL);
    r |= Roebling_SetPattern(rbl,
        iterDef, FORMAT_TEMPL_FOR, FORMAT_TEMPL_TEMPL);
    r |= Roebling_SetPattern(rbl,
        varIfDef, FORMAT_TEMPL_IF, FORMAT_TEMPL_TEMPL);
    r |= Roebling_SetPattern(rbl,
        varIfDef, FORMAT_TEMPL_ELSE, FORMAT_TEMPL_TEMPL);
    r |= Roebling_SetPattern(rbl,
        withDef, FORMAT_TEMPL_WITH, FORMAT_TEMPL_TEMPL);

    return r;
}

static status var(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        wsDef, FORMAT_TEMPL_WHITESPACE, FORMAT_TEMPL_VAR);
    r |= Roebling_SetPattern(rbl,
        propSepDef, FORMAT_TEMPL_PROP_SEP, FORMAT_TEMPL_VAR);
    r |= Roebling_SetPattern(rbl,
        indentDef, FORMAT_TEMPL_INDENT, FORMAT_TEMPL_VAR);
    r |= Roebling_SetPattern(rbl,
        attSepDef, FORMAT_TEMPL_ATT_SEP, FORMAT_TEMPL_VAR);
    r |= Roebling_SetPattern(rbl,
        keyDef, FORMAT_TEMPL_KEY_SEP, FORMAT_TEMPL_VAR);
    r |= Roebling_SetPattern(rbl,
        pathSepDef, FORMAT_TEMPL_PATH_SEP, FORMAT_TEMPL_VAR);
    r |= Roebling_SetPattern(rbl,
        varTokenDef, FORMAT_TEMPL_TOKEN, FORMAT_TEMPL_VAR);
    r |= Roebling_SetPattern(rbl,
        varEndDef, FORMAT_TEMPL_VAR_END, FORMAT_TEMPL_TEXT);

    return r;
}

static status Capture(Roebling *rbl, word captureKey, StrVec *v){
    MemCh *m = rbl->m;
    Mess *mess = (Mess *)Ifc(rbl->m, rbl->dest, TYPE_MESS);
    TemplCtx *ctx = (TemplCtx*)Ifc(rbl->m, rbl->source, TYPE_TEMPL_CTX);
    if(mess->type.state & DEBUG){
        void *args[] = {
            Type_ToStr(OutStream->m, captureKey),
            v,
            NULL
        };
        Out("^c. Templ Capture ^0.$/^y.@^0.\n", args);
    }

    Fetcher *fch = NULL;
    if(captureKey == FORMAT_TEMPL_TEXT){
        Iter_Add(&ctx->it, v);
    }else if(captureKey == FORMAT_TEMPL_VAR){
        fch = Fetcher_Make(m);
        fch->type.state |= FETCHER_VAR;
        Iter_Add(&ctx->it, fch);
    }else if(captureKey == FORMAT_TEMPL_VAR_END){
        Abstract *a = Iter_Current(&ctx->it);
        if(((Fetcher *)a)->val.targets->nvalues == 0){
            a->type.state = (a->type.state & NORMAL_FLAGS) | FETCHER_END;
        }
    }else if(captureKey == FORMAT_TEMPL_TEMPL){
        Abstract *a = Iter_Current(&ctx->it);
        if(a->type.of == TYPE_STRVEC){
            StrVec_AddBytes(m, (StrVec *)a, (byte *)"\n", 1);
        }
        fch = Fetcher_Make(m);
        fch->type.state |= FETCHER_TEMPL;
        Iter_Add(&ctx->it, fch);
    }else if(captureKey == FORMAT_TEMPL_TEMPL_END || 
            captureKey == FORMAT_TEMPL_CONTINUED){
        Abstract *a = Iter_Current(&ctx->it);
        if(((Fetcher *)a)->val.targets->nvalues == 0){
            fch = (Fetcher *)a;
            fch->type.state = (a->type.state & NORMAL_FLAGS) | FETCHER_END;
        }else if(captureKey == FORMAT_TEMPL_CONTINUED){
            fch = Fetcher_Make(m);
            fch->type.state |= FETCHER_TEMPL;
            Iter_Add(&ctx->it, fch);
        }
    }else if(captureKey > _FORMAT_TEMPL_VAR_ANNOTE_START &&
            captureKey < _FORMAT_TEMPL_VAR_ANNOTE_END){
        fch = (Fetcher *)Iter_Current(&ctx->it);

        if(captureKey == FORMAT_TEMPL_PROP_SEP){
            ctx->next.state = FETCH_TARGET_PROP;
        }else if(captureKey == FORMAT_TEMPL_ATT_SEP){
            ctx->next.state = FETCH_TARGET_ATT;
        }else if(captureKey == FORMAT_TEMPL_KEY_SEP){
            ctx->next.state = FETCH_TARGET_KEY;
        }else if(captureKey == FORMAT_TEMPL_PATH_SEP){
            ctx->next.state = FETCH_TARGET_KEY;
        }else{
            Str *s = StrVec_Str(m, v);
            FetchTarget *tg = NULL;
            if(ctx->next.state & FETCH_TARGET_ATT){
                tg = FetchTarget_MakeAtt(m, s);
            }else if(ctx->next.state & FETCH_TARGET_PROP){
                tg = FetchTarget_MakeProp(m, s);
            }else if(ctx->next.state & FETCH_TARGET_IDX){
                tg = FetchTarget_MakeIdx(m, Int_FromStr(s));
            }else{
                tg = FetchTarget_MakeKey(m, s);
            }
            ctx->type.state = ZERO;
            fch->type.state |= FETCHER_VAR;
            Span_Add(fch->val.targets, tg);
        }
    }else if(captureKey > _FORMAT_TEMPL_LOGIC_START && 
            captureKey < _FORMAT_TEMPL_LOGIC_END){
        fch = (Fetcher *)Iter_Current(&ctx->it);
        if(captureKey == FORMAT_TEMPL_FOR){
            fch->type.state = (fch->type.state & NORMAL_FLAGS) | FETCHER_FOR;
            FetchTarget *tg = FetchTarget_MakeIter(m);
            Span_Add(fch->val.targets, tg);
        }else if(captureKey == FORMAT_TEMPL_ACTIVE ||
                captureKey == FORMAT_TEMPL_LEVEL ||
                captureKey == FORMAT_TEMPL_CURRENT ||
                captureKey == FORMAT_TEMPL_INDENT
            ){
            fch->type.state = (fch->type.state & NORMAL_FLAGS) | FETCHER_CONDITION;
            if(captureKey == FORMAT_TEMPL_INDENT){
                fch->type.state |= FETCHER_TEMPL;
            }
            FetchTarget *tg = FetchTarget_MakeCommand(m);
            tg->objType.of = captureKey;
            Span_Add(fch->val.targets, tg);
        }else if(captureKey == FORMAT_TEMPL_IF){
            fch->type.state = (fch->type.state & NORMAL_FLAGS) | FETCHER_IF;
        }else if(captureKey == FORMAT_TEMPL_IFNOT){
            fch->type.state = (fch->type.state & NORMAL_FLAGS) | FETCHER_IFNOT;
        }else if(captureKey == FORMAT_TEMPL_WITH){
            fch->type.state = (fch->type.state & NORMAL_FLAGS) | FETCHER_WITH;
        }
    }

    return SUCCESS;
}

Roebling *Templ_RoeblingMake(MemCh *m, Cursor *curs, void *source){
    Roebling *rbl = Roebling_Make(m, curs, Capture, NULL); 
    Roebling_AddStep(rbl, I16_Wrapped(m, FORMAT_TEMPL_TEXT));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)text));
    Roebling_AddStep(rbl, I16_Wrapped(m, FORMAT_TEMPL_TEMPL));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)templ));
    Roebling_AddStep(rbl, I16_Wrapped(m, FORMAT_TEMPL_VAR));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)var));
    Roebling_Start(rbl);

    rbl->capture = Capture;
    Mess *mess = Mess_Make(m);
    mess->tokenizer = Lookup_Make(m, _TEMPL_START);
    rbl->dest = (Abstract *)mess;
    rbl->source = source;
    return rbl;
}
