#include <external.h>
#include <caneka.h>

int XML_START = 1;
int XML_TAG = 2;
int XML_ATTRIBUTE = 3;
int XML_ATTR_VALUE = 4;
int XML_BODY = 5;

/* setters */
static status setTag(Parser *prs, Range *range, void *source){
    XmlCtx *ctx = (XmlCtx *)as(source, TYPE_XMLCTX);
    String *s =  Range_Copy(ctx->m, range);
    Debug_Print((void *)s, 0, "setTag: ", COLOR_YELLOW, TRUE);
    return SUCCESS;
}

static status setAttr(Parser *prs, Range *range, void *source){
    XmlCtx *ctx = (XmlCtx *)as(source, TYPE_XMLCTX);
    String *s =  Range_Copy(ctx->m, range);
    Debug_Print((void *)s, 0, "setAttr: ", COLOR_YELLOW, TRUE);
    return SUCCESS;
}

static status setAttrValue(Parser *prs, Range *range, void *source){
    XmlCtx *ctx = (XmlCtx *)as(source, TYPE_XMLCTX);
    String *s =  Range_Copy(ctx->m, range);
    Debug_Print((void *)s, 0, "setAttrValue: ", COLOR_YELLOW, TRUE);
    return SUCCESS;
}

static status setBody(Parser *prs, Range *range, void *source){
    XmlCtx *ctx = (XmlCtx *)as(source, TYPE_XMLCTX);
    String *s =  Range_Copy(ctx->m, range);
    Debug_Print((void *)s, 0, "setBody: ", COLOR_YELLOW, TRUE);
    return SUCCESS;
}

static status tagNamed(Parser *prs, Range *range, void *source){
    XmlCtx *ctx = (XmlCtx *)as(source, TYPE_XMLCTX);
    Match *mt = Parser_GetMatch(prs);
    prs->jump = mt->jump;
    String *s =  Range_Copy(ctx->m, range);
    Debug_Print((void *)s, 0, "tagNamed: ", COLOR_YELLOW, TRUE);

    XmlCtx_Open(ctx, s);
    return SUCCESS;
}

static status tagOpened(Parser *prs, Range *range, void *source){
    XmlCtx *ctx = (XmlCtx *)as(source, TYPE_XMLCTX);
    String *s =  Range_Copy(ctx->m, range);
    Match *mt = Parser_GetMatch(prs);
    Debug_Print((void *)s, 0, "tagOpened: ", COLOR_YELLOW, TRUE);
    XmlCtx_Close(ctx, s);
    prs->jump = mt->jump;
    return SUCCESS;
}

/* marks */
static Parser *xmlStartMarkMk(Roebling *rbl){
    return (Parser *)Mark_Make(rbl->m, XML_START);
}

static Parser *xmlTagMarkMk(Roebling *rbl){
    return (Parser *)Mark_Make(rbl->m, XML_TAG);
}

static Parser *xmlAttrMarkMk(Roebling *rbl){
    return (Parser *)Mark_Make(rbl->m, XML_ATTRIBUTE);
}

static Parser *xmlBodyMarkMk(Roebling *rbl){
    return (Parser *)Mark_Make(rbl->m, XML_BODY);
}

static Parser *xmlEndMarkMk(Roebling *rbl){
    return (Parser *)Mark_Make(rbl->m, XML_BODY);
}

/* routing parsers */
static Match *openMt(Roebling *rlb){
    word open[] = {PAT_TERM, '<', '<', PAT_END, 0, 0}; 
    Match *mt = Match_MakePat(rlb->m, bytes(open), 1, ANCHOR_START);
    mt->jump = Roebling_GetMarkIdx(rlb, XML_TAG); 
    return mt;
}

static Parser *startParserMk(Roebling *rlb){
    Match *mt = openMt(rlb);
    return Parser_MakeSingle(rlb->m, mt, NULL); 
}

static Match *spaceMt(Roebling *rlb){
    word sep[] = {
        PAT_ANY, ' ', ' ',
        PAT_ANY, '\t', '\t', PAT_ANY|PAT_TERM, '\r', '\r', PAT_ANY|PAT_TERM, '\n', '\n', PAT_END, 0, 0};
    return  Match_MakePat(rlb->m, bytes(sep), 4, ANCHOR_START); 
}

static Match *nameEndeMt(Roebling *rlb){
    word sep[] = {
        PAT_ANY, '/', '/', PAT_ANY, ' ', ' ',
        PAT_ANY, '\t', '\t', PAT_ANY|PAT_TERM, '\r', '\r', PAT_ANY|PAT_TERM, '\n', '\n', PAT_END, 0, 0};
    return  Match_MakePat(rlb->m, bytes(sep), 5, ANCHOR_START); 
}

static Match *selfCloseeMt(Roebling *rlb){
    word close[] = { PAT_TERM, '/', '/', PAT_ANY, ' ', ' ', PAT_ANY, '\t', '\t', PAT_ANY, '\r', '\r', PAT_ANY, '\n', '\n', PAT_TERM, '>', '>', PAT_END, 0, 0};
    Match *mt = Match_MakePat(rlb->m, bytes(close), 6, ANCHOR_START); 
    mt->jump = Roebling_GetMarkIdx(rlb, XML_START);
    return mt;
}

static Match *tagOpenedMt(Roebling *rlb){
    word close[] = { PAT_TERM, '>', '>',PAT_END, 0, 0};
    Match *mt = Match_MakePat(rlb->m, bytes(close), 1, ANCHOR_START); 
    mt->jump = Roebling_GetMarkIdx(rlb, XML_BODY);
    return mt;
}

static Parser *tagParserMk(Roebling *rlb){
    word tag[] = {PAT_INVERT|PAT_IGNORE, '/', '/', PAT_INVERT|PAT_IGNORE, ' ', ' ',PAT_MANY, 'a', 'z', PAT_MANY, 'A', 'Z', PAT_MANY|PAT_TERM, '0', '9',
        PAT_INVERT|PAT_IGNORE, '/', '/', PAT_INVERT|PAT_IGNORE, ' ', ' ', PAT_MANY, 'a', 'z', PAT_MANY, 'A', 'Z', PAT_MANY, '-', '-', PAT_MANY, '_', '_', PAT_MANY|PAT_TERM, '0', '9', PAT_END, 0, 0};
    Match *mt = Match_MakePat(rlb->m, bytes(tag), 12, ANCHOR_START);

    word close[] = {PAT_SINGLE|PAT_TERM, '/', '/', PAT_MANY, 'a', 'z', PAT_MANY|PAT_TERM, 'A', 'Z',
        PAT_MANY, 'a', 'z', PAT_MANY, 'A', 'Z', PAT_MANY, '-', '-', PAT_MANY, '_', '_', PAT_MANY|PAT_TERM, '0', '9', PAT_SINGLE|PAT_TERM, '>', '>', PAT_END, 0, 0};

    Match *close_mt = Match_MakePat(rlb->m, bytes(tag), 0, ANCHOR_START);
    close_mt->jump = Roebling_GetMarkIdx(rlb, XML_START); 
    Array mt_arr = Array_MakeFrom(rlb->m, 2, mt, close_mt);
    Parser *prs = Parser_MakeMulti(rlb->m, (Match **)mt_arr, tagNamed); 
    return prs;
}

static Parser *spaceParserMk(Roebling *rlb){
    Match *sp = spaceMt(rlb);
    sp->jump = Roebling_GetMarkIdx(rlb, XML_ATTRIBUTE);
    Parser *prs = Parser_MakeSingle(rlb->m, sp, NULL); 
    return prs;
}

static Parser *sepParserMk(Roebling *rlb){
    word sep[] = {
        PAT_ANY, ' ', ' ',
        PAT_ANY, '\t', '\t', PAT_ANY|PAT_TERM, '\r', '\r', PAT_END, 0, 0};
    return Parser_MakeSingle(rlb->m, Match_MakePat(rlb->m, bytes(sep), 3, ANCHOR_START), NULL); 
}

static Parser *attrParserMk(Roebling *rlb){
    word tag[] = {PAT_IGNORE|PAT_INVERT, '=', '=', PAT_MANY, 'a', 'z', PAT_MANY|PAT_TERM, 'A', 'Z',
        PAT_IGNORE|PAT_INVERT, '=', '=', PAT_MANY, 'a', 'z', PAT_MANY, 'A', 'Z', PAT_MANY, '-', '-', PAT_MANY, '_', '_', PAT_MANY|PAT_TERM, '0', '9'};
    Match *mt = Match_MakePat(rlb->m, bytes(tag), 9, ANCHOR_START);
    Parser *prs = Parser_MakeSingle(rlb->m, mt, setAttr); 
    prs->ko = spaceMt(rlb);
    return prs;
}

static Parser *postTagNameParserMk(Roebling *rlb){
    Array mt_arr = Array_MakeFrom(rlb->m, 2, selfCloseeMt(rlb), spaceMt(rlb));
    return Parser_MakeMulti(rlb->m, (Match **)mt_arr, tagOpened); 
}

static Parser *postAttrParserMk(Roebling *rlb){
    Array mt_arr = Array_MakeFrom(rlb->m, 2, selfCloseeMt(rlb), tagOpenedMt(rlb));
    return Parser_MakeMulti(rlb->m, (Match **)mt_arr, tagOpened); 
}

static Parser *attrValueParserMk(Roebling *rlb){
    word quoted[] = {PAT_IGNORE|PAT_TERM, '"', '"', PAT_INVERT, '"', '"', PAT_ANY|PAT_TERM, 32, 255, PAT_IGNORE|PAT_TERM, '"', '"', PAT_END, 0,0};
    word unquoted[] = {PAT_INVERT, '"', '"', PAT_INVERT, ' ', ' ', PAT_INVERT, '\t', '\t', PAT_INVERT, '\r', '\r', PAT_INVERT, '\n', '\n', PAT_ANY|PAT_TERM, 32, 255, PAT_END, 0,0};
    Match *mtq = Match_MakePat(rlb->m, bytes(quoted), 4, ANCHOR_START);
    Match *mtu = Match_MakePat(rlb->m, bytes(unquoted), 9, ANCHOR_START);
    mtq->jump = mtu->jump = Roebling_GetMarkIdx(rlb, XML_ATTRIBUTE);

    Array mt_arr = Array_MakeFrom(rlb->m, 2, mtq, mtu);
    Parser *prs = Parser_MakeMulti(rlb->m, (Match **)mt_arr, setAttrValue);
    return prs;
}

static Parser *bodyParserMk(Roebling *rlb){
    word body[] = {PAT_ANY, '\t', '\t', PAT_ANY, '\r', '\r', PAT_ANY, '\n', '\n', PAT_ANY|PAT_TERM, 32, 255, PAT_END, 0,0};
    
    Match *mt = Match_MakePat(rlb->m, bytes(body), 4, ANCHOR_START);
    mt->jump = Roebling_GetMarkIdx(rlb, XML_ATTRIBUTE);

    Parser *prs = Parser_MakeSingle(rlb->m, mt, setBody);
    prs->ko = openMt(rlb);

    return prs;
}

static Parser *eqParserMk(Roebling *rlb){
    word sep[] = { PAT_ANY, ' ', ' ', PAT_ANY, '\t', '\t', PAT_TERM, '=', '=', PAT_END, 0, 0};
    return Parser_MakeSingle(rlb->m, Match_MakePat(rlb->m, bytes(sep), 3, ANCHOR_START), NULL); 
}

Span *XmlParser_Make(MemCtx *m, ProtoDef *def){
    Span *p =  Span_From(m, 6, 
        (Abstract *)xmlStartMarkMk, 
            (Abstract *)startParserMk,
        (Abstract *)xmlTagMarkMk, 
            (Abstract *)tagParserMk,
            (Abstract *)postTagNameParserMk,
        (Abstract *)xmlAttrMarkMk, 
            (Abstract *)attrParserMk,
            (Abstract *)eqParserMk,
            (Abstract *)postAttrParserMk,
            (Abstract *)attrValueParserMk,
            (Abstract *)postAttrParserMk,
        (Abstract *)xmlBodyMarkMk, 
            (Abstract *)bodyParserMk,
        (Abstract *)xmlEndMarkMk);
    return p;
}
