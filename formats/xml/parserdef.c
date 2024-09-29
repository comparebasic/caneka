#include <external.h>
#include <caneka.h>

#define TAG_ATTR_PAT PAT_MANY,':',':',PAT_MANY,'-','-',PAT_MANY,'_','_',PAT_MANY,'a','z',PAT_MANY|PAT_TERM,'A','Z',\
    PAT_MANY,':',':',PAT_MANY,'-','-',PAT_MANY,'_','_',PAT_MANY,'a','z',PAT_MANY,'0','9',PAT_MANY|PAT_TERM,'A','Z'

word tagDef[] = {
    PAT_TERM|PAT_NO_CAPTURE, '<', '<',
    TAG_ATTR_PAT, 
    PAT_END, 0, 0
};

word closeTagDef[] = {
    PAT_TERM|PAT_NO_CAPTURE, '<', '<',
    PAT_TERM|PAT_NO_CAPTURE, '/', '/',
    TAG_ATTR_PAT,
    PAT_TERM|PAT_NO_CAPTURE, '>', '>',
    PAT_END, 0, 0
};

word bodyDef[] = {
    PAT_KO|PAT_NO_CAPTURE, '<', '<', patText,
    PAT_END, 0, 0
};

word bodyWsDef[] = {
    PAT_KO|PAT_NO_CAPTURE, '<', '<', patWhiteSpace, 
    PAT_END, 0, 0
};

word attDef[] = {
    PAT_NO_CAPTURE|PAT_MANY, '\t', '\t', PAT_NO_CAPTURE|PAT_MANY, '\r', '\r', 
    PAT_NO_CAPTURE|PAT_MANY, '\n', '\n', PAT_NO_CAPTURE|PAT_MANY|PAT_TERM, ' ', ' ',
    PAT_KO|PAT_NO_CAPTURE, '=', '=',
    PAT_KO|PAT_NO_CAPTURE, '/', '/',
    PAT_KO|PAT_NO_CAPTURE, '>', '>',
    TAG_ATTR_PAT,
    PAT_END, 0, 0
};

word tagEndDef[] = {
    PAT_TERM, '>', '>',
    PAT_END, 0, 0
};

word selfCloseDef[] = {
    PAT_TERM, '/', '/',
    PAT_TERM, '>', '>',
    PAT_END, 0, 0
};

word attQuotedDef[] = {
    PAT_NO_CAPTURE|PAT_TERM, '=', '=',
    PAT_NO_CAPTURE|PAT_TERM, '"', '"',
    PAT_KO|PAT_NO_CAPTURE|PAT_CONSUME, '"', '"', patText,
    PAT_END, 0, 0
};

word attUnQuotedDef[] = {
    PAT_NO_CAPTURE|PAT_TERM, '=', '=',
    TAG_ATTR_PAT,
    PAT_END, 0, 0
};

static status setOpen(Roebling *rbl){
    String *s = Range_Copy(rbl->m, &(rbl->range));
    if(DEBUG_XML){
        Debug_Print(s, 0, "Open: ", DEBUG_XML, TRUE);
    }
    XmlCtx *ctx = (XmlCtx *)rbl->source;
    XmlCtx_Open(ctx, s);
    return SUCCESS;
}

static status setTagClosed(Roebling *rbl){
    String *s = Range_Copy(rbl->m, &(rbl->range));
    if(DEBUG_XML){
        Debug_Print(s, 0, "TagClosed: ", DEBUG_XML, TRUE);
    }
    XmlCtx *ctx = (XmlCtx *)rbl->source;
    XmlCtx_TagClosed(ctx);
    return SUCCESS;
}

static status setClose(Roebling *rbl){
    String *s = Range_Copy(rbl->m, &(rbl->range));
    if(DEBUG_XML){
        Debug_Print(s, 0, "Close: ", DEBUG_XML, TRUE);
    }
    XmlCtx *ctx = (XmlCtx *)rbl->source;
    XmlCtx_Close(ctx, s);
    return SUCCESS;
}

static status setWhiteSpaceBody(Roebling *rbl){
    String *s = Range_Copy(rbl->m, &(rbl->range));
    if(DEBUG_XML){
        Debug_Print(s, 0, "WSBody: ", DEBUG_XML, TRUE);
        printf("\n");
    }
    XmlCtx *ctx = (XmlCtx *)rbl->source;
    s->type.state |= WHITESPACE;
    XmlCtx_BodyAppend(ctx, s);
    return SUCCESS;
}

static status setBody(Roebling *rbl){
    String *s = Range_Copy(rbl->m, &(rbl->range));
    if(DEBUG_XML){
        Debug_Print(s, 0, "Body: ", DEBUG_XML, TRUE);
        printf("\n");
    }
    XmlCtx *ctx = (XmlCtx *)rbl->source;
    XmlCtx_BodyAppend(ctx, s);
    return SUCCESS;
}

static status setAtt(Roebling *rbl){
    String *s = Range_Copy(rbl->m, &(rbl->range));
    if(DEBUG_XML){
        Debug_Print(s, 0, "Att: ", DEBUG_XML, TRUE);
        printf("\n");
    }
    XmlCtx *ctx = (XmlCtx *)rbl->source;
    XmlCtx_SetAttr(ctx, s);
    return SUCCESS;
}

static status setAttValue(Roebling *rbl){
    String *s = Range_Copy(rbl->m, &(rbl->range));
    if(DEBUG_XML){
        Debug_Print(s, 0, "AttValue: ", DEBUG_XML, TRUE);
    }
    XmlCtx *ctx = (XmlCtx *)rbl->source;
    XmlCtx_SetAttrValue(ctx, (Abstract *)s);
    return SUCCESS;
}

static status start(Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    Match *mt;

    mt = Span_ReserveNext(rbl->matches);
    Match_SetPattern(mt, (PatCharDef *)tagDef);
    mt->dispatch = setOpen;  
    mt->jump = Roebling_GetMarkIdx(rbl, XML_ATTROUTE);

    mt = Span_ReserveNext(rbl->matches);
    Match_SetPattern(mt, (PatCharDef *)closeTagDef);
    mt->jump = Roebling_GetMarkIdx(rbl, XML_START);

    mt = Span_ReserveNext(rbl->matches);
    Match_SetPattern(mt, (PatCharDef *)bodyWsDef);
    mt->dispatch = setWhiteSpaceBody;  
    mt->jump = Roebling_GetMarkIdx(rbl, XML_START);

    mt = Span_ReserveNext(rbl->matches);
    Match_SetPattern(mt, (PatCharDef *)bodyDef);
    mt->dispatch = setBody;  
    mt->jump = Roebling_GetMarkIdx(rbl, XML_START);
    
    r |= SUCCESS;
    return r; 
}

static status attRoute(Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    Match *mt;

    mt = Span_ReserveNext(rbl->matches);
    Match_SetPattern(mt, (PatCharDef *)attDef);
    mt->dispatch = setAtt;

    mt = Span_ReserveNext(rbl->matches);
    Match_SetPattern(mt, (PatCharDef *)selfCloseDef);
    mt->dispatch = setClose;
    mt->jump = Roebling_GetMarkIdx(rbl, XML_START);

    mt->jump = Roebling_GetMarkIdx(rbl, XML_START);
    mt = Span_ReserveNext(rbl->matches);
    Match_SetPattern(mt, (PatCharDef *)tagEndDef);
    mt->dispatch = setTagClosed;
    mt->jump = Roebling_GetMarkIdx(rbl, XML_START);

    r |= SUCCESS;
    return r; 
}

static status postAttName(Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    Match *mt;

    mt = Span_ReserveNext(rbl->matches);
    Match_SetPattern(mt, (PatCharDef *)attQuotedDef);
    mt->dispatch = setAttValue;
    mt->jump = Roebling_GetMarkIdx(rbl, XML_ATTROUTE);
    mt = Span_ReserveNext(rbl->matches);

    Match_SetPattern(mt, (PatCharDef *)attUnQuotedDef);
    mt->dispatch = setAttValue;
    mt->jump = Roebling_GetMarkIdx(rbl, XML_ATTROUTE);

    r |= SUCCESS;
    return r; 
}

Span *XmlParser_Make(MemCtx *m){
    MemHandle *mh = (MemHandle *)m;
    Span *p =  Span_From(m, 7, 
        (Abstract *)Int_Wrapped(m, XML_START), 
            (Abstract *)Do_Wrapped(mh, (DoFunc)start),
        (Abstract *)Int_Wrapped(m, XML_ATTROUTE), 
            (Abstract *)Do_Wrapped(mh, (DoFunc)attRoute),
        (Abstract *)Int_Wrapped(m, XML_ATTR_VALUE), 
            (Abstract *)Do_Wrapped(mh, (DoFunc)postAttName),
        (Abstract *)Int_Wrapped(m, XML_END));
    return p;
}
