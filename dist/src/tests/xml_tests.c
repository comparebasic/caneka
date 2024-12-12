#include <external.h>
#include <caneka.h>

status Xml_Tests(MemCtx *gm){
    status r = TEST_OK;
    MemCtx *m = MemCtx_Make();

    String *s = NULL;

    XmlParser *xml = XmlParser_Make(m);
    Roebling *rbl = xml->rbl;
    XmlCtx *ctx = xml->ctx;

    s = String_Make(m, bytes("<main/>"));
    Roebling_AddBytes(rbl, s->bytes, s->length);

    Roebling_RunCycle(rbl);
    s = ctx->root->firstChild->name;
    r |= Test(String_EqualsBytes(s, bytes("main")), "Root node equals 'main' have '%s'", s != NULL ? (char *)s->bytes : "NULL");

    s = Roebling_GetMarkDebug(rbl, rbl->jump);
    r |= Test(rbl->jump == Roebling_GetMarkIdx(rbl, XML_ATTROUTE), "Jump next to expected mark %s", s->bytes);

    Roebling_RunCycle(rbl);
    r |= Test(HasFlag(ctx->type.state, SUCCESS) , "XmlCtx has state SUCCSS, have %s", State_ToString(ctx->type.state));

    MemCtx_Free(m);
    return r;
}

status XmlNested_Tests(MemCtx *gm){
    status r = TEST_OK;
    MemCtx *m = MemCtx_Make();

    String *s = String_Make(m, bytes("<main type=\"root\">\n  <alpha>\n    <t model=\"foo\" baseline=\"fancy-pants\">Gotta Get It!</t>\n  </alpha>\n  <alpha>\n    <t model=\"geese\" baseline=\"fancy-pants\">Have It!</t>\n  </alpha>\n</main>"));
    XmlParser *xml = XmlParser_Make(m);
    Roebling *rbl = xml->rbl;
    XmlCtx *ctx = xml->ctx;

    Roebling_AddBytes(rbl, s->bytes, s->length);
    Roebling_RunCycle(rbl);
    Roebling_RunCycle(rbl);
    Roebling_RunCycle(rbl);

    Mess *node = ctx->root->firstChild;
    Mess *bodyNode = NULL;
    Hashed *h = Span_Get(node->atts, node->atts->metrics.get);

    r |= Test(String_EqualsBytes((String *)h->item, bytes("type")), "Attribute has name 'type', have '%s'", ((String *)h->item)->bytes);
    r |= Test(String_EqualsBytes((String *)h->value, bytes("root")), "Attribute Value to be 'root', have '%s'", ((String *)h->value)->bytes);

    Roebling_RunCycle(rbl);
    r |= Test(rbl->jump == Roebling_GetMarkIdx(rbl, XML_START), "Jump set to XML_START");
    
    Roebling_RunCycle(rbl);
    r |= Test(String_EqualsBytes(node->firstChild->body, bytes("\n  ")), "Whitespace before tag added as body, have '%s'", String_ToEscaped(m, node->firstChild->body)->bytes);

    for(int i = 0; i < 10; i++){
        Roebling_RunCycle(rbl);
    }

    node = ctx->root->firstChild->firstChild->next->firstChild->next;

    String *result = (String *)Table_Get(node->atts, (Abstract *)String_Make(m, bytes("model")));
    r |= Test(String_EqualsBytes((String *)result, bytes("foo")), "Expected att value: 'fancy-pants', have '%s'", String_ToEscaped(m, result)->bytes);

    result = (String *)Table_Get(node->atts, (Abstract *)String_Make(m, bytes("baseline")));
    r |= Test(String_EqualsBytes((String *)result, bytes("fancy-pants")), "Expected att value: 'fancy-pants', have '%s'", String_ToEscaped(m, result));

    bodyNode = node->firstChild;
    r |= Test(String_EqualsBytes((String *)bodyNode->body, bytes("Gotta Get It!")), "Expected body value: 'Gotta Get It!', have '%s'", String_ToEscaped(m, (String *)bodyNode->body)->bytes);
    for(int i = 0; i < 6; i++){
        Roebling_RunCycle(rbl);
    }

    node = ctx->root->firstChild->firstChild->next;
    r |= Test(String_EqualsBytes((String *)node->name, bytes("alpha")), "NodeName is 'alpha', have '%s'", String_ToEscaped(m, (String *)node->name)->bytes);
    node = node->next->next;
    r |= Test(String_EqualsBytes((String *)node->name, bytes("alpha")), "Second NodeName is 'alpha', have '%s'", String_ToEscaped(m, (String *)node->name)->bytes);

    while(!HasFlag(ctx->type.state, SUCCESS)){
        Roebling_RunCycle(rbl);
    }
    r |= Test(HasFlag(ctx->type.state, SUCCESS), "XML has state success", State_ToString(ctx->type.state));

    MemCtx_Free(m);
    return r;
}

status XmlParser_Tests(MemCtx *gm){
    status r = TEST_OK;
    MemCtx *m = MemCtx_Make();
    XmlParser *xml = XmlParser_Make(m); 

    String *s = String_Make(m, bytes("<main><sub>The World is Upside Down!</sub></main>"));
    XmlParser_Parse(xml, s);
    Mess *node = xml->ctx->root->firstChild->firstChild->firstChild;
    r |= Test(HasFlag(xml->ctx->type.state, SUCCESS), "XML has state success", State_ToString(xml->ctx->type.state));
    r |= Test(String_EqualsBytes((String *)node->body, bytes("The World is Upside Down!")), "XML node has expected body, have '%s'", ((String *)node->body)->bytes);

    return r;
}

status XmlStringType_Tests(MemCtx *gm){
    status r = TEST_OK;
    MemCtx *m = MemCtx_Make();
    XmlParser *xml = XmlParser_Make(m); 
    String *s = NULL;

    s = String_Make(m, bytes("<main type=\"root\" cat=\"${fancy}\" count=\"19\" max=3>Not a cash<s>${body}</s></main>"));
    XmlParser_Parse(xml, s);
    Mess *node = xml->ctx->root->firstChild;

    s = (String *)Table_Get(node->atts, (Abstract *)String_Make(m, bytes("type")));
    r |= Test(s->type.state == ZERO, "String root is normal, no flags for '%s', have %d", s->bytes, s->type.state);
    s = (String *)Table_Get(node->atts, (Abstract *)String_Make(m, bytes("cat")));
    r |= Test((s->type.state & FLAG_STRING_IS_CASH) != 0, "String is idendified as FLAG_STRING_IS_CASH for '%s', have %d", s->bytes, s->type.state);
    s = (String *)Table_Get(node->atts, (Abstract *)String_Make(m, bytes("count")));
    r |= Test((s->type.state & FLAG_STRING_IS_NUM) != 0, "String is idendified as FLAG_STRING_IS_NUM for '%s', have %d", s->bytes, s->type.state);
    s = (String *)Table_Get(node->atts, (Abstract *)String_Make(m, bytes("max")));
    r |= Test((s->type.state & FLAG_STRING_IS_NUM) != 0, "String is idendified as FLAG_STRING_IS_NUM for '%s', have %d", s->bytes, s->type.state);

    s = node->firstChild->body;
    r |= Test(s->type.state == ZERO, "String body is normal, no flags for '%s', have %d", s->bytes, s->type.state);

    s = node->firstChild->next->firstChild->body;
    r |= Test((s->type.state & FLAG_STRING_IS_CASH) != 0, "Sub-element body is idendified as FLAG_STRING_IS_CASH for '%s', have %d", s->bytes, s->type.state);

    return r;
}
