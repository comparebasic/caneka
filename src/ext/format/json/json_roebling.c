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

static status keyValue(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl,
        leadDef, JSON_LEAD, JSON_KEY_VALUE);
    r |= Roebling_SetPattern(rbl,
        indentDef, JSON_INDENT, JSON_START);
    r |= Roebling_SetPattern(rbl,
        indentArrDef, JSON_ARR_INDENT, JSON_ARR);
    r |= Roebling_SetPattern(rbl,
        quotedStringDef, JSON_KEY_VALUE, JSON_KEY_VALUE_SEP);

    return r;
}

static status keySep(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl,
        leadDef, JSON_LEAD, JSON_KEY_SEP);
    r |= Roebling_SetPattern(rbl,
        keySepDef, JSON_KEY_SEP, JSON_KEY_VALUE);

    return r;
}

static status key(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl,
        leadDef, JSON_LEAD, JSON_KEY);
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

static status kvSep(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl,
        leadDef, JSON_LEAD, JSON_KEY_VALUE);
    r |= Roebling_SetPattern(rbl,
        commaSepDef, JSON_COMMA_SEP, JSON_KEY);
    r |= Roebling_SetPattern(rbl,
        outdentDef, JSON_OUTDENT, JSON_START);

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
    MemCh *m = rbl->m;

    Iter *seelIt = (Iter *)as(rbl->source, TYPE_ITER);
    Single *seelSg = Iter_Get(seelIt);
    cls instTypeOf = seelSg->val.w;

    Iter *it = (Iter *)as(rbl->dest, TYPE_ITER);
    Iter *nodeIt = Iter_Get(it);
    Span *p = NULL;
    if(nodeIt != NULL){
        p = nodeIt->p;
    }

    if(it->type.state & DEBUG){
        void *args[7];
        args[0] = Type_ToStr(OutStream->m, captureKey);
        args[1] = v,
        args[2] = it->p;
        args[3] = p;
        args[4] = NULL;
        Out("^c.Json Capture ^E0.$^ec. -> ^0y.@\n    it: @\n    @\n", args);
    }

    if(captureKey == JSON_INDENT){
        Span *node = NULL;
        if(instTypeOf == TYPE_SPAN){
            node = Span_Make(m);
        }else if(instTypeOf == TYPE_TABLE){
            node = Table_Make(m);
        }else if(instTypeOf & TYPE_INSTANCE){
            node = (Span*)Inst_Make(m, instTypeOf);
        }else{
            Error(m, FUNCNAME, FILENAME, LINENUMBER, 
                "Unknown type to indent for JSON", NULL);
            rbl->type.state |= ERROR;
            return rbl->type.state;
        }
        Iter_Add(it, Iter_Make(m, (Span *)node));
        p = node;
    }

    if(p == NULL){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error unable to find current element to add to", NULL);
        return ERROR;
    }

    if(captureKey == JSON_KEY){
        if(instTypeOf == TYPE_TABLE){
            Table_SetKey(nodeIt, v);
        }else if(instTypeOf & TYPE_INSTANCE){
            Table *seel = Lookup_Get(SeelLookup, nodeIt->p->type.of);
            nodeIt->metrics.selected = Seel_GetIdx(seel, v);
        }else{
            Error(m, FUNCNAME, FILENAME, LINENUMBER, 
                "Unknown type to indent for JSON", NULL);
            rbl->type.state |= ERROR;
            return rbl->type.state;
        }
    }else if(captureKey == JSON_VALUE){
        Span_Add(p, v);
    }else if(captureKey == JSON_KEY_VALUE){
        if(instTypeOf == TYPE_TABLE){
            Table_SetValue(nodeIt, v);
        }else if(instTypeOf & TYPE_INSTANCE){
            i32 idx = nodeIt->metrics.selected;
            Iter_SetByIdx(nodeIt, idx, v);
            nodeIt->metrics.selected = -1;
        }else{
            Error(m, FUNCNAME, FILENAME, LINENUMBER, 
                "Unknown type to indent for JSON", NULL);
            rbl->type.state |= ERROR;
            return rbl->type.state;
        }
    }else if(captureKey == JSON_OUTDENT){
        if(it->idx > 0){
            Iter_Remove(it);
            Iter_Prev(it);
        }
    }

    return ZERO;
}

void *JsonParser_GetRoot(Roebling *rbl){
    Iter *nodeIt = Iter_GetByIdx((Iter *)rbl->dest, 0);
    if(nodeIt != NULL){
        return nodeIt->p;
    }
    return NULL;
}

Roebling *JsonParser_Make(MemCh *m, Cursor *curs, cls instTypeOf){
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
    Roebling_AddStep(rbl, I16_Wrapped(m, JSON_KEY_VALUE));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)keyValue));
    Roebling_AddStep(rbl, I16_Wrapped(m, JSON_KEY_VALUE_SEP));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)kvSep));
    Roebling_AddStep(rbl, I16_Wrapped(m, JSON_COMMA_SEP));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)arrSep));
    Roebling_AddStep(rbl, I16_Wrapped(m, JSON_END));
    Roebling_Start(rbl);

    rbl->capture = Capture;
    rbl->source = (Abstract *)Iter_Make(m, Span_Make(m));
    Iter_Add((Iter *)rbl->source, I16_Wrapped(m, instTypeOf));
    rbl->dest = (Abstract *)Iter_Make(m, Span_Make(m));

    DebugStack_Pop();
    return rbl;
}
