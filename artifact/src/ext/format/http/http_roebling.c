#include <external.h>
#include <caneka.h>

static PatCharDef getDef[] = {
    {PAT_TERM,'G' ,'G'},
    {PAT_TERM,'E' ,'E'},
    {PAT_TERM,'T' ,'T'},
    {PAT_TERM|PAT_INVERT_CAPTURE,' ' ,' '},
    {PAT_END, 0, 0}
};

static PatCharDef postDef[] = {
    {PAT_TERM,'P' ,'P'},
    {PAT_TERM,'O' ,'O'},
    {PAT_TERM,'S' ,'S'},
    {PAT_TERM,'T' ,'T'},
    {PAT_TERM|PAT_INVERT_CAPTURE,' ' ,' '},
    {PAT_END, 0, 0}
};

static PatCharDef uriDef[] = {
    {PAT_KO|PAT_KO_TERM, ' ', ' '},
    patText,
    {PAT_END, 0, 0}
};

static PatCharDef versionDef[] = {
    {PAT_TERM,'H' ,'H'},
    {PAT_TERM,'T' ,'T'},
    {PAT_TERM,'T' ,'T'},
    {PAT_TERM,'P' ,'P'},
    {PAT_TERM,'/' ,'/'},
    {PAT_TERM,'1' ,'1'},
    {PAT_TERM,'.' ,'.'},
    {PAT_TERM,'1' ,'2'},
    {PAT_END, 0, 0}
};

static PatCharDef endlDef[] = {
    {PAT_TERM, '\r', '\r'},
    {PAT_TERM, '\n', '\n'},
    {PAT_END, 0, 0}
};

static status method(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        getDef, HTTP_METHOD_GET, HTTP_PATH);
    r |= Roebling_SetPattern(rbl,
        postDef, HTTP_METHOD_POST, HTTP_PATH);

    return r;
}

static status uri(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        uriDef, HTTP_PATH, HTTP_VERSION);

    return r;
}

static status version(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        versionDef, HTTP_PATH, HTTP_END);

    return r;
}

static status Capture(Roebling *rbl, word captureKey, StrVec *v){
    /*
    */
    return SUCCESS;
}

Roebling *HttpRbl_Make(MemCh *m, Cursor *curs, Abstract *source){
    DebugStack_Push(curs, curs->type.of);
    Roebling *rbl = Roebling_Make(m, curs, Capture, NULL); 
    Roebling_AddStep(rbl, (Abstract *)I16_Wrapped(m, HTTP_METHOD));
    Roebling_AddStep(rbl, (Abstract *)Do_Wrapped(m, (DoFunc)method));
    Roebling_AddStep(rbl, (Abstract *)I16_Wrapped(m, HTTP_PATH));
    Roebling_AddStep(rbl, (Abstract *)Do_Wrapped(m, (DoFunc)uri));
    Roebling_AddStep(rbl, (Abstract *)I16_Wrapped(m, HTTP_VERSION));
    Roebling_AddStep(rbl, (Abstract *)Do_Wrapped(m, (DoFunc)version));
    Roebling_AddStep(rbl, (Abstract *)I16_Wrapped(m, HTTP_END));
    Roebling_Start(rbl);

    rbl->capture = Capture;
    rbl->source = source;
    DebugStack_Pop();
    return rbl;
}
