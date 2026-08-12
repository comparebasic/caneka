#include <external.h>
#include <caneka.h>

static PatCharDef lineDef[] = {
    {PAT_MANY|PAT_KO|PAT_INVERT, '\\', '\\'},
    {PAT_MANY|PAT_KO, '\n', '\n'}, 
    patText,
    {PAT_END, 0, 0}
};

static status start(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        lineDef, LINE_CONTENT, LINE_CONTENT);
    return r;
}

static status Capture(Roebling *rbl, word captureKey, StrVec *v){
    MemCh *m = rbl->m;
    Iter *it = (Iter *)rbl->dest;
    Iter_Push(it, v);
    return ZERO;
}

Roebling *NlParser_Make(MemCh *m, Iter *it, Cursor *curs){
    Debug_Push(m, curs);

    Roebling *rbl = Roebling_Make(m, curs, Capture, NULL); 
    Roebling_AddStep(rbl, I16_Wrapped(m, LINE_CONTENT));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)start));
    Roebling_Start(rbl);

    rbl->capture = Capture;
    rbl->dest = (Abstract *)it;

    Return(m, rbl);
}
