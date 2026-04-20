#include <external.h>
#include <caneka.h>

static PatCharDef textDef[] = {
    {PAT_KO|PAT_KO_TERM|PAT_INVERT_CAPTURE, '$', '$'},
    patText,
    {PAT_END, 0, 0}
};

static PatCharDef varDef[] = {
    {PAT_TERM, '$', '$'},
    {PAT_TERM, '{', '{'},
    {PAT_END, 0, 0}
};

static PatCharDef keyDef[] = {
    {PAT_KO|PAT_INVERT_CAPTURE, '}', '}'},
    {PAT_KO|PAT_INVERT_CAPTURE, '.', '.'},
    {PAT_KO|PAT_INVERT_CAPTURE|PAT_KO_TERM, '?', '?'},
    patText,
    {PAT_END, 0, 0}
};

static PatCharDef closeDef[] = {
    {PAT_TERM, '}', '}'},
    {PAT_END, 0, 0}
};

static PatCharDef sepDef[] = {
    {PAT_TERM, '.', '.'},
    {PAT_END, 0, 0}
};

static PatCharDef ifDef[] = {
    {PAT_TERM, '?', '?'},
    {PAT_END, 0, 0}
};

static status Capture(Roebling *rbl, word captureKey, StrVec *v){
    MemCh *m = rbl->m;

    if(rbl->curs->type.state & DEBUG){
        void *args[7];
        args[0] = Type_ToStr(OutStream->m, captureKey);
        args[1] = v,
        args[2] = NULL;
        Out("^c.Cash Capture ^E0.$^ec. -> ^0y.@\n", args);
    }

    Iter *it = (Iter *)rbl->dest;
    if(captureKey == CASH_TEXT){
        Iter_Add(it, v);
    }else if(captureKey == CASH_KEY || captureKey == CASH_IF){
        if(captureKey == CASH_KEY && Equals(v, K(m, "/"))){
            Jump *end = Jump_Make(m, CASH_END);
            Iter_Add(it, end);
            Jump_FindSource(end, it);
        }else{
            Abstract *a = Iter_Get(it);
            if(a->type.of != TYPE_FETCHER){
                a = (Abstract *)Fetcher_Make(m);
                Iter_Add(it, a);
            }
            Fetcher *fch = (Fetcher *)a;


            if(captureKey == CASH_KEY){
                Span_Add(fch->val.targets, FetchTarget_MakeKey(m, Ifc(m, v, TYPE_STR)));
            }else if(captureKey == CASH_IF){
                Span_Add(fch->val.targets, Jump_Make(m, CASH_IF));
            }
        }
    }

    return ZERO;
}

static status text(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl, varDef, CASH_VAR, CASH_VAR);
    r |= Roebling_SetPattern(rbl, textDef, CASH_TEXT, CASH_TEXT);
    return r;
}

static status var(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl, closeDef, CASH_CLOSE, CASH_TEXT);
    r |= Roebling_SetPattern(rbl, keyDef, CASH_KEY, CASH_VAR);
    r |= Roebling_SetPattern(rbl, sepDef, CASH_SEP, CASH_VAR);
    r |= Roebling_SetPattern(rbl, ifDef, CASH_IF, CASH_VAR);
    return r;
}

Roebling *CashParser_Make(MemCh *m, Cursor *curs, cls instTypeOf){
    Debug_Push(m, curs);

    Roebling *rbl = Roebling_Make(m, curs, Capture, NULL); 
    Roebling_AddStep(rbl, I16_Wrapped(m, CASH_TEXT));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)text));
    Roebling_AddStep(rbl, I16_Wrapped(m, CASH_VAR));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)var));
    Roebling_AddStep(rbl, I16_Wrapped(m, CASH_END));
    Roebling_Start(rbl);

    rbl->capture = Capture;
    rbl->source = (Abstract *)Iter_Make(m, Span_Make(m));
    Iter_Add((Iter *)rbl->source, I16_Wrapped(m, instTypeOf));
    rbl->dest = (Abstract *)Iter_Make(m, Span_Make(m));

    Return(m, rbl);
}
