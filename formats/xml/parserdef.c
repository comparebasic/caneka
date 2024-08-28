#include <external.h>
#include <caneka.h>

static word attStart[] = {
    WS_REQUIRED,
    PAT_IGNORE, 'a','z', PAT_IGNORE, 'A','Z',PAT_IGNORE|PAT_TERM, '_','_',
    PAT_END, 0, 0
};

static word selfClose[] = {
    WS_OPTIONAL,
    PAT_IGNORE|PAT_TERM, '/', '/', PAT_IGNORE|PAT_TERM, '>', '>',
    PAT_END, 0, 0
};

static word nl_upper[] = {PAT_TERM, '\n', '\n', PAT_TERM, 'A', 'Z', PAT_END, 0, 0}; 

static word open[] = {PAT_TERM, '<', '<', PAT_END, 0, 0}; 

static word tag[] = {PAT_INVERT|PAT_MANY, '/', '/', PAT_INVERT|PAT_MANY, ' ', ' ',\
    PAT_MANY, 'a', 'z', PAT_MANY, 'A', 'Z', PAT_MANY, '-', '-', PAT_MANY, '_', '_',\
    PAT_MANY, ':', ':', PAT_MANY|PAT_TERM, '0', '9',
    PAT_END, 0, 0
};

static word attrTag[] = {PAT_IGNORE|PAT_INVERT, '=', '=', PAT_MANY, 'a', 'z', \
    PAT_MANY|PAT_TERM, 'A', 'Z', PAT_IGNORE|PAT_INVERT, '=', '=', PAT_MANY, 'a', 'z', \
    PAT_MANY, 'A', 'Z', PAT_MANY, '-', '-', PAT_MANY, '_', '_', \
    PAT_MANY|PAT_TERM, '0', '9', PAT_END, 0, 0
};

static word closeDef[] = {
    PAT_TERM|PAT_IGNORE, '/','/', PAT_MANY, 'a', 'z', PAT_ANY|PAT_TERM, 'A', 'Z',
    PAT_ANY, 'a', 'z', PAT_ANY, 'A', 'Z', \
    PAT_ANY, '-', '-', PAT_ANY, '_', '_', PAT_ANY, ':', ':', PAT_ANY|PAT_TERM, '0', '9', \
    PAT_IGNORE|PAT_TERM, '>', '>',
    PAT_END, 0, 0
};

static word sep[] = { PAT_ANY, ' ', ' ', PAT_ANY, '\t', '\t', PAT_ANY, '\r', '\r', \
    PAT_ANY|PAT_TERM, '\n', '\n', PAT_END, 0, 0};

static word gt[] = { PAT_TERM, '>', '>',PAT_END, 0, 0};

static word quoted[] = {PAT_IGNORE|PAT_TERM, '"', '"', PAT_INVERT, '"', '"', \
    PAT_ANY|PAT_TERM, 32, 255, PAT_IGNORE|PAT_TERM, '"', '"', PAT_END, 0,0
};

static word unquoted[] = {PAT_INVERT, '"', '"', PAT_INVERT, ' ', ' ', \
    PAT_INVERT, '\t', '\t', PAT_INVERT, '\r', '\r', PAT_INVERT, '\n', '\n', \
    PAT_ANY|PAT_TERM, 32, 255, PAT_END, 0,0
};

static word body[] = {PAT_ANY, '\t', '\t', PAT_ANY, '\r', '\r', PAT_ANY, '\n', '\n', \
    PAT_ANY|PAT_TERM, 32, 255, PAT_END, 0,0
};

static word sepEq[] = { PAT_ANY, ' ', ' ', PAT_ANY, '\t', '\t', PAT_TERM, '=', '=', \
    PAT_END, 0, 0
};

/* setters */
static status setTag(Roebling *rbl){
    printf("setTag\n");
    /*
    XmlCtx *ctx = (XmlCtx *)as(source, TYPE_XMLCTX);
    Debug_Print((void *)s, 0, "setTag: ", COLOR_YELLOW, TRUE);
    printf("\n");
    */
    return SUCCESS;
}

static status setAttr(Roebling *rbl){
    printf("setAttr\n");
    /*
    XmlCtx *ctx = (XmlCtx *)as(source, TYPE_XMLCTX);
    String *s =  Range_Copy(ctx->m, range);
    XmlCtx_SetAttr(ctx, s);
    */
    return SUCCESS;
}

static status setAttrValue(Roebling *rbl){
    printf("setAttrValue\n");
    /*
    XmlCtx *ctx = (XmlCtx *)as(source, TYPE_XMLCTX);
    String *s =  Range_Copy(ctx->m, range);
    Debug_Print((void *)s, 0, "setAttrValue: ", COLOR_YELLOW, TRUE);
    printf("\n");
    */
    return SUCCESS;
}

static status setBody(Roebling *rbl){
    printf("setBody\n");
    /*
    XmlCtx *ctx = (XmlCtx *)as(source, TYPE_XMLCTX);
    String *s =  Range_Copy(ctx->m, range);
    Debug_Print((void *)s, 0, "setBody: ", COLOR_YELLOW, TRUE);
    printf("\n");
    */
    return SUCCESS;
}

static status tagNamed(Roebling *rbl){
    String *s = Range_Copy(rbl->m, &(rbl->range));
    XmlCtx *ctx = (XmlCtx *)as(rbl->source, TYPE_XMLCTX);
    return XmlCtx_Open(ctx, s);
}

static status tagOpened(Roebling *rbl){
    printf("tagOpened\n");
    /*
    XmlCtx *ctx = (XmlCtx *)as(source, TYPE_XMLCTX);
    String *s =  Range_Copy(ctx->m, range);
    Match *mt = Parser_GetMatch(prs);
    Debug_Print((void *)s, 0, "tagOpened: ", COLOR_YELLOW, TRUE);
    printf("\n");
    prs->jump = mt->jump;
    */
    return SUCCESS;
}

static status tagClose(Roebling *rbl){
    XmlCtx *ctx = (XmlCtx *)as(rbl->source, TYPE_XMLCTX);
    String *s = Range_Copy(rbl->m, &(rbl->range));
    return XmlCtx_Close(ctx, s);
}

/* routing parsers */
static status startParserMk(Roebling *rbl){
    Roebling_ResetPatterns(rbl);
    Match *mt = NULL;

    mt = Span_ReserveNext(rbl->matches.values);
    Match_SetPattern(mt, (PatCharDef *)open);
    mt->jump = Roebling_GetMarkIdx(rbl, XML_TAG); 

    return SUCCESS;
}

static status tagParserMk(Roebling *rbl){
    Roebling_ResetPatterns(rbl);

    Match *open_mt = Span_ReserveNext(rbl->matches.values);
    Match_SetPattern(open_mt, (PatCharDef *)tag);

    Match *close_mt = Span_ReserveNext(rbl->matches.values);
    Match_SetPattern(close_mt, (PatCharDef *)closeDef);
    close_mt->jump = Roebling_GetMarkIdx(rbl, XML_START); 

    rbl->dispatch = tagNamed;

    return SUCCESS;
}

static status attRoute(Roebling *rbl){
    Roebling_ResetPatterns(rbl);

    Match *toAtt = Span_ReserveNext(rbl->matches.values);
    Match_SetPattern(toAtt, (PatCharDef *)tag);
    toAtt->jump = Roebling_GetMarkIdx(rbl, XML_ATTRIBUTE);

    Match *self_mt = Span_ReserveNext(rbl->matches.values);
    Match_SetPattern(self_mt, (PatCharDef *)selfClose);
    self_mt->jump = Roebling_GetMarkIdx(rbl, XML_START);
    self_mt->dispatch = tagClose;

    return SUCCESS;
}

static status spaceParserMk(Roebling *rbl){
    Roebling_ResetPatterns(rbl);
    Match *mt = NULL;

    mt = Span_ReserveNext(rbl->matches.values);
    Match_SetPattern(mt, (PatCharDef *)sep);
    mt->jump = Roebling_GetMarkIdx(rbl, XML_ATTRIBUTE);

    return SUCCESS;
}

static status sepParserMk(Roebling *rbl){
    Roebling_ResetPatterns(rbl);
    Match *mt = NULL;

    mt = Span_ReserveNext(rbl->matches.values);
    Match_SetPattern(mt, (PatCharDef *)sep);

    return SUCCESS;
}

static status attrParserMk(Roebling *rbl){
    Roebling_ResetPatterns(rbl);
    Match *mt = NULL;

    mt = Span_ReserveNext(rbl->matches.values);
    Match_SetPattern(mt, (PatCharDef *)attrTag);
    rbl->dispatch = setAttr;

    return SUCCESS;
}

static status postTagNameParserMk(Roebling *rbl){
    Roebling_ResetPatterns(rbl);
    Match *mt = NULL;

    mt = Span_ReserveNext(rbl->matches.values);
    Match_SetPattern(mt, (PatCharDef *)sep);

    mt = Span_ReserveNext(rbl->matches.values);
    Match_SetPattern(mt, (PatCharDef *)selfClose);
    
    rbl->dispatch = tagOpened;
    return SUCCESS;
}

static status postAttrParserMk(Roebling *rbl){
    Roebling_ResetPatterns(rbl);
    Match *mt = NULL;

    mt = Span_ReserveNext(rbl->matches.values);
    Match_SetPattern(mt, (PatCharDef *)selfClose);

    mt = Span_ReserveNext(rbl->matches.values);
    Match_SetPattern(mt, (PatCharDef *)gt);

    rbl->dispatch = tagOpened;

    return SUCCESS;
}

static status attrValueParserMk(Roebling *rbl){
    Roebling_ResetPatterns(rbl);
    Match *mt = NULL;

    mt = Span_ReserveNext(rbl->matches.values);
    Match_SetPattern(mt, (PatCharDef *)quoted);
    mt->jump = Roebling_GetMarkIdx(rbl, XML_ATTRIBUTE);

    mt = Span_ReserveNext(rbl->matches.values);
    Match_SetPattern(mt, (PatCharDef *)unquoted);

    rbl->dispatch = setAttrValue;

    return SUCCESS;
}

static status bodyParserMk(Roebling *rbl){
    Roebling_ResetPatterns(rbl);
    Match *mt = NULL;

    mt = Span_ReserveNext(rbl->matches.values);
    Match_SetPattern(mt, (PatCharDef *)body);

    rbl->dispatch = setBody;

    return SUCCESS;
}

static status eqParserMk(Roebling *rbl){
    Roebling_ResetPatterns(rbl);
    Match *mt = NULL;

    mt = Span_ReserveNext(rbl->matches.values);
    Match_SetPattern(mt, (PatCharDef *)sepEq);

    return SUCCESS;
}

Span *XmlParser_Make(MemCtx *m){
    MemHandle *mh = (MemHandle *)m;
    Span *p =  Span_From(m, 14, 
        (Abstract *)Int_Wrapped(m, XML_START), 
            (Abstract *)Do_Wrapped(mh, (DoFunc)startParserMk),
        (Abstract *)Int_Wrapped(m, XML_TAG), 
            (Abstract *)Do_Wrapped(mh, (DoFunc)tagParserMk),
        (Abstract *)Int_Wrapped(m, XML_ATTROUTE), 
            (Abstract *)Do_Wrapped(mh, (DoFunc)attRoute),

        (Abstract *)Int_Wrapped(m, XML_ATTRIBUTE), 
            (Abstract *)Do_Wrapped(mh, (DoFunc)attrParserMk),
            (Abstract *)Do_Wrapped(mh, (DoFunc)eqParserMk),
            (Abstract *)Do_Wrapped(mh, (DoFunc)postAttrParserMk),
        (Abstract *)Int_Wrapped(m, XML_ATTR_VALUE), 
            (Abstract *)Do_Wrapped(mh, (DoFunc)attrValueParserMk),
            (Abstract *)Do_Wrapped(mh, (DoFunc)postAttrParserMk),
        (Abstract *)Int_Wrapped(m, XML_BODY), 
            (Abstract *)Do_Wrapped(mh, (DoFunc)bodyParserMk),
        (Abstract *)Int_Wrapped(m, XML_END));
    return p;
}
