#include <external.h>
#include <caneka.h>

static PatCharDef leadDef[] = {
    {PAT_MANY|PAT_TERM, ' ', ' '},
    {PAT_END, 0, 0}
};

static PatCharDef indentDef[] = {
    {PAT_ANY|PAT_INVERT_CAPTURE|PAT_TERM,' ' ,' '},{PAT_MANY|PAT_TERM, '=', '='},{PAT_ANY|PAT_TERM|PAT_CONSUME,' ' ,' '},
    {PAT_END, 0, 0}
};

static PatCharDef lineDef[] = {
    {PAT_KO, '\n', '\n'},{PAT_ANY|PAT_INVERT_CAPTURE|PAT_TERM, ' ', ' '},
    {PAT_KO, '\n', '\n'},patText,
    {PAT_END, 0, 0}
};

static PatCharDef numberDef[] = {
    {PAT_ANY|PAT_INVERT_CAPTURE|PAT_TERM, ' ', ' '},
    {PAT_MANY|PAT_TERM, '0', '9'},
    {PAT_END, 0, 0}
};

static PatCharDef keyDef[] = {
    {PAT_KO, ':', ':'}, patText,
    {PAT_END, 0, 0}
};

static PatCharDef bulletDef[] = {
    {PAT_TERM, '-', '-'}, {PAT_ANY|PAT_INVERT_CAPTURE|PAT_TERM, ' ', ' '},
    {PAT_KO, '\n', '\n'},
    patText,
    {PAT_END, 0, 0}
};


static PatCharDef nlDef[] = {
    {PAT_TERM, '\n', '\n'},
    {PAT_END, 0, 0}
};

static status line(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl,
        nlDef, CONFIG_NL, CONFIG_START);
    r |= Roebling_SetPattern(rbl,
        numberDef, CONFIG_NUMBER, CONFIG_START);
    r |= Roebling_SetPattern(rbl,
        lineDef, CONFIG_LINE, CONFIG_START);
    return r;
}

static status start(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl,
        leadDef, CONFIG_LEAD, CONFIG_START);
    r |= Roebling_SetPattern(rbl,
        indentDef, CONFIG_INDENT, CONFIG_START);
    r |= Roebling_SetPattern(rbl,
        bulletDef, CONFIG_ITEM, CONFIG_START);
    r |= Roebling_SetPattern(rbl,
        numberDef, CONFIG_NUMBER, CONFIG_START);
    r |= Roebling_SetPattern(rbl,
        lineDef, CONFIG_LINE, CONFIG_START);
    r |= Roebling_SetPattern(rbl,
        nlDef, CONFIG_NL, CONFIG_START);
    r |= Roebling_SetPattern(rbl,
        keyDef, CONFIG_KEY, CONFIG_LINE);
    return r;
}

static status Capture(Roebling *rbl, word captureKey, StrVec *v){
    Mess *mess = rbl->mess;
    Tokenize *tk = Lookup_Get(mess->tokenizer, captureKey);
    if(rbl->mess->type.state & DEBUG){
        Abstract *args[] = {
            (Abstract *)Type_ToStr(OutStream->m, captureKey),
            (Abstract *)v,
            (Abstract *)tk,
            NULL
        };
        Out("^c.Config Capture ^E0.$^ec./@ -> @\n", args);
    }
    /*
    if(tk != NULL){
        if(mess->current->parent == NULL && tk->captureKey == FORMATTER_LINE){
            tk = Lookup_Get(mess->tokenizer, FORMATTER_PARAGRAPH);
        }
        Mess_Tokenize(mess, tk, v);
    }else{
        Abstract *args[] = {
            (Abstract *)Type_ToStr(rbl->m, captureKey),
            NULL
        };
        Error(rbl->m, FUNCNAME, FILENAME, LINENUMBER,
            "Unable to find Tokenize for this captureKey: $", args);
        return ERROR;
    }
    */
    return SUCCESS;
}

Roebling *FormatConfig_Make(MemCh *m, Cursor *curs, Abstract *source){
    DebugStack_Push(curs, curs->type.of);
    Roebling *rbl = Roebling_Make(m, curs, Capture, NULL); 
    Roebling_AddStep(rbl, (Abstract *)I16_Wrapped(m, CONFIG_START));
    Roebling_AddStep(rbl, (Abstract *)Do_Wrapped(m, (DoFunc)start));
    Roebling_AddStep(rbl, (Abstract *)I16_Wrapped(m, CONFIG_LINE));
    Roebling_AddStep(rbl, (Abstract *)Do_Wrapped(m, (DoFunc)line));
    Roebling_AddStep(rbl, (Abstract *)I16_Wrapped(m, CONFIG_END));
    Roebling_Start(rbl);

    rbl->capture = Capture;

    rbl->mess = Mess_Make(m);
    rbl->mess->tokenizer = FormatFmt_Defs;
    rbl->source = source;
    DebugStack_Pop();
    return rbl;
}
