#include <external.h>
#include <caneka.h>

static PatCharDef indentDef[] = {
    {PAT_ANY|PAT_INVERT_CAPTURE|PAT_TERM,' ' ,' '},{PAT_MANY|PAT_TERM, '=', '='},{PAT_ANY|PAT_TERM|PAT_CONSUME,' ' ,' '},
    {PAT_END, 0, 0}
};

static PatCharDef lineDef[] = {
    {PAT_KO, '\n', '\n'},{PAT_ANY|PAT_INVERT_CAPTURE|PAT_TERM, ' ', ' '},
    {PAT_KO, '\n', '\n'},patText,
    {PAT_END, 0, 0}
};

static PatCharDef clsDef[] = {
    {PAT_ANY|PAT_INVERT_CAPTURE|PAT_TERM,' ' ,' '},
    {PAT_MANY|PAT_KO|PAT_INVERT,'\\','\\'},
    {PAT_TERM|PAT_INVERT_CAPTURE,'.','.'},
    {PAT_MANY,'a','z'},{PAT_MANY,'A','Z'},{PAT_MANY,'0','9'},{PAT_MANY,'-','-'},
    {PAT_END, 0, 0}
};

static PatCharDef valueDef[] = {
    {PAT_ANY|PAT_INVERT_CAPTURE|PAT_TERM,' ' ,' '},
    {PAT_MANY|PAT_KO|PAT_INVERT,'\\','\\'},{PAT_KO,',',','}, patText,
    {PAT_END, 0, 0}
};

static PatCharDef kvDef[] = {
    {PAT_KO, ':', ':'}, patText,
    {PAT_END, 0, 0}
};

static PatCharDef nlDef[] = {
    {PAT_TERM, '\n', '\n'},
    {PAT_END, 0, 0}
};

static PatCharDef dashDef[] = {
    {PAT_ANY|PAT_INVERT_CAPTURE,' ', ' '},{PAT_TERM,'-','-'},{PAT_ANY|PAT_TERM,' ' ,' '},
    {PAT_END, 0, 0}
};

static PatCharDef tableDef[] = {
    {PAT_ANY, ' ', ' '}, {PAT_TERM, '+', '+'},
    {PAT_END, 0, 0}
};

static PatCharDef tagDef[] = {
    {PAT_MANY|PAT_KO|PAT_INVERT,'\\','\\'},
    {PAT_TERM|PAT_INVERT_CAPTURE, '_', '_'},
    {PAT_MANY|PAT_KO|PAT_INVERT,'\\','\\'},
    {PAT_KO|PAT_KO_TERM, '=', '='},
    patText,
    {PAT_END, 0, 0}
};

static PatCharDef labelDef[] = {
    {PAT_MANY|PAT_KO|PAT_INVERT,'\\','\\'},
    {PAT_KO|PAT_KO_TERM,'@', '@'},
    patAnyText,
    {PAT_END, 0, 0}
};

static PatCharDef urlTldDef[] = {
    {PAT_KO|PAT_INVERT_CAPTURE|PAT_OPTIONAL|PAT_KO_TERM, '.', '.'},
        {PAT_KO|PAT_INVERT_CAPTURE, ' ', ' '},
        {PAT_KO|PAT_INVERT_CAPTURE, '\t', '\t'},
        {PAT_KO|PAT_INVERT_CAPTURE, '\r', '\r'},
        {PAT_KO|PAT_INVERT_CAPTURE|PAT_KO_TERM, '\n', '\n'},
    {PAT_INVERT|PAT_MANY|PAT_TERM, 0, 31},
    {PAT_END, 0, 0}
};

static status start(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    Match *mt = NULL;

    r |= Roebling_SetPattern(rbl,
        nlDef, FORMATTER_NEXT, FORMATTER_START);
    r |= Roebling_SetPattern(rbl,
        clsDef, FORMATTER_CLASS, FORMATTER_START);
    r |= Roebling_SetPattern(rbl,
        indentDef, FORMATTER_INDENT, FORMATTER_LINE);
    Match *mtd = Roebling_GetMatch(rbl);
    r |= Roebling_SetPattern(rbl,
        dashDef, FORMATTER_BULLET, FORMATTER_LINE);
    r |= Roebling_SetPattern(rbl,
        tableDef, FORMATTER_TABLE, FORMATTER_TABLE_VALUE);
    r |= Roebling_SetPattern(rbl,
        lineDef, FORMATTER_LINE, FORMATTER_START);
    r |= Roebling_SetPattern(rbl,
        tagDef, FORMATTER_TAG, FORMATTER_LABEL);
    mt = Roebling_GetMatch(rbl);
    mt->type.state |= MATCH_SEARCH;
    r |= Roebling_SetPattern(rbl,
        kvDef, FORMATTER_KEY, FORMATTER_LINE);

    return r;
}

static status line(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        lineDef, FORMATTER_LINE, FORMATTER_START);
    r |= Roebling_SetPattern(rbl,
        tagDef, FORMATTER_LINE, FORMATTER_LABEL);

    Match *mt = Roebling_GetMatch(rbl);
    mt->type.state |= MATCH_SEARCH;

    return r;
}

static status value(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        valueDef, FORMATTER_VALUE, FORMATTER_VALUE);
    r |= Roebling_SetPattern(rbl,
        lineDef, FORMATTER_LAST_VALUE, FORMATTER_START);

    return r;
}

static status tableValue(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        clsDef, FORMATTER_CLASS, FORMATTER_TABLE_VALUE);
    r |= Roebling_SetPattern(rbl,
        valueDef, FORMATTER_TABLE_VALUE, FORMATTER_TABLE_VALUE);
    r |= Roebling_SetPattern(rbl,
        lineDef, FORMATTER_LAST_TABLE_VALUE, FORMATTER_TABLE_VALUE);
    r |= Roebling_SetPattern(rbl,
        nlDef, FORMATTER_NEXT, FORMATTER_START);

    return r;
}

static status label(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        labelDef, FORMATTER_LABEL, FORMATTER_TAG_VALUE);

    return r;
}

static status tagValue(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        urlTldDef, FORMATTER_URL, FORMATTER_START);

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
        Out("^c.Fmt After Capture ^E0.$^ec./@ -> @\n", args);
    }
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
        Error(rbl->m, (Abstract *)rbl, FUNCNAME, FILENAME, LINENUMBER,
            "Unable to find Tokenize for this captureKey: $", args);
        return ERROR;
    }
    return SUCCESS;
}

Roebling *FormatFmt_Make(MemCh *m, Cursor *curs, Abstract *source){
    DebugStack_Push(curs, curs->type.of);
    Roebling *rbl = Roebling_Make(m, curs, Capture, NULL); 
    Roebling_AddStep(rbl, (Abstract *)I16_Wrapped(m, FORMATTER_START));
    Roebling_AddStep(rbl, (Abstract *)Do_Wrapped(m, (DoFunc)start));
    Roebling_AddStep(rbl, (Abstract *)I16_Wrapped(m, FORMATTER_LINE));
    Roebling_AddStep(rbl, (Abstract *)Do_Wrapped(m, (DoFunc)line));
    Roebling_AddStep(rbl, (Abstract *)I16_Wrapped(m, FORMATTER_VALUE));
    Roebling_AddStep(rbl, (Abstract *)Do_Wrapped(m, (DoFunc)value));
    Roebling_AddStep(rbl, (Abstract *)I16_Wrapped(m, FORMATTER_TABLE_VALUE));
    Roebling_AddStep(rbl, (Abstract *)Do_Wrapped(m, (DoFunc)tableValue));
    Roebling_AddStep(rbl, (Abstract *)I16_Wrapped(m, FORMATTER_LABEL));
    Roebling_AddStep(rbl, (Abstract *)Do_Wrapped(m, (DoFunc)label));
    Roebling_AddStep(rbl, (Abstract *)I16_Wrapped(m, FORMATTER_TAG_VALUE));
    Roebling_AddStep(rbl, (Abstract *)Do_Wrapped(m, (DoFunc)tagValue));
    Roebling_AddStep(rbl, (Abstract *)I16_Wrapped(m, FORMATTER_END));
    Roebling_Start(rbl);

    rbl->capture = Capture;

    rbl->mess = Mess_Make(m);
    rbl->mess->tokenizer = FormatFmt_Defs;
    rbl->source = source;
    DebugStack_Pop();
    return rbl;
}
