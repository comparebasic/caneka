#include <external.h>
#include <caneka.h>

#define TAG_ATTR_PAT PAT_MANY,':',':',PAT_MANY,'-','-',PAT_MANY,'_','_',PAT_MANY,'a','z',PAT_MANY|PAT_TERM,'A','Z',\
    PAT_MANY,':',':',PAT_MANY,'-','-',PAT_MANY,'_','_',PAT_MANY,'a','z',PAT_MANY,'0','9',PAT_MANY|PAT_TERM,'A','Z'

static word tagDef[] = {
    PAT_TERM|PAT_NO_CAPTURE, '<', '<',
    TAG_ATTR_PAT, 
    PAT_END, 0, 0
};

static word closeTagDef[] = {
    PAT_TERM|PAT_NO_CAPTURE, '<', '<',
    PAT_TERM|PAT_NO_CAPTURE, '/', '/',
    TAG_ATTR_PAT,
    PAT_TERM|PAT_NO_CAPTURE, '>', '>',
    PAT_END, 0, 0
};

static word bodyDef[] = {
    PAT_KO|PAT_NO_CAPTURE, '<', '<', patText,
    PAT_END, 0, 0
};

static word bodyCashDef[] = {
    PAT_KO|PAT_NO_CAPTURE, '<', '<', PAT_MANY, '$', '$', patText,
    PAT_END, 0, 0
};


static word bodyWsDef[] = {
    PAT_KO|PAT_NO_CAPTURE, '<', '<', patWhiteSpace, 
    PAT_END, 0, 0
};

static word attDef[] = {
    PAT_NO_CAPTURE|PAT_MANY, '\t', '\t', PAT_NO_CAPTURE|PAT_MANY, '\r', '\r', 
    PAT_NO_CAPTURE|PAT_MANY, '\n', '\n', PAT_NO_CAPTURE|PAT_MANY|PAT_TERM, ' ', ' ',
    PAT_KO|PAT_NO_CAPTURE, '=', '=',
    PAT_KO|PAT_NO_CAPTURE, '/', '/',
    PAT_KO|PAT_NO_CAPTURE, '>', '>',
    TAG_ATTR_PAT,
    PAT_END, 0, 0
};

static word tagEndDef[] = {
    PAT_TERM, '>', '>',
    PAT_END, 0, 0
};

static word selfCloseDef[] = {
    PAT_TERM, '/', '/',
    PAT_TERM, '>', '>',
    PAT_END, 0, 0
};

static word attQuotedDef[] = {
    PAT_NO_CAPTURE|PAT_TERM, '=', '=',
    PAT_NO_CAPTURE|PAT_TERM, '"', '"',
    PAT_KO|PAT_NO_CAPTURE|PAT_CONSUME, '"', '"', patText,
    PAT_END, 0, 0
};

static word attQuotedNumDef[] = {
    PAT_NO_CAPTURE|PAT_TERM, '=', '=',
    PAT_NO_CAPTURE|PAT_TERM, '"', '"',
    PAT_KO|PAT_NO_CAPTURE|PAT_CONSUME, '"', '"', PAT_MANY, '0', '9',
    PAT_END, 0, 0
};

static word attQuotedCashDef[] = {
    PAT_NO_CAPTURE|PAT_TERM, '=', '=',
    PAT_NO_CAPTURE|PAT_TERM, '"', '"',
    PAT_KO|PAT_NO_CAPTURE|PAT_CONSUME, '"', '"', PAT_MANY, '$', '$', patText,
    PAT_END, 0, 0
};

static word attUnQuotedDef[] = {
    PAT_NO_CAPTURE|PAT_TERM, '=', '=',
    TAG_ATTR_PAT,
    PAT_END, 0, 0
};

static status start(Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)tagDef, XML_CAPTURE_OPEN, XML_ATTROUTE);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)closeTagDef, XML_CAPTURE_CLOSE_TAG, XML_START);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)bodyWsDef, XML_CAPTURE_TAG_WHITESPACE_BODY, XML_START);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)bodyCashDef, XML_CAPTURE_BODY_CASH, XML_START);

    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)bodyDef, XML_CAPTURE_BODY, XML_START);
    return r; 
}

static status attRoute(Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)attDef, XML_CAPTURE_ATTRIBUTE, -1);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)selfCloseDef, XML_CAPTURE_TAG_SELFCLOSED, XML_START);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)tagEndDef, XML_CAPTURE_TAG_ENDED, XML_START);
    return r; 
}

static status postAttName(Roebling *rbl){
    status r = READY;
    Roebling_ResetPatterns(rbl);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)attQuotedDef, XML_CAPTURE_ATTR_VALUE, XML_ATTROUTE);
    r |= Roebling_SetPattern(rbl,
        (PatCharDef*)attUnQuotedDef, XML_CAPTURE_ATTR_VALUE, XML_ATTROUTE);
    return r; 
}

static status xmlParser_Capture(word captureKey, int matchIdx, String *s, Abstract *source){
    status r = READY;
    XmlCtx *ctx = (XmlCtx *)source;
    if(captureKey == XML_CAPTURE_OPEN){
        if(DEBUG_XML){
            Debug_Print(s, 0, "Open: ", DEBUG_XML, TRUE);
        }
        r = XmlCtx_Open(ctx, s);
    }else if(captureKey == XML_CAPTURE_TAG_ENDED){
        if(DEBUG_XML){
            Debug_Print(s, 0, "TagClosed: ", DEBUG_XML, TRUE);
        }
        r = XmlCtx_TagClosed(ctx);
    }else if(captureKey == XML_CAPTURE_CLOSE_TAG){
        if(DEBUG_XML){
            Debug_Print(s, 0, "Close: ", DEBUG_XML, TRUE);
        }
        r = XmlCtx_Close(ctx, s);
    }else if(captureKey == XML_CAPTURE_TAG_SELFCLOSED){
        if(DEBUG_XML){
            Debug_Print(s, 0, "SelfClose: ", DEBUG_XML, TRUE);
        }
        r = XmlCtx_Close(ctx, NULL);
    }else if(captureKey == XML_CAPTURE_TAG_WHITESPACE_BODY){
        if(DEBUG_XML){
            Debug_Print(s, 0, "WSBody: ", DEBUG_XML, TRUE);
            printf("\n");
        }
        s->type.state |= WHITESPACE;
        r = XmlCtx_BodyAppend(ctx, s);
    }else if(captureKey == XML_CAPTURE_BODY){
        if(DEBUG_XML){
            Debug_Print(s, 0, "Body: ", DEBUG_XML, TRUE);
            printf("\n");
        }
        r = XmlCtx_BodyAppend(ctx, s);
    }else if(captureKey == XML_CAPTURE_ATTRIBUTE){
        if(DEBUG_XML){
            Debug_Print(s, 0, "Att: ", DEBUG_XML, TRUE);
            printf("\n");
        }
        r = XmlCtx_SetAttr(ctx, s);
    }else if(captureKey == XML_CAPTURE_ATTR_VALUE){
        if(DEBUG_XML){
            Debug_Print(s, 0, "AttValue: ", DEBUG_XML, TRUE);
        }
        r = XmlCtx_SetAttrValue(ctx, (Abstract *)s);
    }

    return r;
}

status XmlParser_Parse(XmlParser *xml, String *s){
    if(s != NULL){
        Roebling_AddBytes(xml->rbl, s->bytes, s->length);
    }
    while(!HasFlag(xml->rbl->type.state, BREAK)
            && !HasFlag(xml->ctx->type.state, SUCCESS)){
        Roebling_RunCycle(xml->rbl);
    };

    xml->type.state = 
        (xml->rbl->type.state & (BREAK|ERROR)) | 
            (xml->ctx->type.state & (ERROR|SUCCESS));

    return xml->type.state;
}

XmlParser *XmlParser_Make(MemCtx *m){
    MemHandle *mh = (MemHandle *)m;
    XmlParser *xml = MemCtx_Alloc(m, sizeof(XmlParser));
    xml->ctx = XmlCtx_Make(mh);

    Span *parsers_do =  Span_From(m, 7, 
        (Abstract *)Int_Wrapped(m, XML_START), 
            (Abstract *)Do_Wrapped(mh, (DoFunc)start),
        (Abstract *)Int_Wrapped(m, XML_ATTROUTE), 
            (Abstract *)Do_Wrapped(mh, (DoFunc)attRoute),
        (Abstract *)Int_Wrapped(m, XML_ATTR_VALUE), 
            (Abstract *)Do_Wrapped(mh, (DoFunc)postAttName),
        (Abstract *)Int_Wrapped(m, XML_END));

    LookupConfig config[] = {
        {XML_START, (Abstract *)String_Make(m, bytes("XML_START"))},
        {XML_ATTROUTE, (Abstract *)String_Make(m, bytes("XML_ATTROUTE"))},
        {XML_ATTR_VALUE, (Abstract *)String_Make(m, bytes("XML_ATTR_VALUE"))},
        {XML_END, (Abstract *)String_Make(m, bytes("XML_END"))},
        {0, NULL},
    };

    Lookup *desc = Lookup_FromConfig(m, config, NULL);

    xml->rbl = Roebling_Make(m, TYPE_ROEBLING, parsers_do, desc, String_Init(m, STRING_EXTEND), xmlParser_Capture, (Abstract *)xml->ctx); 

    return xml;
}
