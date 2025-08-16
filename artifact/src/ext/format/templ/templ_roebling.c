#include <external.h>
#include <caneka.h>

static PatCharDef textDef[] = {
    {PAT_KO|PAT_KO_TERM|PAT_INVERT_CAPTURE,'$' ,'$'},
    patText,
    {PAT_END, 0, 0}
};

static PatCharDef varDef[] = {
    {PAT_TERM|PAT_INVERT_CAPTURE|PAT_CONSUME, '$' ,'$'},
    {PAT_TERM|PAT_INVERT_CAPTURE|PAT_CONSUME, '{', '{'},
    {PAT_END, 0, 0}
};

static PatCharDef varCloseDef[] = {
    {PAT_TERM|PAT_INVERT_CAPTURE|PAT_CONSUME, '}', '}'},
    {PAT_END, 0, 0}
};

static PatCharDef varBodyDef[] = {
    {PAT_KO|PAT_INVERT_CAPTURE, '}', '}'}, {PAT_KO|PAT_INVERT_CAPTURE, '!', '!'}, 
    {PAT_KO|PAT_INVERT_CAPTURE, '#', '#'}, {PAT_KO|PAT_INVERT_CAPTURE, '?', '?'},
    {PAT_KO|PAT_INVERT_CAPTURE, ':', ':'}, {PAT_KO|PAT_INVERT_CAPTURE, '.', '.'},
        {PAT_KO|PAT_KO_TERM|PAT_INVERT_CAPTURE, '*', '*'},
    patText,
    {PAT_END, 0, 0}
};

static PatCharDef varPathSepDef[] = {
    {PAT_TERM, '.' ,'.'},
    {PAT_END, 0, 0}
};

static PatCharDef varAttSepDef[] = {
    {PAT_TERM, '*' ,'*'},
    {PAT_END, 0, 0}
};

static PatCharDef varIfDef[] = {
    {PAT_ANY|PAT_INVERT_CAPTURE, ' ', ' '}, {PAT_TERM|PAT_ANY|PAT_INVERT_CAPTURE, '\t', '\t'},
    {PAT_TERM, '?' ,'?'},
    {PAT_END, 0, 0}
};

static PatCharDef varForDef[] = {
    {PAT_ANY|PAT_INVERT_CAPTURE, ' ', ' '}, {PAT_TERM|PAT_ANY|PAT_INVERT_CAPTURE, '\t', '\t'},
    {PAT_TERM|PAT_INVERT_CAPTURE|PAT_CONSUME, ':' ,':'},
    {PAT_END, 0, 0}
};


static PatCharDef varIfNotDef[] = {
    {PAT_ANY|PAT_INVERT_CAPTURE, ' ', ' '}, 
        {PAT_TERM|PAT_ANY|PAT_INVERT_CAPTURE, '\t', '\t'},
    {PAT_TERM|PAT_INVERT_CAPTURE, '?' ,'?'},
    {PAT_END, 0, 0}
};

static PatCharDef varItemDef[] = {
    {PAT_TERM|PAT_INVERT_CAPTURE|PAT_CONSUME, '@', '@'},
    {PAT_TERM|PAT_INVERT_CAPTURE, '}', '}'},
    {PAT_END, 0, 0}
};

static PatCharDef varImpliedDef[] = {
    {PAT_TERM|PAT_INVERT_CAPTURE|PAT_CONSUME, '_', '_'},
    {PAT_TERM|PAT_INVERT_CAPTURE, '}', '}'},
    {PAT_END, 0, 0}
};

static PatCharDef varIdxDef[] = {
    {PAT_TERM|PAT_INVERT_CAPTURE|PAT_CONSUME, '#', '#'},
    {PAT_ANY|PAT_TERM, '0', '9'},
    {PAT_END, 0, 0}
};

static status text(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        textDef, FORMAT_TEMPL_TEXT, FORMAT_TEMPL_VAR);

    return r;
}

static status var(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    Match *mt = NULL;
    r |= Roebling_SetPattern(rbl,
        varDef, FORMAT_TEMPL_VAR, FORMAT_TEMPL_VAR_BODY);
    mt = Roebling_GetMatch(rbl);
    mt->type.state |= MATCH_ACCEPT_EMPTY;

    return r;
}

static status varBody(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    Match *mt = NULL;
    r |= Roebling_SetPattern(rbl,
        varCloseDef, FORMAT_TEMPL_VAR_CLOSE, FORMAT_TEMPL_TEXT);
    mt = Roebling_GetMatch(rbl);
    mt->type.state |= MATCH_ACCEPT_EMPTY;

    r |= Roebling_SetPattern(rbl,
        varBodyDef, FORMAT_TEMPL_VAR_BODY, FORMAT_TEMPL_VAR_BODY);
    r |= Roebling_SetPattern(rbl,
        varPathSepDef, FORMAT_TEMPL_VAR_PATH_SEP, FORMAT_TEMPL_VAR_BODY);
    r |= Roebling_SetPattern(rbl,
        varForDef, FORMAT_TEMPL_VAR_FOR, FORMAT_TEMPL_VAR_BODY);
    mt = Roebling_GetMatch(rbl);
    mt->type.state |= MATCH_ACCEPT_EMPTY;
    r |= Roebling_SetPattern(rbl,
        varAttSepDef, FORMAT_TEMPL_VAR_ATT_SEP, FORMAT_TEMPL_VAR_BODY);
    r |= Roebling_SetPattern(rbl,
        varIdxDef, FORMAT_TEMPL_VAR_IDX, FORMAT_TEMPL_VAR_BODY);
    mt = Roebling_GetMatch(rbl);
    mt->type.state |= MATCH_ACCEPT_EMPTY;
    r |= Roebling_SetPattern(rbl,
        varItemDef, FORMAT_TEMPL_VAR_ITEM, FORMAT_TEMPL_VAR_BODY);
    mt = Roebling_GetMatch(rbl);
    mt->type.state |= MATCH_ACCEPT_EMPTY;
    r |= Roebling_SetPattern(rbl,
        varImpliedDef, FORMAT_TEMPL_VAR_IMPLIED, FORMAT_TEMPL_VAR_BODY);
    mt = Roebling_GetMatch(rbl);
    mt->type.state |= MATCH_ACCEPT_EMPTY;

    /*
    r |= Roebling_SetPattern(rbl,
        varIfDef, FORMAT_TEMPL_VAR_ENDFOR, FORMAT_TEMPL_VAR_BODY);
    r |= Roebling_SetPattern(rbl,
        varIfNotDef, FORMAT_TEMPL_VAR_IFNOT, FORMAT_TEMPL_VAR_BODY);
    */
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
        Out("^c. Templ Capture ^E0.$^ec./@^0.\n", args);
    }

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
    return SUCCESS;
}

Roebling *Templ_RoeblingMake(MemCh *m, Cursor *curs, Abstract *source){
    Roebling *rbl = Roebling_Make(m, curs, Capture, NULL); 
    Roebling_AddStep(rbl, (Abstract *)I16_Wrapped(m, FORMAT_TEMPL_TEXT));
    Roebling_AddStep(rbl, (Abstract *)Do_Wrapped(m, (DoFunc)text));
    Roebling_AddStep(rbl, (Abstract *)I16_Wrapped(m, FORMAT_TEMPL_VAR));
    Roebling_AddStep(rbl, (Abstract *)Do_Wrapped(m, (DoFunc)var));
    Roebling_AddStep(rbl, (Abstract *)I16_Wrapped(m, FORMAT_TEMPL_VAR_BODY));
    Roebling_AddStep(rbl, (Abstract *)Do_Wrapped(m, (DoFunc)varBody));
    Roebling_Start(rbl);

    rbl->capture = Capture;
    rbl->mess = Mess_Make(m);
    rbl->mess->tokenizer = Lookup_Make(m, _APPS_TEMPL_START);
    rbl->source = source;
    return rbl;
}
