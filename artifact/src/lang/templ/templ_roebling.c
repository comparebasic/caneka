#include <external.h>
#include <caneka.h>

static PatCharDef textToTemplDef[] = {
    {PAT_KO|PAT_KO_TERM|PAT_CONSUME,'\n' ,'\n'},
    {PAT_KO|PAT_KO_TERM|PAT_CONSUME,';',';'},
    patText,
    {PAT_END, 0, 0}
};

static PatCharDef textToVarDef[] = {
    {PAT_KO|PAT_KO_TERM|PAT_INVERT_CAPTURE,'{','{'},
    patText,
    {PAT_END, 0, 0}
};

static PatCharDef templTokenDef[] = {
    {PAT_KO|PAT_INVERT_CAPTURE, ' ', ' '}, {PAT_KO|PAT_INVERT_CAPTURE, '\n', '\n'}, 
    {PAT_KO|PAT_INVERT_CAPTURE, '\t', '\t'}, {PAT_KO|PAT_INVERT_CAPTURE, '!', '!'},
    {PAT_KO|PAT_INVERT_CAPTURE, '#', '#'}, {PAT_KO|PAT_INVERT_CAPTURE, '?', '?'},
    {PAT_KO|PAT_INVERT_CAPTURE, ':', ':'}, {PAT_KO|PAT_INVERT_CAPTURE, '.', '.'},
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

static PatCharDef attSepDef[] = {
    {PAT_TERM, '*' ,'*'},
    {PAT_END, 0, 0}
};

static PatCharDef propSepDef[] = {
    {PAT_TERM, '#' ,'#'},
    {PAT_END, 0, 0}
};

static PatCharDef pathSepDef[] = {
    {PAT_TERM, '.' ,'.'},
    {PAT_INVERT|PAT_INVERT_CAPTURE|PAT_TERM, '.' ,'.'},
    {PAT_END, 0, 0}
};

static PatCharDef varIfDef[] = {
    {PAT_TERM, '?' ,'?'},
    {PAT_END, 0, 0}
};

static PatCharDef iterDef[] = {
    {PAT_TERM, '.' ,'.'},
    {PAT_TERM, '.' ,'.'},
    {PAT_TERM, '.' ,'.'},
    {PAT_END, 0, 0}
};

static PatCharDef withDef[] = {
    {PAT_TERM, ':' ,':'},
    {PAT_END, 0, 0}
};

static PatCharDef levelDef[] = {
    {PAT_TERM, '^' ,'^'},
    {PAT_MANY|PAT_TERM, '0' ,'9'},
    {PAT_END, 0, 0}
};

static PatCharDef levelSpreadDef[] = {
    {PAT_TERM, '^' ,'^'},
    {PAT_MANY|PAT_TERM, '0' ,'9'},
    {PAT_TERM|PAT_INVERT_CAPTURE|PAT_CONSUME, '.' ,'.'},
    {PAT_TERM|PAT_INVERT_CAPTURE|PAT_CONSUME, '.' ,'.'},
    {PAT_INVERT|PAT_INVERT_CAPTURE|PAT_TERM, '.' ,'.'},
    {PAT_END, 0, 0}
};

static PatCharDef betweenLevelsDef[] = {
    {PAT_TERM, '.' ,'.'},
    {PAT_TERM, '.' ,'.'},
    {PAT_INVERT|PAT_INVERT_CAPTURE|PAT_TERM, '.' ,'.'},
    {PAT_END, 0, 0}
};

static PatCharDef itemLevelDef[] = {
    {PAT_TERM, '.' ,'.'},
    {PAT_INVERT|PAT_INVERT_CAPTURE|PAT_TERM, '.' ,'.'},
    {PAT_END, 0, 0}
};

static PatCharDef nestDef[] = {
    {PAT_TERM, '-' ,'-'},
    {PAT_TERM, '>' ,'>'},
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
    {PAT_KO, '}', '}'},
    {PAT_KO|PAT_INVERT_CAPTURE, '#', '#'}, {PAT_KO|PAT_INVERT_CAPTURE, '.', '.'},
        {PAT_KO|PAT_KO_TERM|PAT_INVERT_CAPTURE, '*', '*'},
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
        textToTemplDef, FORMAT_TEMPL_TEXT, FORMAT_TEMPL_TEMPL);
    r |= Roebling_SetPattern(rbl,
        textToVarDef, FORMAT_TEMPL_TEXT, FORMAT_TEMPL_VAR);

    return r;
}

static status templ(MemCh *m, Roebling *rbl){
    status r = READY;
    printf("templ\n");
    fflush(stdout);
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        wsDef, FORMAT_TEMPL_WHITESPACE, FORMAT_TEMPL_TEMPL);
    r |= Roebling_SetPattern(rbl,
        templTokenDef, FORMAT_TEMPL_TOKEN, FORMAT_TEMPL_TEMPL);
    r |= Roebling_SetPattern(rbl,
        templContinueDef, FORMAT_TEMPL_CONTINUED, FORMAT_TEMPL_TEMPL);
    r |= Roebling_SetPattern(rbl,
        templEndDef, FORMAT_TEMPL_END, FORMAT_TEMPL_TEXT);
    r |= Roebling_SetPattern(rbl,
        propSepDef, FORMAT_TEMPL_PROP_SEP, FORMAT_TEMPL_TEMPL);
    r |= Roebling_SetPattern(rbl,
        attSepDef, FORMAT_TEMPL_ATT_SEP, FORMAT_TEMPL_TEMPL);
    r |= Roebling_SetPattern(rbl,
        pathSepDef, FORMAT_TEMPL_PATH_SEP, FORMAT_TEMPL_TEMPL);
    r |= Roebling_SetPattern(rbl,
        withDef, FORMAT_TEMPL_WITH, FORMAT_TEMPL_TEMPL);

    return r;
}

static status var(MemCh *m, Roebling *rbl){
    status r = READY;
    printf("var\n");
    fflush(stdout);
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        wsDef, FORMAT_TEMPL_WHITESPACE, FORMAT_TEMPL_VAR);
    r |= Roebling_SetPattern(rbl,
        varTokenDef, FORMAT_TEMPL_TOKEN, FORMAT_TEMPL_VAR);

    return r;
}

static status Capture(Roebling *rbl, word captureKey, StrVec *v){
    MemCh *m = rbl->m;
    Mess *mess = rbl->mess;
    TemplCtx *ctx = (TemplCtx*)as(rbl->source, TYPE_TEMPL_CTX);
    if(rbl->mess->type.state & DEBUG){
        Abstract *args[] = {
            (Abstract *)Type_ToStr(OutStream->m, captureKey),
            (Abstract *)v,
            NULL
        };
        Out("^c. Templ Capture ^0.$/^y.@^0.\n", args);
    }

    /*
    if(captureKey == FORMAT_TEMPL_TEXT || captureKey == FORMAT_TEMPL_INDENT){
        Iter_Add(&ctx->it, v);
    }else if(captureKey == FORMAT_TEMPL_VAR){
        Fetcher *fch = Fetcher_Make(m);
        Iter_Add(&ctx->it, (Abstract *)fch);
    }else if(captureKey == FORMAT_TEMPL_VAR_CLOSE){
        Fetcher *fch = (Fetcher *)Iter_Current(&ctx->it);
        if(fch->val.targets->nvalues == 0){
            fch->type.state = (fch->type.state & NORMAL_FLAGS) | FETCHER_END;
        }
    }else if(captureKey > _FORMAT_TEMPL_VAR_BODY_START &&
            captureKey < _FORMAT_TEMPL_VAR_BODY_END){
        Str *s = NULL;
        Fetcher *fch = (Fetcher *)Iter_Current(&ctx->it);
        if(v->p->nvalues == 1){
            s = Span_Get(v->p, 0);
        }else{
            s = Str_Make(m, v->total+1);
            Iter it;
            Iter_Init(&it, v->p);
            while((Iter_Next(&it) & END) == 0){
                Str *_s = (Str *)Iter_Get(&it);
                Str_Add(s, _s->bytes, _s->length);
            }
        }

        if(captureKey == FORMAT_TEMPL_VAR_PATH_SEP){
            fch->type.state = (fch->type.state & NORMAL_FLAGS) | FETCH_TARGET_KEY;
        }else if(captureKey == FORMAT_TEMPL_VAR_ATT_SEP){
            fch->type.state = (fch->type.state & NORMAL_FLAGS) | FETCH_TARGET_ATT;
        }else if(captureKey == FORMAT_TEMPL_VAR_FUNC_SEP){
            fch->type.state = (fch->type.state & NORMAL_FLAGS) | FETCH_TARGET_FUNC;
        }else if(captureKey == FORMAT_TEMPL_VAR_IDX){
            fch->type.state = (fch->type.state & NORMAL_FLAGS) | FETCH_TARGET_IDX;
        }else{
            FetchTarget *tg = NULL;
            if(fch->type.state & FETCH_TARGET_ATT){
                tg = FetchTarget_MakeAtt(m, s);
            }else if(fch->type.state & FETCH_TARGET_FUNC){
                tg = FetchTarget_MakeFunc(m, s);
            }else if(fch->type.state & FETCH_TARGET_IDX){
                tg = FetchTarget_MakeIdx(m, Int_FromStr(s));
            }else{
                tg = FetchTarget_MakeKey(m, s);
            }
            fch->type.state |= FETCHER_VAR;
            Span_Add(fch->val.targets, (Abstract *)tg);
        }
    }else if(captureKey > _FORMAT_TEMPL_LOGIC_START && 
            captureKey < _FORMAT_TEMPL_LOGIC_END){
        Fetcher *fch = (Fetcher *)Iter_Current(&ctx->it);
        if(captureKey == FORMAT_TEMPL_VAR_FOR){
            fch->type.state = (fch->type.state & NORMAL_FLAGS) | FETCHER_FOR;
            FetchTarget *tg = FetchTarget_MakeIter(m);
            Span_Add(fch->val.targets, (Abstract *)tg);
        }else if(captureKey == FORMAT_TEMPL_VAR_IF){
            fch->type.state = (fch->type.state & NORMAL_FLAGS) | FETCHER_IF;
        }else if(captureKey == FORMAT_TEMPL_VAR_IFNOT){
            fch->type.state = (fch->type.state & NORMAL_FLAGS) | FETCHER_IFNOT;
        }else if(captureKey == FORMAT_TEMPL_VAR_WITH){
            fch->type.state = (fch->type.state & NORMAL_FLAGS) | FETCHER_WITH;
        }
    }
    */
    return SUCCESS;
}

Roebling *Templ_RoeblingMake(MemCh *m, Cursor *curs, Abstract *source){
    Roebling *rbl = Roebling_Make(m, curs, Capture, NULL); 
    Roebling_AddStep(rbl, (Abstract *)I16_Wrapped(m, FORMAT_TEMPL_TEXT));
    Roebling_AddStep(rbl, (Abstract *)Do_Wrapped(m, (DoFunc)text));
    Roebling_AddStep(rbl, (Abstract *)I16_Wrapped(m, FORMAT_TEMPL_TEMPL));
    Roebling_AddStep(rbl, (Abstract *)Do_Wrapped(m, (DoFunc)templ));
    Roebling_AddStep(rbl, (Abstract *)I16_Wrapped(m, FORMAT_TEMPL_VAR));
    Roebling_AddStep(rbl, (Abstract *)Do_Wrapped(m, (DoFunc)var));
    Roebling_Start(rbl);

    rbl->capture = Capture;
    rbl->mess = Mess_Make(m);
    rbl->mess->tokenizer = Lookup_Make(m, _TEMPL_START);
    rbl->source = source;
    return rbl;
}
