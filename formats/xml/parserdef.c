#include <external.h>
#include <caneka.h>

#define TAG_ATTR_PAT PAT_SINGLE,':',':',PAT_SINGLE,'-','-',PAT_SINGLE,'_','_',PAT_SINGLE,'a','z',PAT_SINGLE|PAT_TERM,'A','Z',\
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
    PAT_KO, '<', '<', patText,
    PAT_END, 0, 0
};

word attDef[] = {
    PAT_NO_CAPTURE|PAT_MANY, '\t', '\t', PAT_NO_CAPTURE|PAT_MANY, '\r', '\r', 
    PAT_NO_CAPTURE|PAT_MANY, '\n', '\n', PAT_NO_CAPTURE|PAT_MANY|PAT_TERM, ' ', ' ',
    PAT_KO, '=', '=',
    PAT_KO, '/', '/',
    PAT_KO, '>', '>',
    TAG_ATTR_PAT,
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
    PAT_KO|PAT_NO_CAPTURE, '"', '"', patText,
    PAT_NO_CAPTURE|PAT_TERM, '"', '"',
    PAT_END, 0, 0
};

word attUnQuotedDef[] = {
    PAT_NO_CAPTURE|PAT_TERM, '=', '=',
    TAG_ATTR_PAT,
    PAT_END, 0, 0
};


static status setOpen(Roebling *rbl){
    printf("Open\n"); 
    return SUCCESS;
}

static status setClose(Roebling *rbl){
    printf("Close\n"); 
    return SUCCESS;
}

static status setBody(Roebling *rbl){
    printf("Body\n"); 
    return SUCCESS;
}

static status setAtt(Roebling *rbl){
    printf("Body\n"); 
    return SUCCESS;
}

static status setAttValue(Roebling *rbl){
    printf("Body\n"); 
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
    mt->dispatch = setClose;  
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

    mt->jump = Roebling_GetMarkIdx(rbl, XML_START);
    mt = Span_ReserveNext(rbl->matches);
    Match_SetPattern(mt, (PatCharDef *)tagEndDef);
    mt->jump = Roebling_GetMarkIdx(rbl, XML_START);

    mt = Span_ReserveNext(rbl->matches);
    Match_SetPattern(mt, (PatCharDef *)selfCloseDef);
    mt->dispatch = setClose;
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
    Span *p =  Span_From(m, 14, 
        (Abstract *)Int_Wrapped(m, XML_START), 
            (Abstract *)Do_Wrapped(mh, (DoFunc)start),
        (Abstract *)Int_Wrapped(m, XML_ATTROUTE), 
            (Abstract *)Do_Wrapped(mh, (DoFunc)attRoute),
        (Abstract *)Int_Wrapped(m, XML_ATTR_VALUE), 
            (Abstract *)Do_Wrapped(mh, (DoFunc)postAttName),
        (Abstract *)Int_Wrapped(m, XML_END));
    return p;
}
