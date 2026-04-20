#include <external.h>
#include <caneka.h>

static PatCharDef textDef[] = {
    {PAT_KO|PAT_KO_TERM|PAT_INVERT_CAPTURE, '$', '$'},
    patText,
    {PAT_END, 0, 0}
};

static PatCharDef varDef[] = {
    {PAT_TERM|PAT_INVERT_CAPTURE, '$', '$'},
    {PAT_TERM|PAT_INVERT_CAPTURE, '{', '}'},
    {PAT_KO|PAT_KO_TERM|PAT_INVERT_CAPTURE, '}', '}'},
    patText,
    {PAT_END, 0, 0}
};

static status Capture(Roebling *rbl, word captureKey, StrVec *v){
    MemCh *m = rbl->m;

    if(rbl->type.state & DEBUG){
        void *args[7];
        args[0] = Type_ToStr(OutStream->m, captureKey);
        args[1] = v,
        args[2] = NULL;
        Out("^c.Cash Capture ^E0.$^ec. -> ^0y.@\n", args);
    }

    Iter *it = (Iter *)rbl->dest;
    if(captureKey == CASH_TEXT){
        Iter_Add(it, v);
    }else if(captureKey == CASH_VAR){
        Fetcher *fch = Fetcher_Make(m);
        Span_Add(fch->val.targets, FetchTarget_MakeKey(m, Ifc(m, v, TYPE_STR)));
        Iter_Add(it, fch);
    }

    return ZERO;
}

static status text(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl, varDef, CASH_VAR, CASH_TEXT);
    r |= Roebling_SetPattern(rbl, textDef, CASH_TEXT, CASH_TEXT);
    return r;
}

Roebling *CashParser_Make(MemCh *m, Cursor *curs, cls instTypeOf){
    Debug_Push(m, curs);

    Roebling *rbl = Roebling_Make(m, curs, Capture, NULL); 
    Roebling_AddStep(rbl, I16_Wrapped(m, CASH_TEXT));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)text));
    Roebling_AddStep(rbl, I16_Wrapped(m, CASH_END));
    Roebling_Start(rbl);

    rbl->capture = Capture;
    rbl->source = (Abstract *)Iter_Make(m, Span_Make(m));
    Iter_Add((Iter *)rbl->source, I16_Wrapped(m, instTypeOf));
    rbl->dest = (Abstract *)Iter_Make(m, Span_Make(m));

    Return(m, rbl);
}
