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
    Abstract *args[4];
    MemCh *m = rbl->m;

    if(it->type.state & DEBUG){
        args[0] = (Abstract *)Type_ToStr(OutStream->m, captureKey);
        args[1] = (Abstract *)v;
        args[2] = (Abstract *)it->p;
        args[3] = NULL;
        Out("^c.Config Capture ^E0.$^ec. -> @ ^y.\\@@\n", args);
    }

    Abstract *prev = Iter_Get(it);
    if(prev == NULL && it->p->nvalues == 0){
        NodeObj *obj = Object_Make(rbl->m, TYPE_NODEOBJ);
        Iter_Add(it, obj);
        prev =(Abstract *)obj; 
    }

    Single *token = NULL;
    StrVec *value = NULL;
    i32 idx = it->idx;
    if(prev != NULL && prev->type.of == TYPE_WRAPPED_PTR){
        token = (Single *)prev;
    }

    if(token != NULL && token->type.of == TYPE_WRAPPED_PTR){
        value = (StrVec *)token->val.ptr;
    }

    Iter_GetByIdx(it, idx);

    NodeObj *current = (NodeObj *)prev;
    while(current->type.of != TYPE_OBJECT && (Iter_Prev(it) & END) == 0){
        current = Iter_Get(it);
    }
    Iter_GetByIdx(it, idx);

    if(captureKey == CONFIG_LEAD){
        return rbl->type.state;
    }else if(captureKey == CONFIG_INDENT){
        if(Object_GetPropByIdx(current, NODEOBJ_PROPIDX_ATTS) != NULL){
            current->objType.state |= NODEOBJ_ATTS;
        }
        NodeObj *obj = Object_Make(rbl->m, TYPE_NODEOBJ);
        if(value != NULL){
            Object_SetPropByIdx(obj, NODEOBJ_PROPIDX_NAME, (Abstract *)value);

            args[0] = (Abstract *)value;
            args[1] = NULL;
            Out("^p.add sub obj @\n", args);

            Object_ByPath(current, value, (Abstract *)obj, SPAN_OP_SET);  
        }
        Iter_Add(it, obj);
        return rbl->type.state;
    }else if(captureKey == CONFIG_KEY){
        if(current->objType.state & NODEOBJ_ATTS){ 
            if(prev->type.of == TYPE_ITER && ((Iter *)prev)->p->type.of == TYPE_TABLE){
                Table_SetKey((Iter *)prev, (Abstract *)StrVec_Str(m, v));
            }else{
                Table *tbl = Table_Make(m);
                Iter *it = Iter_Make(m, tbl);
                Table_SetKey((Iter *)it, (Abstract *)StrVec_Str(m, v));
                Object_Add(current, (Abstract *)tbl);
            }
        }
    }else if(captureKey == CONFIG_OUTDENT){
        if(Object_GetPropByIdx(current, NODEOBJ_PROPIDX_ATTS) != NULL){
            current->objType.state |= NODEOBJ_ATTS;
        }
        return rbl->type.state;
    }else if(captureKey == CONFIG_NL){
        if(Object_GetPropByIdx(current, NODEOBJ_PROPIDX_ATTS) != NULL){
            current->objType.state |= NODEOBJ_ATTS;
        }
        StrVec *body = (StrVec *)Object_GetPropByIdx(current, NODEOBJ_PROPIDX_VALUE);
        if(body != NULL){
            Str *s = Span_Get(body->p, body->p->max_idx);
            if(s != NULL){
                s->type.state |= LAST;
            }
        }
        return rbl->type.state;
    }else if(captureKey == CONFIG_NUMBER){
        if((current->objType.state & NODEOBJ_ATTS) == 0 &&
                value != NULL && token != NULL && token->objType.of == CONFIG_KEY){
            Table *atts = (Table *)Object_GetPropByIdx(current, NODEOBJ_PROPIDX_ATTS);
            if(atts == NULL){
                atts = Table_Make(m);
                Object_SetPropByIdx(current, NODEOBJ_PROPIDX_ATTS, (Abstract *)atts);
            }
            Single *nsg = I64_Wrapped(m, I64_FromStr(StrVec_Str(m, v)));
            Table_Set(atts, (Abstract *)value, (Abstract *)nsg);
            return rbl->type.state;
        }
    }else if(captureKey == CONFIG_LINE){
        if((current->objType.state & NODEOBJ_ATTS) == 0 &&
                value != NULL && token != NULL && token->objType.of == CONFIG_KEY){
            Table *atts = (Table *)Object_GetPropByIdx(current, NODEOBJ_PROPIDX_ATTS);
            if(atts == NULL){
                atts = Table_Make(m);
                Object_SetPropByIdx(current, NODEOBJ_PROPIDX_ATTS, (Abstract *)atts);
            }
            Table_Set(atts, (Abstract *)value, (Abstract *)v);
            return rbl->type.state;
        }else if(prev != NULL && prev->type.of != TYPE_OBJECT ){
            if(prev->type.of == TYPE_ITER && ((Iter *)prev)->p->type.of == TYPE_TABLE){
                Table_SetValue((Iter *)prev, (Abstract *)v);
                return rbl->type.state;
            }
            
            StrVec *body = (StrVec *)Object_GetPropByIdx(current, NODEOBJ_PROPIDX_VALUE);
            if(body != NULL){
                Str *s = Span_Get(body->p, body->p->max_idx);
                if(s != NULL && s->type.state & MORE){
                    StrVec_Add(body, Str_FromCstr(m, "\n\n", STRING_COPY));
                }else if(body->p->nvalues > 0){
                    StrVec_Add(body, Str_FromCstr(m, " ", STRING_COPY));
                }
            }else{
                body = StrVec_Make(m);
                Object_SetPropByIdx(current, NODEOBJ_PROPIDX_VALUE, (Abstract *)body);
            }
            StrVec_AddVec(body, v);
            return rbl->type.state;
        }
    }

    Iter_Add(it, (Abstract *)Ptr_Wrapped(rbl->m, v, captureKey));

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
