#include <external.h>
#include <caneka.h>


static PatCharDef leadDef[] = {
    {PAT_MANY, '\t', '\t'},
    {PAT_MANY, '\r', '\r'},
    {PAT_MANY, '\n', '\n'},
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
    r |= Roebling_SetPattern(rbl,
        commaSepDef, JSON_COMMA_SEP, JSON_KEY);
    r |= Roebling_SetPattern(rbl,
        outdentDef, JSON_OUTDENT, JSON_START);
    r |= Roebling_SetPattern(rbl,
        outdentArrDef, JSON_ARR_OUTDENT, JSON_START);

    return r;
}

static status keyValue(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl,
        leadDef, JSON_LEAD, JSON_KEY_VALUE);
    r |= Roebling_SetPattern(rbl,
        indentDef, JSON_INDENT, JSON_KEY);
    r |= Roebling_SetPattern(rbl,
        indentArrDef, JSON_ARR_INDENT, JSON_ARR);
    r |= Roebling_SetPattern(rbl,
        quotedStringDef, JSON_KEY_VALUE, JSON_KEY_VALUE_SEP);
    r |= Roebling_SetPattern(rbl,
        numberDef, JSON_NUMBER, JSON_KEY_VALUE_SEP);
    r |= Roebling_SetPattern(rbl,
        outdentDef, JSON_OUTDENT, JSON_START);

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
    r |= Roebling_SetPattern(rbl,
        indentDef, JSON_INDENT, JSON_KEY);
    r |= Roebling_SetPattern(rbl,
        indentArrDef, JSON_ARR_INDENT, JSON_ARR);

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

    Iter *it = (Iter *)Ifc(rbl->m, rbl->source, TYPE_ITER);
    void *value = NULL;

    if(rbl->dest->type.state & DEBUG){
        void *args[7];
        args[0] = Type_ToStr(OutStream->m, captureKey);
        args[1] = v,
        args[2] = NULL;
        Out("^y.Json Capture ^E0.$^ec. -> ^0y.@^0\n", args);
    }

    Iter *prevIt = Iter_Latest(it, TYPE_ITER);
    Node *node = Iter_Latest(it, TYPE_NODE);

    if(captureKey == JSON_INDENT){
        rbl->nest++;
        Node *n = NULL;
        if(it->p->nvalues == 0){
            n = (Inst *)rbl->dest;
        }else{
            n = Inst_Make(m, TYPE_NODE);
            value = n;
        }

        Iter *oit = Iter_Make(m, Span_Get(n, INST_PROPIDX_CHILDREN));

        Iter_Add(it, n);
        Iter_Add(it, oit);
    }else if(captureKey == JSON_ARR_INDENT){
        rbl->nest++;
        Inst *n = Inst_Make(m, TYPE_NODE);
        value = n;
        Iter *oit = Iter_Make(m, Span_Get(n, INST_PROPIDX_CHILDREN));

        Iter_Add(it, n);
        Iter_Add(it, oit);
    }

    if(captureKey == JSON_OUTDENT){
        if(prevIt == NULL || prevIt->p->type.of != TYPE_TABLE){
            void *ar[] = {
                prevIt != NULL ? prevIt->p : NULL,
                NULL
            };
            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                "Closing } does not have corresponding open {", ar);
                rbl->type.state |= ERROR;
            
            return rbl->type.state;
        }
        rbl->nest--;
        Iter_Remove(it);
        Iter_Prev(it);
        Iter_Remove(it);
        Iter_Prev(it);
    }else if(captureKey == JSON_ARR_OUTDENT){
        if(prevIt == NULL || prevIt->p->type.of != TYPE_SPAN){
            void *ar[] = {
                prevIt != NULL ? prevIt->p : NULL,
                NULL
            };
            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                "Closing ] does not have corresponding open [", ar);
                rbl->type.state |= ERROR;
            
            return rbl->type.state;
        }
        rbl->nest--;
        Iter_Remove(it);
        Iter_Prev(it);
        Iter_Remove(it);
        Iter_Prev(it);
    }else if(captureKey == JSON_KEY){
        Table_SetKey(prevIt, v);
    }else if(captureKey == JSON_VALUE){
        value = v;
    }else if(captureKey == JSON_NUMBER){
        i64 n = I64_FromStr(Ifc(m, v, TYPE_STR));
        value = I64_Wrapped(m, n);
    }else if(captureKey == JSON_KEY_VALUE){
        value = v;
    }

    if(value != NULL){
        if(prevIt != NULL){
            if(prevIt->metrics.selected != -1){
                Table_SetValue(prevIt, value);
            }else{
                Iter_Add(prevIt, value);
            }
        }
    }

    return ZERO;
}

void *JsonParser_GetRoot(Roebling *rbl){
    return rbl->dest;
}

Roebling *JsonParser_Make(MemCh *m, Cursor *curs){
    Debug_Push(m, curs);

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
    Node *n = Inst_Make(m, TYPE_NODE);
    rbl->dest = (Abstract *)n;

    Return(m, rbl);
}
