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

static PatCharDef tokenDef[] = {
    {PAT_MANY, 'a', 'z'},
    {PAT_MANY, 'Z', 'Z'},
    {PAT_MANY, '-', '-'},
    {PAT_MANY, '_', '_'},
    {PAT_MANY|PAT_TERM, '0', '9'},
    {PAT_ANY|PAT_TERM|PAT_INVERT_CAPTURE|PAT_CONSUME, ' ', ' '},
    {PAT_TERM|PAT_INVERT_CAPTURE, '{', '{'},
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
    {PAT_TERM|PAT_INVERT_CAPTURE, '-', '-'}, {PAT_ANY|PAT_INVERT_CAPTURE|PAT_TERM, ' ', ' '},
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
        tokenDef, CONFIG_TOKEN, CONFIG_START);
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
        tokenDef, CONFIG_TOKEN, CONFIG_START);
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

static Abstract *findRecentOf(Iter *_it, cls typeOf, Abstract *before, i32 *idx){
    Iter it;
    memcpy(&it, _it, sizeof(Iter));
    Abstract *prev = Iter_Get(&it);
    while(prev == NULL || (prev->type.of != typeOf && (Iter_Prev(&it) & END) == 0)){
        if(prev == before){
            break;
        }
        prev = Iter_Get(&it);
    }
    if(prev != NULL && prev->type.of == typeOf){
        *idx = it.idx;
        return prev;
    }
    return NULL;
}

static status removeObj(Iter *it, i32 idx){
    while(it->idx >= idx){
        if(it->idx == 0){
            it->type.state |= SUCCESS;
            break;
        }
        Iter_Remove(it);
        Iter_Prev(it);
    }
    return it->type.state;
}

static status Capture(Roebling *rbl, word captureKey, StrVec *v){
    Iter *it = (Iter *)as(rbl->dest, TYPE_ITER);
    Abstract *args[7];
    MemCh *m = rbl->m;

    Abstract *prev = Iter_Get(it);
    i32 zero = 0;
    i32 currentIdx = 0;
    NodeObj *current = (NodeObj *)findRecentOf(it,
        TYPE_OBJECT, NULL, &currentIdx);

    i32 dataIdx = 0;
    Iter *data = (Iter *)findRecentOf(it,
        TYPE_ITER, (Abstract *)current, &dataIdx);

    i32 singleIdx = 0;
    Single *single = (Single *)findRecentOf(it,
        TYPE_WRAPPED_PTR, (Abstract *)data, &singleIdx);

    StrVec *token = NULL;
    if(single != NULL){
        token = (StrVec *)single->val.ptr;
    }

    if(prev == NULL && it->p->nvalues == 0){
        NodeObj *obj = Object_Make(rbl->m, TYPE_NODEOBJ);
        Iter_Add(it, obj);
        current = obj;
    }

    if(it->type.state & DEBUG){
        args[0] = (Abstract *)Type_ToStr(OutStream->m, captureKey);
        args[1] = (Abstract *)v,
        args[2] = (Abstract *)NULL;
        args[3] = (Abstract *)data;
        args[4] = (Abstract *)NULL;
        args[5] = (Abstract *)it->p;
        args[6] = NULL;
        Out("^c.Config Capture ^E0.$^ec. -> ^0y.   @\n   @\n   @\n   @\n   ^b.&^0\n", args);
    }

    if(captureKey == CONFIG_LEAD){
        return rbl->type.state;
    }else if(captureKey == CONFIG_INDENT){
        if(data != NULL){
            removeObj(it, currentIdx+1);
            data = NULL;
        }
        NodeObj *obj = Object_Make(rbl->m, TYPE_NODEOBJ);
        if(token != NULL){
            Object_SetPropByIdx(obj, NODEOBJ_PROPIDX_NAME, (Abstract *)token);
            Object_ByPath(current, token, (Abstract *)obj, SPAN_OP_SET);  
            Iter_Remove(it);
            Iter_Prev(it);
        }
        Iter_Add(it, obj);
        return rbl->type.state;
    }else if(captureKey == CONFIG_KEY){
        if(Object_GetPropByIdx(current, NODEOBJ_PROPIDX_ATTS) == NULL){
            Table *tbl = Table_Make(m);
            Iter *itn = Iter_Make(m, tbl);
            Table_SetKey((Iter *)itn, (Abstract *)StrVec_Str(m, v));
            Object_SetPropByIdx(current, NODEOBJ_PROPIDX_ATTS, (Abstract *)tbl);
            Iter_Add(it, itn);
        }else if(data == NULL){
            Table *tbl = Table_Make(m);
            Iter *itn = Iter_Make(m, tbl);
            Table_SetKey((Iter *)itn, (Abstract *)StrVec_Str(m, v));
            Object_Add(current, (Abstract *)tbl);
            Iter_Add(it, itn);
        }else if(data->p->type.of == TYPE_TABLE){
            Table_SetKey(data, (Abstract *)StrVec_Str(m, v));
        }
    }else if(captureKey == CONFIG_OUTDENT){
        removeObj(it, currentIdx);
        return rbl->type.state;
    }else if(captureKey == CONFIG_NL){
        if(data != NULL && 
                (data->p->type.of != TYPE_TABLE || data->metrics.selected == -1)){
            removeObj(it, currentIdx+1);
            data = NULL;
        }
        return rbl->type.state;
    }else if(captureKey == CONFIG_ITEM){
        if(data == NULL){
            Span *p = Span_Make(m);
            Iter *itn = Iter_Make(m, p);
            Span_Add(p, (Abstract *)v);
            Object_Add(current, (Abstract *)p);
            Iter_Add(it, itn);
        }else if(data->p->type.of == TYPE_SPAN){
            Iter_Add(data, (Abstract *)v);
        }else if(data->p->type.of == TYPE_TABLE){
            Span *p = Span_Make(m);
            Iter *itn = Iter_Make(m, p);
            Span_Add(p, (Abstract *)v);
            Table_SetValue(data, (Abstract *)p);
            Iter_Add(it, itn);
        }
    }else if(captureKey == CONFIG_NUMBER){
        Single *n = I64_Wrapped(m, I64_FromStr(StrVec_Str(m, v)));
        if(data != NULL){
            if(data->p->type.of == TYPE_TABLE){
                Table_SetValue(data, (Abstract *)n);
            }else if(data->p->type.of == TYPE_SPAN){
                Iter_Add(data, (Abstract *)n); 
            }
        }
    }else if(captureKey == CONFIG_LINE || captureKey == CONFIG_TOKEN){
        if(data != NULL){
            if(data->p->type.of == TYPE_TABLE){
                Table_SetValue(data, (Abstract *)v);
            }else if(data->p->type.of == TYPE_SPAN){
                Iter_Add(data, (Abstract *)v);
            }
        }else{
            Iter_Add(it, (Abstract *)Ptr_Wrapped(rbl->m, v, captureKey));
        }
    }

    return SUCCESS;
}

NodeObj *FormatConfig_GetRoot(Roebling *rbl){
    return Span_Get(((Iter *)rbl->dest)->p, 0);
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
