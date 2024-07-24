#include <external.h>
#include <caneka.h>

int XML_START = 1;
int XML_TAG = 2;
int XML_ATTRIBUTE = 3;
int XML_ATTR_VALUE = 4;
int XML_BODY = 5;

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

/* routing parsers */
static Match *openMt(Roebling *rlb){
    word open[] = {PAT_TERM, '<', '<', PAT_END, 0, 0}; 
    Match *mt Match_MakePat(rlb->m, bytes(open), 1, ANCHOR_START);
    mt->jump = Roebling_GetMarkIdx(rlb, XML_TAG); 
    return mt;
}

static Parser *startParserMk(Roebling *rlb){
    Match *mt = openMt(rlb);
    return Parser_MakeSingle(rlb->m, mt, NULL); 
}

static Parser *tagParserMk(Roebling *rlb){
    word tag[] = {PAT_INVERT|PAT_IGNORE, ' ', ' ', PAT_MANY, 'a', 'z', PAT_MANY|PAT_TERM, 'A', 'Z',
        PAT_INVERT|PAT_IGNORE, ' ', ' ', PAT_MANY, 'a', 'z', PAT_MANY, 'A', 'Z', PAT_MANY, '-', '-', PAT_MANY, '_', '_', PAT_MANY|PAT_TERM, '0', '9'};
    Match *mt = Match_MakePat(rlb->m, bytes(tag), 9, ANCHOR_START)
    mt->jump = Roebling_GetMarkIdx(rlb, XML_ATTRIBUTE);

    return Parser_MakeSingle(rlb->m, mt, setKey); 
}

static Match *spaceMt(Roebling *rlb){
    word sep[] = {
        PAT_ANY, ' ', ' ',
        PAT_ANY, '\t', '\t', PAT_ANY|PAT_TERM, '\r', '\r', PAT_ANY|PAT_TERM, '\n', '\n', PAT_END, 0, 0};
    return  Match_MakePat(rlb->m, bytes(sep), 3, ANCHOR_START); 
}

static Match *selfCloseeMt(Roebling *rlb){
    word close[] = { PAT_TERM, '/', '/', PAT_ANY, ' ', ' ', PAT_ANY, '\t', '\t', PAT_ANY, '\r', '\r', PAT_ANY, '\n', '\n', PAT_TERM, '>', '>', PAT_END, 0, 0};
    Match *mt Match_MakePat(rlb->m, bytes(close), 6, ANCHOR_START); 
    mt->jump = Roebling_GetMarkIdx(rlb, XML_START);
    return mt;
}

static Match *tagOpenedMt(Roebling *rlb){
    word close[] = { PAT_TERM, '>', '>',PAT_END, 0, 0};
    Match *mt Match_MakePat(rlb->m, bytes(close), 1, ANCHOR_START); 
    mt->jump = Roebling_GetMarkIdx(rlb, XML_BODY);
    return mt;
}

static Parser *tagParserMk(Roebling *rlb){
    word tag[] = {PAT_MANY, 'a', 'z', PAT_MANY|PAT_TERM, 'A', 'Z',
        PAT_MANY, 'a', 'z', PAT_MANY, 'A', 'Z', PAT_MANY, '-', '-', PAT_MANY, '_', '_', PAT_MANY|PAT_TERM, '0', '9'};
    Match *mt = Match_MakePat(rlb->m, bytes(tag), 7, ANCHOR_START);
    Parser *prs = Parser_MakeSingle(rlb->m, mt, setTag); 
    prs->ko = spaceMt(rlb->m);
    return prs;
}

static Parser *spaceParserMk(Roebling *rlb){
    Match *sp = spaceMt(rlb->m);
    sp->jump = Roebling_GetMarkIdx(rlb, XML_ATTRIBUTE);
    Parser *prs = Parser_MakeSingle(rlb->m, sp, NULL); 
}

static Parser *tagSpaceParserMk(Roebling *rlb){
    Parser *prs = Parser_MakeSingle(rlb->m, spaceMt(rlb->m), NULL); 
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
    Match *mt = Match_MakePat(rlb->m, bytes(tag), 9, ANCHOR_START)
    Parser *prs = Parser_MakeSingle(rlb->m, mt, setTag); 
    prs->ko = spaceMt(rlb->m);
    return prs;
}

static Parser *postAttrParserMk(Roebling *rlb){
    Array mt_arr = Array_MakeFrom(rlb->m, 2, selfCloseeMt(rlb), tagOpenedMt(rlb));
    return Parser_MakeMulti(rlb->m, mt_arr, tagOpened); 
}

static Parser *attrValueParserMk(Roebling *rlb){
    word quoted[] = {PAT_IGNORE|PAT_TERM, '"', '"', PAT_INVERT, '"', '"', PAT_ANY|PAT_TERM, 32, 255, PAT_IGNORE|PAT_TERM, '"', '"', PAT_END, 0,0};
    word unquoted[] = {PAT_INVERT, '"', '"', PAT_INVERT, ' ', ' ', PAT_INVERT, '\t', '\t', PAT_INVERT, '\r', '\r', PAT_INVERT, '\n', '\n', PAT_ANY|PAT_TERM, 32, 255, PAT_END, 0,0};
    Match *mtq = Match_MakePat(rlb->m, bytes(quoted), 4, ANCHOR_START)
    Match *mtu = Match_MakePat(rlb->m, bytes(tag), 9, ANCHOR_START)
    mtq->mark = mtu->mark = Roebling_GetMarkIdx(rlb, XML_ATTRIBUTE);

    Array mt_arr = Array_MakeFrom(rlb->m, 2, mtq, mtu);
    Parser *prs = Parser_MakeMulti(rlb->m, mt_arr, setAttrValue);
}

static Parser *bodyParserMk(Roebling *rlb){
    word body[] = {PAT_ANY, '\t', '\t', PAT_ANY, '\r', '\r', PAT_ANY, '\n', '\n' PAT_ANY|PAT_TERM, 32, 255, PAT_END, 0,0};
    
    Match *mtq = Match_MakePat(rlb->m, bytes(quoted), 4, ANCHOR_START)
    Match *mtu = Match_MakePat(rlb->m, bytes(tag), 9, ANCHOR_START)
    mtq->mark = mtu->mark = Roebling_GetMarkIdx(rlb, XML_ATTRIBUTE);

    Array mt_arr = Array_MakeFrom(rlb->m, 2, mtq, mtu);
    Parser *prs = Parser_MakeMulti(rlb->m, mt_arr, setAttrValue);
    prs->ko = openMt(rlb);

    return prs;
}

static Parser *sepParserMk(Roebling *rlb){
    word sep[] = { PAT_ANY, ' ', ' ', PAT_ANY, '\t', '\t', PAT_TERM, '=', '=', PAT_END, 0, 0};
    return Parser_MakeSingle(rlb->m, Match_MakePat(rlb->m, bytes(sep), 3, ANCHOR_START), NULL); 
}

Span *XmlParser_Make(MemCtx *m, ProtoDef *def){
    Span *p =  Span_From(m, 6, 
        (Abstract *)xmlStartMarkMk, 
            (Abstract *)startParserMk,
        (Abstract *)xmlTagMarkMk, 
            (Abstract *)tagParserMk,
            (Abstract *)tagSpaceParserMk,
        (Abstract *)xmlAttrMarkMk, 
            (Abstract *)attrParserMk,
            (Abstract *)eqParserMk,
            (Abstract *)postAttrParserMk,
            (Abstract *)attrValueParserMk,
            (Abstract *)postAttrParserMk,
        (Abstract *)xmlBodyMarkMk, 
            (Abstract *)bodyParserMk,
    return p;
}
