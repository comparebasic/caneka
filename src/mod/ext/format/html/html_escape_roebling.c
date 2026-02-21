#include <external.h>
#include <caneka.h>

static PatCharDef amperDef[] = {
    {PAT_TERM, '&', '&'},
    {PAT_END, 0, 0}
};

static PatCharDef gtDef[] = {
    {PAT_TERM, '>', '>'},
    {PAT_END, 0, 0}
};

static PatCharDef ltDef[] = {
    {PAT_TERM, '<', '<'},
    {PAT_END, 0, 0}
};

static PatCharDef quoteDef[] = {
    {PAT_TERM, '"', '"'},
    {PAT_END, 0, 0}
};

static PatCharDef squoteDef[] = {
    {PAT_TERM, '\'', '\''},
    {PAT_END, 0, 0}
};

static PatCharDef contentDef[] = {
    {PAT_KO|PAT_KO_TERM|PAT_INVERT_CAPTURE, '<', '<'}, {PAT_KO|PAT_KO_TERM|PAT_INVERT_CAPTURE, '>', '>'},
    {PAT_KO|PAT_KO_TERM|PAT_INVERT_CAPTURE, '&', '&'}, {PAT_KO|PAT_KO_TERM|PAT_INVERT_CAPTURE, '"', '"'},
    {PAT_KO|PAT_KO_TERM|PAT_INVERT_CAPTURE, '\'', '\''},
    patText,
    {PAT_END, 0, 0}
};

static status start(MemCh *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        amperDef, HTML_ENT_AMPER, HTML_ENT_CONTENT);
    r |= Roebling_SetPattern(rbl,
        gtDef, HTML_ENT_GT, HTML_ENT_CONTENT);
    r |= Roebling_SetPattern(rbl,
        ltDef, HTML_ENT_LT, HTML_ENT_CONTENT);
    r |= Roebling_SetPattern(rbl,
        quoteDef, HTML_ENT_QUOTE, HTML_ENT_CONTENT);
    r |= Roebling_SetPattern(rbl,
        squoteDef, HTML_ENT_SQUOTE, HTML_ENT_CONTENT);
    r |= Roebling_SetPattern(rbl,
        contentDef, HTML_ENT_CONTENT, HTML_ENT_CONTENT);

    return r;
}

static status Capture(Roebling *rbl, word captureKey, StrVec *v){
    void *args[3];
    StrVec *content = as(rbl->dest, TYPE_STRVEC);
    if(rbl->curs->type.state & DEBUG){
        args[0] = Type_ToStr(OutStream->m, captureKey);
        args[1] = v;
        args[2] = NULL;
        Out("^y.Token: $/@^0\n", args);
    }
    if(captureKey == HTML_ENT_AMPER){
        StrVec_Add(content, S(rbl->m, "&amp;"));
    }else if(captureKey == HTML_ENT_GT){
        StrVec_Add(content, S(rbl->m, "&gt;"));
    }else if(captureKey == HTML_ENT_LT){
        StrVec_Add(content, S(rbl->m, "&gt;"));
    }else if(captureKey == HTML_ENT_QUOTE){
        StrVec_Add(content, S(rbl->m, "&quot;"));
    }else if(captureKey == HTML_ENT_SQUOTE){
        StrVec_Add(content, S(rbl->m, "&apos;"));
    }else{
        StrVec_AddVec(content, v);
    }
    return SUCCESS;
}

status HtmlEnt_IntoVec(MemCh *m, Roebling *rbl, StrVec *v){
    rbl->dest = (Abstract *)StrVec_Make(m);
    Roebling_Reset(m, rbl, v);
    Roebling_Run(rbl);
    Roebling_Finalize(rbl, NULL, NEGATIVE);
    return rbl->type.state;
}

Roebling *HtmlEntRbl_Make(MemCh *m, Cursor *curs, void *source){
    DebugStack_Push(curs, curs->type.of);
    Roebling *rbl = Roebling_Make(m, curs, Capture, NULL); 
    Roebling_AddStep(rbl, I16_Wrapped(m, HTML_ENT_CONTENT));
    Roebling_AddStep(rbl, Do_Wrapped(m, (DoFunc)start));
    Roebling_AddStep(rbl, I16_Wrapped(m, HTML_ENT_END));
    Roebling_Start(rbl);

    rbl->capture = Capture;
    rbl->source = source;
    DebugStack_Pop();
    return rbl;
}
