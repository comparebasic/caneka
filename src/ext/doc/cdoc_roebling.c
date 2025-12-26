#include <external.h>
#include <caneka.h>

static PatCharDef funcDef[] = {
    {PAT_SINGLE, 'a', 'z'},
    {PAT_SINGLE|PAT_TERM, 'A', 'Z'},
    {PAT_MANY, 'a', 'z'},
    {PAT_MANY, '0', '9'},
    {PAT_MANY|PAT_TERM, 'A', 'Z'},
    {PAT_MANY|PAT_TERM, ' ', ' '},
    {PAT_KO|PAT_KO_TERM, '{', '{'},
    {PAT_MANY, 'a', 'z'},
    {PAT_MANY, '0', '9'},
    {PAT_MANY, 'A', 'Z'},
    {PAT_MANY, ' ', ' '},
    {PAT_MANY, ',', ','},
    {PAT_MANY, '_', '_'},
    {PAT_MANY|PAT_TERM, '(', '*'},
    {PAT_END, 0, 0}
};

static PatCharDef funcMultiLineDef[] = {
    {PAT_SINGLE, 'a', 'z'},
    {PAT_SINGLE|PAT_TERM, 'A', 'Z'},
    {PAT_MANY, 'a', 'z'},
    {PAT_MANY, '0', '9'},
    {PAT_MANY|PAT_TERM, 'A', 'Z'},
    {PAT_MANY|PAT_TERM, ' ', ' '},
    {PAT_KO|PAT_KO_TERM, '{', '{'},
    {PAT_MANY, 'a', 'z'},
    {PAT_MANY, '0', '9'},
    {PAT_MANY, 'A', 'Z'},
    {PAT_MANY, ' ', ' '},
    {PAT_MANY, ',', ','},
    {PAT_MANY, '_', '_'},
    {PAT_MANY, '(', '*'},
    {PAT_MANY|PAT_TERM, '\n', '\n'},
    {PAT_END, 0, 0}
};

static PatCharDef includePathDef[] = {
    {PAT_TERM, '#', '#'},
    {PAT_TERM, 'i', 'i'},
    {PAT_TERM, 'n', 'n'},
    {PAT_TERM, 'c', 'c'},
    {PAT_TERM, 'l', 'l'},
    {PAT_TERM, 'u', 'u'},
    {PAT_TERM, 'd', 'd'},
    {PAT_TERM, 'e', 'e'},
    {PAT_TERM|PAT_MANY, ' ', ' '},
    {PAT_SINGLE|PAT_TERM, '<', '<'},
    {PAT_MANY, 'a', 'z'}, {PAT_MANY, 'A', 'Z'}, {PAT_MANY, '0', '9'},
        {PAT_MANY, '-', '-'}, {PAT_MANY, '_', '_'}, {PAT_MANY|PAT_TERM, '.', '.'},
    {PAT_SINGLE|PAT_TERM, '>', '>'},
    {PAT_ANY|PAT_INVERT_CAPTURE, ' ', ' '}, {PAT_SINGLE|PAT_TERM|PAT_CONSUME, '\n', '\n'},
    {PAT_END, 0, 0}
};

static PatCharDef includeLocalDef[] = {
    {PAT_TERM, '#', '#'},
    {PAT_TERM, 'i', 'i'},
    {PAT_TERM, 'n', 'n'},
    {PAT_TERM, 'c', 'c'},
    {PAT_TERM, 'l', 'l'},
    {PAT_TERM, 'u', 'u'},
    {PAT_TERM, 'd', 'd'},
    {PAT_TERM, 'e', 'e'},
    {PAT_MANY|PAT_TERM, ' ', ' '},
    {PAT_SINGLE|PAT_TERM, '"', '"'},
    {PAT_MANY, 'a', 'z'}, {PAT_MANY, 'A', 'Z'}, {PAT_MANY, '0', '9'},
        {PAT_MANY, '-', '-'}, {PAT_MANY, '_', '_'}, {PAT_MANY|PAT_TERM, '.', '.'},
    {PAT_SINGLE|PAT_TERM, '"', '"'},
    {PAT_ANY|PAT_CONSUME, ' ', ' '}, {PAT_SINGLE|PAT_TERM|PAT_CONSUME, '\n', '\n'},
    {PAT_END, 0, 0}
};

static PatCharDef funcEndDef[] = {
    {PAT_SINGLE|PAT_TERM, '}', '}'},
    {PAT_TERM|PAT_CONSUME, '\n', '\n'},
    {PAT_END, 0, 0}
};

static PatCharDef blankLineDef[] = {
    {PAT_SINGLE, '\n', '\n'},
    {PAT_END, 0, 0}
};

static PatCharDef lineDef[] = {
    {PAT_MANY|PAT_TERM, ' ', ' '},
    {PAT_KO|PAT_KO_TERM, '\n', '\n'},
    {PAT_MANY, '\t', '\t'}, {PAT_MANY, '\r', '\r'}, {PAT_INVERT|PAT_MANY|PAT_TERM, 0, 31},
    {PAT_END, 0, 0}
};

static PatCharDef commentDef[] = {
    {PAT_MANY|PAT_INVERT_CAPTURE|PAT_TERM, '/', '/'},
    {PAT_MANY|PAT_INVERT_CAPTURE|PAT_TERM, '*', '*'},
    {PAT_KO|PAT_KO_TERM, '*', '*'},
    {PAT_KO|PAT_KO_TERM, '/', '/'},
    patText,
    {PAT_END, 0, 0}
};

static status start(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl,
        funcDef, DOC_FUNC, DOC_START);
    r |= Roebling_SetPattern(rbl,
        funcDef, DOC_FUNC, DOC_START);
    r |= Roebling_SetPattern(rbl,
        funcMultiLineDef, DOC_FUNC_MULTILINE, DOC_START);
    r |= Roebling_SetPattern(rbl,
        includeLocalDef, DOC_INCLUDE_LOCAL, DOC_START);
    r |= Roebling_SetPattern(rbl,
        includePathDef, DOC_INCLUDE_PATH, DOC_START);
    r |= Roebling_SetPattern(rbl,
        blankLineDef, DOC_LINE, DOC_START);
    r |= Roebling_SetPattern(rbl,
        funcEndDef, DOC_FUNC_END, DOC_START);
    r |= Roebling_SetPattern(rbl,
        lineDef, DOC_LINE, DOC_START);
    r |= Roebling_SetPattern(rbl,
        commentDef, DOC_COMMENT, DOC_START);

    return r;
}

static status Capture(Roebling *rbl, word captureKey, StrVec *v){
    void *args[5];
    if(1){
        args[0] = Type_ToStr(OutStream->m, captureKey);
        args[1] = v,
        args[2] = NULL;
        Out("^c.Doc Capture ^E0.$^ec. -> ^0y.@^0.\n", args);
    }

    return SUCCESS;
}

Roebling *Doc_MakeRoebling(MemCh *m, Cursor *curs, void *source){
    DebugStack_Push(curs, curs->type.of);
    Roebling *rbl = Roebling_Make(m, curs, Capture, NULL); 
    Roebling_AddStep(rbl, I16_Wrapped(m, DOC_START));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)start));
    Roebling_AddStep(rbl, I16_Wrapped(m, DOC_END));
    Roebling_Start(rbl);

    void *args[] = {
        rbl,
        NULL
    };
    Out("^p.Rbl @^0\n", args);

    rbl->capture = Capture;
    rbl->source = source;
    rbl->dest = (Abstract *)Iter_Make(m, Span_Make(m));

    DebugStack_Pop();
    return rbl;
}
