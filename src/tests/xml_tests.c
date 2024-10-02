#include <external.h>
#include <caneka.h>

static Roebling *rblMake(MemCtx *m){
    XmlCtx *ctx = XmlCtx_Make((MemHandle *)m, NULL);
    Span *parsers_do = XmlParser_Make(m);

    LookupConfig config[] = {
        {XML_START, (Abstract *)String_Make(m, bytes("XML_START"))},
        {XML_ATTROUTE, (Abstract *)String_Make(m, bytes("XML_ATTROUTE"))},
        {XML_ATTR_VALUE, (Abstract *)String_Make(m, bytes("XML_ATTR_VALUE"))},
        {XML_END, (Abstract *)String_Make(m, bytes("XML_END"))},
        {0, NULL},
    };

    Lookup *desc = Lookup_FromConfig(m, config, NULL);
    return Roebling_Make(m, TYPE_ROEBLING, parsers_do, desc, String_Init(m, STRING_EXTEND), (Abstract *)ctx); 
}

status Xml_Tests(MemCtx *gm){
    status r = TEST_OK;
    MemCtx *m = MemCtx_Make();

    String *s = NULL;
    Roebling *rbl = rblMake(m);
    XmlCtx *ctx = (XmlCtx *)rbl->source;

    s = String_Make(m, bytes("<main/>"));
    Roebling_AddBytes(rbl, s->bytes, s->length);

    Roebling_Run(rbl);
    Hashed *h = (Hashed *)ctx->root->firstChild->value;
    s = (String *)h->item;
    r |= Test(String_EqualsBytes(s, bytes("main")), "Root node equals 'main' have '%s'", s != NULL ? (char *)s->bytes : "NULL");

    s = Roebling_GetMarkDebug(rbl, rbl->jump);
    r |= Test(rbl->jump == Roebling_GetMarkIdx(rbl, XML_ATTROUTE), "Jump next to expected mark %s", s->bytes);

    Roebling_Run(rbl);
    r |= Test(HasFlag(ctx->type.state, SUCCESS) , "XmlCtx has state SUCCSS, have %s", State_ToString(ctx->type.state));

    MemCtx_Free(m);
    return r;
}

status XmlNested_Tests(MemCtx *gm){
    status r = TEST_OK;
    MemCtx *m = MemCtx_Make();

    String *s = NULL;
    Roebling *rbl = rblMake(m);
    XmlCtx *ctx = (XmlCtx *)rbl->source;

    s = String_Make(m, bytes("<main type=\"root\">\n  <alpha>\n    <t model=\"foo\" baseline=\"fancy-pants\">Gotta Get It!</t>\n  </alpha>\n  <alpha>\n    <t model=\"geese\" baseline=\"fancy-pants\">Have It!</t>\n  </alpha>\n</main>"));
    Roebling_AddBytes(rbl, s->bytes, s->length);
    Roebling_Run(rbl);
    Roebling_Run(rbl);
    Roebling_Run(rbl);

    Mess *node = ctx->root->firstChild;
    Mess *bodyNode = NULL;
    Hashed *h = node->atts;
    r |= Test(String_EqualsBytes((String *)h->item, bytes("type")), "Attribute has name 'type', have '%s'", ((String *)h->item)->bytes);
    r |= Test(String_EqualsBytes((String *)h->value, bytes("root")), "Attribute Value to be 'root', have '%s'", ((String *)h->value)->bytes);

    Roebling_Run(rbl);
    r |= Test(rbl->jump == Roebling_GetMarkIdx(rbl, XML_START), "Jump set to XML_START");
    
    Roebling_Run(rbl);
    r |= Test(String_EqualsBytes(node->firstChild->body, bytes("\n  ")), "Whitespace before tag added as body, have '%s'", String_ToEscaped(m, node->firstChild->body)->bytes);

    for(int i = 0; i < 10; i++){
        Roebling_Run(rbl);
    }

    node = ctx->root->firstChild->firstChild->next->firstChild->next;
    Hashed *att = node->atts;
    r |= Test(String_EqualsBytes((String *)att->item, bytes("model")), "Expected att item: 'model', have '%s'", String_ToEscaped(m, (String *)att->item)->bytes);
    r |= Test(String_EqualsBytes((String *)att->value, bytes("foo")), "Expected att value: 'fancy-pants', have '%s'", String_ToEscaped(m, (String *)att->value)->bytes);
    att = att->next;
    r |= Test(String_EqualsBytes((String *)att->item, bytes("baseline")), "Expected att item: 'model', have '%s'", String_ToEscaped(m, (String *)att->item)->bytes);
    r |= Test(String_EqualsBytes((String *)att->value, bytes("fancy-pants")), "Expected att value: 'fancy-pants', have '%s'", String_ToEscaped(m, (String *)att->value)->bytes);
    bodyNode = node->firstChild;
    r |= Test(String_EqualsBytes((String *)bodyNode->body, bytes("Gotta Get It!")), "Expected body value: 'Gotta Get It!', have '%s'", String_ToEscaped(m, (String *)bodyNode->body)->bytes);
    for(int i = 0; i < 6; i++){
        Roebling_Run(rbl);
    }

    node = ctx->root->firstChild->firstChild->next;
    r |= Test(String_EqualsBytes((String *)node->value->item, bytes("alpha")), "NodeName is 'alpha', have '%s'", String_ToEscaped(m, (String *)node->value->item)->bytes);
    node = node->next->next;
    r |= Test(String_EqualsBytes((String *)node->value->item, bytes("alpha")), "Second NodeName is 'alpha', have '%s'", String_ToEscaped(m, (String *)node->value->item)->bytes);

    while(!HasFlag(ctx->type.state, SUCCESS)){
        Roebling_Run(rbl);
    }
    r |= Test(HasFlag(ctx->type.state, SUCCESS), "XML has state success", State_ToString(ctx->type.state));

    MemCtx_Free(m);
    return r;
}
