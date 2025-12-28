#include <external.h>
#include <caneka.h>

static PatCharDef retDef[] = {
    {PAT_SINGLE, 'a', 'z'},
    {PAT_SINGLE|PAT_TERM, 'A', 'Z'},
    {PAT_MANY, 'a', 'z'},
    {PAT_MANY, '0', '9'},
    {PAT_MANY|PAT_TERM, 'A', 'Z'},
    {PAT_MANY|PAT_TERM, ' ', ' '},
    {PAT_ANY|PAT_TERM, '*', '*'},
    {PAT_END, 0, 0}
};

static PatCharDef funcNameDef[] = {
    {PAT_KO|PAT_KO_TERM|PAT_INVERT_CAPTURE, '(', '('},
    {PAT_MANY, 'a', 'z'},
    {PAT_MANY, '0', '9'},
    {PAT_MANY, 'A', 'Z'},
    {PAT_MANY, '_', '_'},
    {PAT_END, 0, 0}
};

static PatCharDef funcDef[] = {
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

static status funcDecl(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl,
        funcDef, DOC_FUNC, DOC_START);
    r |= Roebling_SetPattern(rbl,
        funcMultiLineDef, DOC_FUNC_MULTILINE, DOC_START);
    return r;
}

static status funcName(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl,
        funcNameDef, DOC_FUNC_NAME, DOC_FUNC);
    return r;
}

static status start(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl,
        retDef, DOC_FUNC_RET, DOC_FUNC_NAME);
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
    MemCh *m = rbl->m;

    Iter *it = (Iter *)as(rbl->dest, TYPE_ITER);
    NodeObj *dobj = (NodeObj *)rbl->source;
    NodeObj *node = Iter_Get(it);
    NodeObj funcObj = NULL;
    NodeObj commentObj = NULL;
    if(node !== NULL){
        Str *type = Node_Att(node, K(m, "type"));
        if(type != NULL){
            if( 
        }
    }

    if(rbl->dest->type.state & DEBUG){
        args[0] = Type_ToStr(OutStream->m, captureKey);
        args[1] = v,
        args[2] = NULL;
        Out("^c.Doc Capture ^E0.$^ec. -> ^0y.@^0.\n", args);
    }

    if(captureKey == DOC_INCLUDE_PATH || captureKey == DOC_INCLUDE_LOCAL){
        Table *tbl = NodeObj_GetTblOfAtt(dobj, K(m, "include"));
        Table_Set(tbl, v, v);
    }else if(captureKey == DOC_RET){
        NodeObj *funcObj = Inst_Make(m, TYPE_NODEOBJ);
        NodeObj_SetAtt(funcObj, S(m, "ret"), v);
        Iter_Add(it, funcObj);
    }else if(captureKey == DOC_FUNC || captureKey == DOC_FUNC_MULTILINE){
        NodeObj *funcObj = Inst_Make(m, TYPE_NODEOBJ);
        Table *tbl = NodeObj_GetTblOfAtt(dobj, K(m, "func"));
        Table_Set(tbl, v, v);
    }

    return SUCCESS;
}

Roebling *Doc_MakeRoebling(MemCh *m, Cursor *curs, void *source){
    DebugStack_Push(curs, curs->type.of);
    Roebling *rbl = Roebling_Make(m, curs, Capture, NULL); 
    Roebling_AddStep(rbl, I16_Wrapped(m, DOC_START));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)start));
    Roebling_AddStep(rbl, I16_Wrapped(m, DOC_FUNC_NAME));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)funcName));
    Roebling_AddStep(rbl, I16_Wrapped(m, DOC_FUNC));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)funcDecl));
    Roebling_AddStep(rbl, I16_Wrapped(m, DOC_END));
    Roebling_Start(rbl);

    rbl->capture = Capture;
    rbl->source = source;
    rbl->dest = (Abstract *)Iter_Make(m, Span_Make(m));
    rbl->dest->type.state |= DEBUG;

    DebugStack_Pop();
    return rbl;
}
