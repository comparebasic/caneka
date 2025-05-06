#include <external.h>
#include <caneka.h>

static PatCharDef indentDef[] = {
    {PAT_ANY|PAT_INVERT_CAPTURE,' ' ,' '},{PAT_MANY|PAT_TERM, '=', '='},{PAT_ANY|PAT_INVERT_CAPTURE|PAT_TERM,' ' ,' '},
    {PAT_END, 0, 0}
};

static PatCharDef lineDef[] = {
    {PAT_KO, '\n', '\n'}, patText,
    {PAT_END, 0, 0}
};

static PatCharDef leadLineDef[] = {
    {PAT_KO, '\n', '\n'}, {PAT_TERM|PAT_INVERT_CAPTURE, ' ', ' '}, {PAT_KO, '\n', '\n'}, patText,
    {PAT_END, 0, 0}
};

static PatCharDef kvKeyDef[] = {
    {PAT_KO, '\n','\n'}, {PAT_KO,':', ':'}, patText,
    {PAT_ANY,' ',' '}, {PAT_ANY|PAT_TERM,'\r','\r'},{PAT_ANY|PAT_TERM,'\t','\t'},
    {PAT_INVERT|PAT_INVERT_CAPTURE|PAT_TERM,'\n', '\n'},
    {PAT_END, 0, 0}
};

static PatCharDef noLeadLineDef[] = {
    {PAT_ANY|PAT_INVERT_CAPTURE|PAT_TERM, ' ', ' '},
    {PAT_KO, '\n', '\n'}, patText,
    {PAT_END, 0, 0}
};

static PatCharDef nlDef[] = {
    {PAT_TERM, '\n', '\n'},
    {PAT_END, 0, 0}
};

static PatCharDef dashDef[] = {
    {PAT_ANY|PAT_INVERT_CAPTURE, ' ', ' '}, {PAT_TERM, '-', '-'},{PAT_ANY|PAT_INVERT_CAPTURE|PAT_TERM,' ' ,' '},
    {PAT_END, 0, 0}
};

static PatCharDef plusDef[] = {
    {PAT_ANY, ' ', ' '}, {PAT_TERM, '+', '+'},
    {PAT_END, 0, 0}
};

static PatCharDef cmdDef[] = {
    {PAT_TERM|PAT_INVERT_CAPTURE, '.', '.'},{PAT_KO, ':', ':'}, patText,
    {PAT_END, 0, 0}
};

static PatCharDef commaSepDef[] = {
    {PAT_KO, ',', ','}, patText,
    {PAT_END, 0, 0}
};

static PatCharDef lastValueDef[] = {
    {PAT_KO,'\n','\n'}, patText,
    {PAT_END, 0, 0}
};

static status start(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        nlDef, FORMATTER_NEXT, FORMATTER_START); r |= Roebling_SetPattern(rbl,
        indentDef, FORMATTER_INDENT, FORMATTER_LINE);
    r |= Roebling_SetPattern(rbl,
        dashDef, FORMATTER_ALIAS, FORMATTER_NOLEADLINE);
    r |= Roebling_SetPattern(rbl,
        plusDef, FORMATTER_ALIAS, FORMATTER_VALUE);
    r |= Roebling_SetPattern(rbl,
        cmdDef, FORMATTER_METHOD, FORMATTER_VALUE);
    r |= Roebling_SetPattern(rbl,
        kvKeyDef, FORMATTER_KV_KEY, FORMATTER_KV_VALUE);
    r |= Roebling_SetPattern(rbl,
        leadLineDef, FORMATTER_LEAD_LINE, FORMATTER_START);
    r |= Roebling_SetPattern(rbl,
        lineDef, FORMATTER_LINE, FORMATTER_START);

    return r;
}

static status line(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        lineDef, FORMATTER_LINE, FORMATTER_START);

    return r;
}

static status noLeadLine(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        dashDef, FORMATTER_ALIAS, FORMATTER_NOLEADLINE);
    r |= Roebling_SetPattern(rbl,
        noLeadLineDef, FORMATTER_LINE, FORMATTER_NOLEADLINE);
    r |= Roebling_SetPattern(rbl,
        nlDef, FORMATTER_NEXT, FORMATTER_START);

    return r;
}

static status value(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        nlDef, FORMATTER_NEXT, FORMATTER_START);
    r |= Roebling_SetPattern(rbl,
        commaSepDef, FORMATTER_VALUE, FORMATTER_VALUE);
    r |= Roebling_SetPattern(rbl,
        lastValueDef, FORMATTER_LAST_VALUE, FORMATTER_VALUE);

    return r;
}

static status kvValue(MemCtx *m, Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);

    r |= Roebling_SetPattern(rbl,
        lineDef, FORMATTER_KV_VALUE, FORMATTER_START);

    return r;
}

static status Capture(Roebling *rbl, word captureKey, StrVec *v){
    MessSet *mset = rbl->mset;
    if(rbl->type.state & DEBUG){
        Abstract *args[] = {
            (Abstract *)Type_ToStr(OutStream->m, captureKey);
            (Abstract *)v,
            (Abstract *)rbl,
            NULL
        }
        Debug("^p.Fmt Capture $/@ from @^0\n", args);
    }
    return SUCCESS;
}

static void addPatterns(FmtCtx *fmt){
    struct pat_config *cnf = pats;
    Lookup *lk = Lookup_Make(fmt->m, 0, NULL, (Abstract *)fmt);
    while(cnf->name != NULL){
        FmtDef *def = FmtDef_Make(fmt->m);
        def->name = String_Make(fmt->m, bytes(cnf->name));
        if(cnf->alias != NULL){
            def->alias = String_Make(fmt->m, bytes(cnf->alias));
        }
        def->id = lk->values->max_idx+1;
        def->type.state = cnf->flags;
        if(cnf->parent != 0){
            def->parent = Lookup_Get(lk, def->id+cnf->parent);
            if(def->parent != NULL){
                def->parent->child = def;
            }
        }
        Lookup_Add(fmt->m, lk, def->id, def);
        cnf++;
    }
    Fmt_Add(fmt->m, fmt, lk);
}

Roebling *FormatFmt_Make(MemCtx *m, Abstract *source){
    Roebling *rbl = Roebling_Make(m, curs, Capture, NULL); 
    Roebling_AddStep(rbl, (Abstract *)I16_Wrapped(m, RBL_TEST_START));
    Roebling_AddStep(rbl, (Abstract *)Do_Wrapped(m, (DoFunc)SetWord2));
    Roebling_AddStep(rbl, (Abstract *)I16_Wrapped(m, RBL_TEST_END));
    Roebling_Start(rbl);

    rbl->mess = Mess_Make(m);
    rbl->source = source;
    return rbl;
}
