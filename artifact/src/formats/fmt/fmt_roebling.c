#include <external.h>
#include <caneka.h>

static PatCharDef indentDef[] = {
    {PAT_ANY|PAT_INVERT_CAPTURE,' ' ,' '},{PAT_MANY|PAT_TERM, '=', '='},{PAT_ANY|PAT_INVERT_CAPTURE|PAT_TERM,' ' ,' '},
    {PAT_END, 0, 0}
};

static PatCharDef lineDef[] = {
    {PAT_KO, '\n', '\n'}, patText,
    {PAT_END, 0, 0}
};

static PatCharDef leadLineDef[] = {
    {PAT_KO, '\n', '\n'}, {PAT_TERM|PAT_INVERT_CAPTURE, ' ', ' '}, {PAT_KO, '\n', '\n'}, patText,
    {PAT_END, 0, 0}
};

static PatCharDef kvKeyDef[] = {
    {PAT_KO, '\n','\n'}, {PAT_KO,':', ':'}, patText,
    {PAT_ANY,' ',' '}, {PAT_ANY|PAT_TERM,'\r','\r'},{PAT_ANY|PAT_TERM,'\t','\t'},
    {PAT_INVERT|PAT_INVERT_CAPTURE|PAT_TERM,'\n', '\n'},
    {PAT_END, 0, 0}
};

static PatCharDef noLeadLineDef[] = {
    {PAT_ANY|PAT_INVERT_CAPTURE|PAT_TERM, ' ', ' '},
    {PAT_KO, '\n', '\n'}, patText,
    {PAT_END, 0, 0}
};

static PatCharDef nlDef[] = {
    {PAT_TERM, '\n', '\n'},
    {PAT_END, 0, 0}
};

static PatCharDef dashDef[] = {
    {PAT_ANY|PAT_INVERT_CAPTURE, ' ', ' '}, {PAT_TERM, '-', '-'},{PAT_ANY|PAT_INVERT_CAPTURE|PAT_TERM,' ' ,' '},
    {PAT_END, 0, 0}
};

static PatCharDef plusDef[] = {
    {PAT_ANY, ' ', ' '}, {PAT_TERM, '+', '+'},
    {PAT_END, 0, 0}
};

static PatCharDef cmdDef[] = {
    {PAT_TERM|PAT_INVERT_CAPTURE, '.', '.'},{PAT_KO, ':', ':'}, patText,
    {PAT_END, 0, 0}
};

static PatCharDef commaSepDef[] = {
    {PAT_KO, ',', ','}, patText,
    {PAT_END, 0, 0}
};

static PatCharDef lastValueDef[] = {
    {PAT_KO,'\n','\n'}, patText,
    {PAT_END, 0, 0}
};

static status start(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        nlDef, FORMATTER_NEXT, FORMATTER_START); r |= Roebling_SetPattern(rbl,
        indentDef, FORMATTER_INDENT, FORMATTER_LINE);
    r |= Roebling_SetPattern(rbl,
        dashDef, FORMATTER_ALIAS, FORMATTER_NOLEADLINE);
    r |= Roebling_SetPattern(rbl,
        plusDef, FORMATTER_ALIAS, FORMATTER_VALUE);
    r |= Roebling_SetPattern(rbl,
        cmdDef, FORMATTER_METHOD, FORMATTER_VALUE);
    r |= Roebling_SetPattern(rbl,
        kvKeyDef, FORMATTER_KV_KEY, FORMATTER_KV_VALUE);
    r |= Roebling_SetPattern(rbl,
        leadLineDef, FORMATTER_LEAD_LINE, FORMATTER_START);
    r |= Roebling_SetPattern(rbl,
        lineDef, FORMATTER_LINE, FORMATTER_START);

    return r;
}

static status line(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        lineDef, FORMATTER_LINE, FORMATTER_START);

    return r;
}

static status noLeadLine(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        dashDef, FORMATTER_ALIAS, FORMATTER_NOLEADLINE);
    r |= Roebling_SetPattern(rbl,
        noLeadLineDef, FORMATTER_LINE, FORMATTER_NOLEADLINE);
    r |= Roebling_SetPattern(rbl,
        nlDef, FORMATTER_NEXT, FORMATTER_START);

    return r;
}

static status value(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        nlDef, FORMATTER_NEXT, FORMATTER_START);
    r |= Roebling_SetPattern(rbl,
        commaSepDef, FORMATTER_VALUE, FORMATTER_VALUE);
    r |= Roebling_SetPattern(rbl,
        lastValueDef, FORMATTER_LAST_VALUE, FORMATTER_VALUE);

    return r;
}

static status kvValue(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        lineDef, FORMATTER_KV_VALUE, FORMATTER_START);

    return r;
}

static status Capture(Roebling *rbl, word captureKey, StrVec *v){
    Mess *mset = rbl->mess;
    if(rbl->type.state & DEBUG){
        Abstract *args[] = {
            (Abstract *)Type_ToStr(OutStream->m, captureKey),
            (Abstract *)v,
            (Abstract *)rbl,
            NULL
        };
        Debug("^p.Fmt Capture $/@ from @^0\n", args);
    }
    return SUCCESS;
}

Roebling *FormatFmt_Make(MemCh *m, Cursor *curs, Abstract *source){
    Roebling *rbl = Roebling_Make(m, curs, Capture, NULL); 
    Roebling_AddStep(rbl, (Abstract *)I16_Wrapped(m, FORMATTER_START));
    Roebling_AddStep(rbl, (Abstract *)Do_Wrapped(m, (DoFunc)start));
    Roebling_AddStep(rbl, (Abstract *)I16_Wrapped(m, FORMATTER_LINE));
    Roebling_AddStep(rbl, (Abstract *)Do_Wrapped(m, (DoFunc)line));
    Roebling_AddStep(rbl, (Abstract *)I16_Wrapped(m, FORMATTER_NOLEADLINE));
    Roebling_AddStep(rbl, (Abstract *)Do_Wrapped(m, (DoFunc)noLeadLine));
    Roebling_AddStep(rbl, (Abstract *)I16_Wrapped(m, FORMATTER_VALUE));
    Roebling_AddStep(rbl, (Abstract *)Do_Wrapped(m, (DoFunc)value));
    Roebling_AddStep(rbl, (Abstract *)I16_Wrapped(m, FORMATTER_KV_VALUE));
    Roebling_AddStep(rbl, (Abstract *)Do_Wrapped(m, (DoFunc)kvValue));
    Roebling_AddStep(rbl, (Abstract *)I16_Wrapped(m, FORMATTER_END));
    Roebling_Start(rbl);

    rbl->capture = Capture;

    rbl->mess = Mess_Make(m);
    rbl->mess->tokenizer = FormatFmt_Defs;
    rbl->source = source;
    return rbl;
}
