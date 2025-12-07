#include <external.h>
#include <caneka.h>

static PatCharDef leadDef[] = {
    {PAT_MANY, '\t', '\t'},
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

static PatCharDef indentArrDef[] = {
    {PAT_ANY|PAT_INVERT_CAPTURE|PAT_TERM,' ' ,' '},{PAT_TERM, '[', '['},
        {PAT_ANY|PAT_TERM|PAT_CONSUME,' ' ,' '},
    {PAT_END, 0, 0}
};

static PatCharDef outdentArrDef[] = {
    {PAT_ANY|PAT_INVERT_CAPTURE|PAT_TERM,' ' ,' '},{PAT_TERM, ']', ']'},
        {PAT_ANY|PAT_TERM|PAT_CONSUME,' ' ,' '},
    {PAT_END, 0, 0}
};

static PatCharDef quotedStringDef[] = {
    {PAT_TERM|PAT_INVERT_CAPTURE, '"', '"'},
    {PAT_MANY|PAT_KO|PAT_INVERT, '\\', '\\'},
    {PAT_MANY|PAT_KO, '"', '"'}, patText,
    {PAT_END, 0, 0}
};

static PatCharDef numberDef[] = {
    {PAT_ANY|PAT_INVERT_CAPTURE|PAT_TERM, ' ', ' '},
    {PAT_MANY|PAT_TERM, '0', '9'},
    {PAT_END, 0, 0}
};

static PatCharDef commaSepDef[] = {
    {PAT_TERM, ',', ','},
    {PAT_END, 0, 0}
};

static PatCharDef keySepDef[] = {
    {PAT_TERM, ':', ':'},
    {PAT_END, 0, 0}
};

static status start(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        leadDef, JSON_LEAD, JSON_START);
    r |= Roebling_SetPattern(rbl,
        indentDef, JSON_INDENT, JSON_KEY);
    r |= Roebling_SetPattern(rbl,
        indentArrDef, JSON_ARR_INDENT, JSON_ARR);
    r |= Roebling_SetPattern(rbl,
        quotedStringDef, JSON_STRING, JSON_COMMA_SEP);
    r |= Roebling_SetPattern(rbl,
        numberDef, JSON_NUMBER, JSON_END);

    return r;
}

static status key(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl,
        leadDef, JSON_LEAD, JSON_START);
    r |= Roebling_SetPattern(rbl,
        quotedStringDef, JSON_KEY, JSON_KEY_SEP);

    return r;
}

static status arr(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl,
        leadDef, JSON_LEAD, JSON_ARR);
    r |= Roebling_SetPattern(rbl,
        quotedStringDef, JSON_VALUE, JSON_KEY_SEP);
    r |= Roebling_SetPattern(rbl,
        numberDef, JSON_NUMBER, JSON_KEY_SEP);

    return r;
}

static status keySep(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl,
        leadDef, JSON_LEAD, JSON_KEY_SEP);
    r |= Roebling_SetPattern(rbl,
        keySepDef, JSON_KEY_SEP, JSON_START);

    return r;
}

static status arrSep(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl,
        leadDef, JSON_LEAD, JSON_COMMA_SEP);
    r |= Roebling_SetPattern(rbl,
        commaSepDef, JSON_COMMA_SEP, JSON_ARR);
    r |= Roebling_SetPattern(rbl,
        outdentDef, JSON_OUTDENT, JSON_START);
    r |= Roebling_SetPattern(rbl,
        outdentArrDef, JSON_ARR_OUTDENT, JSON_START);

    return r;
}

static status Capture(Roebling *rbl, word captureKey, StrVec *v){
    void *args[7];
    MemCh *m = rbl->m;
    Iter *it = (Iter *)as(rbl->dest, TYPE_ITER);
    NodeObj *node = (NodeObj *)as(rbl->source, TYPE_NODEOBJ);

    if(it->type.state & DEBUG){
        args[0] = Type_ToStr(OutStream->m, captureKey);
        args[1] = v,
        args[2] = it->p;
        args[3] = node;
        args[4] = NULL;
        Out("^c.Json Capture ^E0.$^ec. -> ^0y.@\n    it: @\n    @\n", args);
    }

    return ZERO;
}

Roebling *JsonParser_Make(MemCh *m, Cursor *curs, void *source){
    DebugStack_Push(curs, curs->type.of);
    Roebling *rbl = Roebling_Make(m, curs, Capture, NULL); 
    Roebling_AddStep(rbl, I16_Wrapped(m, JSON_START));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)start));
    Roebling_AddStep(rbl, I16_Wrapped(m, JSON_KEY));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)key));
    Roebling_AddStep(rbl, I16_Wrapped(m, JSON_ARR));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)arr));
    Roebling_AddStep(rbl, I16_Wrapped(m, JSON_KEY_SEP));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)keySep));
    Roebling_AddStep(rbl, I16_Wrapped(m, JSON_COMMA_SEP));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)arrSep));
    Roebling_AddStep(rbl, I16_Wrapped(m, JSON_END));
    Roebling_Start(rbl);

    rbl->capture = Capture;
    rbl->source = source;
    rbl->dest = (Abstract *)Iter_Make(m, Span_Make(m));

    Iter_Add((Iter *)rbl->dest, rbl->source);

    DebugStack_Pop();
    return rbl;
}
