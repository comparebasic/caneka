#include <external.h>
#include <caneka.h>

static PatCharDef leadDef[] = {
    {PAT_MANY|PAT_TERM, ' ', ' '},
    {PAT_END, 0, 0}
};

static PatCharDef indentDef[] = {
    {PAT_ANY|PAT_INVERT_CAPTURE|PAT_TERM,' ' ,' '},{PAT_TERM, '{', '{'},{PAT_ANY|PAT_TERM|PAT_CONSUME,' ' ,' '},
    {PAT_END, 0, 0}
};

static PatCharDef outdentDef[] = {
    {PAT_ANY|PAT_INVERT_CAPTURE|PAT_TERM,' ' ,' '},{PAT_TERM, '}', '}'},{PAT_ANY|PAT_TERM|PAT_CONSUME,' ' ,' '},
    {PAT_END, 0, 0}
};

static PatCharDef lineDef[] = {
    {PAT_KO|PAT_KO_TERM, '\n', '\n'},{PAT_ANY|PAT_INVERT_CAPTURE|PAT_TERM, ' ', ' '},
    {PAT_KO|PAT_INVERT_CAPTURE, '{', '{'},{PAT_KO|PAT_INVERT_CAPTURE, '}', '}'}, {PAT_KO|PAT_KO_TERM, '\n', '\n'},
    patText,
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
        indentDef, CONFIG_INDENT, CONFIG_START);
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
        indentDef, CONFIG_INDENT, CONFIG_START);
    r |= Roebling_SetPattern(rbl,
        outdentDef, CONFIG_OUTDENT, CONFIG_START);
    r |= Roebling_SetPattern(rbl,
        lineDef, CONFIG_LINE, CONFIG_START);
    r |= Roebling_SetPattern(rbl,
        nlDef, CONFIG_NL, CONFIG_START);
    r |= Roebling_SetPattern(rbl,
        keyDef, CONFIG_KEY, CONFIG_LINE);
    return r;
}

static status Capture(Roebling *rbl, word captureKey, StrVec *v){
    Iter *it = (Iter *)as(rbl->dest, TYPE_ITER);
    if(it->type.state & DEBUG){
        Abstract *args[] = {
            (Abstract *)Type_ToStr(OutStream->m, captureKey),
            (Abstract *)v,
            NULL
        };
        Out("^c.Config Capture ^E0.$^ec./$\n", args);
    }
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
    rbl->source = source;
    rbl->dest = (Abstract *)Iter_Make(m, Span_Make(m));

    DebugStack_Pop();
    return rbl;
}
