#include <external.h>
#include <caneka.h>

status Xml_Tests(MemCtx *gm){
    status r = TEST_OK;
    MemCtx *m = MemCtx_Make();

    String *s = NULL;
    Roebling *rbl = NULL;
    XmlCtx *ctx = NULL;

    Span *xmlParsersMk = XmlParser_Make(m);

    ctx = XmlCtx_Make((MemHandle *)m, NULL);

    Span *parsers_do = XmlParser_Make(m);

    LookupConfig config[] = {
        {XML_START, (Abstract *)String_Make(m, bytes("START"))},
        {XML_TAG, (Abstract *)String_Make(m, bytes("TAG"))},
        {XML_ATTROUTE, (Abstract *)String_Make(m, bytes("ATTROUTE"))},
        {XML_ATTRIBUTE, (Abstract *)String_Make(m, bytes("ATTRIBUTE"))},
        {XML_ATTR_VALUE, (Abstract *)String_Make(m, bytes("ATTR_VALUE"))},
        {XML_BODY, (Abstract *)String_Make(m, bytes("BODY"))},
        {XML_END, (Abstract *)String_Make(m, bytes("END"))},
        {0, NULL},
    };

    Lookup *desc = Lookup_FromConfig(m, config, NULL);
    rbl = Roebling_Make(m, TYPE_ROEBLING, parsers_do, desc, String_Init(m, STRING_EXTEND), (Abstract *)ctx); 
    s = String_Make(m, bytes("<main/>"));
    Roebling_AddBytes(rbl, s->bytes, s->length);

    Roebling_Run(rbl);

    s = Roebling_GetMarkDebug(rbl, rbl->jump);
    r |= Test(rbl->jump == Roebling_GetMarkIdx(rbl, XML_TAG), "Jump next to expected mark %s", s->bytes);

    Roebling_Run(rbl);
    r |= Test(ctx->type.state == READY, "Xml state not yet complete, have %s", State_ToString(ctx->type.state));
    s = (String *)ctx->root->firstChild->value->item;
    r |= Test(String_EqualsBytes(s, bytes("main")), "Xml firstChild node is expected, have '%s'", s->bytes);
    Roebling_Run(rbl);
    r |= Test(ctx->type.state == COMPLETE, "Xml state is complete, have %s", State_ToString(ctx->type.state));
    s = Roebling_GetMarkDebug(rbl, rbl->jump);
    r |= Test(rbl->jump == Roebling_GetMarkIdx(rbl, XML_START), "Jump next to expected mark %s", s != NULL ? (char *)s->bytes: "NULL");

    MemCtx_Free(m);
    return r;
}

status XmlNested_Tests(MemCtx *gm){
    status r = TEST_OK;
    MemCtx *m = MemCtx_Make();

    String *s = NULL;
    Roebling *rbl = NULL;
    XmlCtx *ctx = NULL;

    Span *xmlParsersMk = XmlParser_Make(m);

    ctx = XmlCtx_Make((MemHandle *)m, NULL);

    Span *parsers_do = XmlParser_Make(m);

    LookupConfig config[] = {
        {XML_START, (Abstract *)String_Make(m, bytes("START"))},
        {XML_TAG, (Abstract *)String_Make(m, bytes("TAG"))},
        {XML_ATTRIBUTE, (Abstract *)String_Make(m, bytes("ATTRIBUTE"))},
        {XML_ATTR_VALUE, (Abstract *)String_Make(m, bytes("ATTR_VALUE"))},
        {XML_BODY, (Abstract *)String_Make(m, bytes("BODY"))},
        {XML_END, (Abstract *)String_Make(m, bytes("END"))},
        {0, NULL},
    };

    Lookup *desc = Lookup_FromConfig(m, config, NULL);
    rbl = Roebling_Make(m, TYPE_ROEBLING, parsers_do, desc, String_Init(m, STRING_EXTEND), (Abstract *)ctx); 
    s = String_Make(m, bytes("<main type=\"root\">\n  <alpha>\n    <t model=\"foo\" baseline=\"fancy-pants\">Gotta Get It!</t>\n  </alpha>\n  <alpha>\n    <t model=\"geese\" baseline=\"fancy-pants\">Have It!</t>\n  </alpha>\n</main>"));
    Roebling_AddBytes(rbl, s->bytes, s->length);
    printf("\x1b[%dmParsing:\n%s\x1b[0m\n", COLOR_BLUE, s->bytes);

    Roebling_Run(rbl);
    Debug_Print((void *)ctx, 0, "Xml", COLOR_PURPLE, TRUE);
    printf("\n");

    Roebling_Run(rbl);
    Debug_Print((void *)ctx, 0, "Xml", COLOR_PURPLE, TRUE);
    printf("\n");

    Roebling_Run(rbl);
    Debug_Print((void *)ctx, 0, "Xml", COLOR_PURPLE, TRUE);
    printf("\n");

    r |= SUCCESS;

    MemCtx_Free(m);
    return r;
}
