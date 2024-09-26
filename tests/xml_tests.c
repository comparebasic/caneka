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

    s = Roebling_GetMarkDebug(rbl, rbl->jump);
    r |= Test(rbl->jump == Roebling_GetMarkIdx(rbl, XML_ATTROUTE), "Jump next to expected mark %s", s->bytes);

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
    Roebling *rbl = rblMake(m);
    XmlCtx *ctx = (XmlCtx *)rbl->source;

    s = String_Make(m, bytes("<main type=\"root\">\n  <alpha>\n    <t model=\"foo\" baseline=\"fancy-pants\">Gotta Get It!</t>\n  </alpha>\n  <alpha>\n    <t model=\"geese\" baseline=\"fancy-pants\">Have It!</t>\n  </alpha>\n</main>"));
    Roebling_AddBytes(rbl, s->bytes, s->length);
    printf("\x1b[%dmParsing:\n%s\x1b[0m\n", COLOR_BLUE, s->bytes);

    Roebling_Run(rbl);
    Debug_Print((void *)ctx, 0, "Xml", COLOR_PURPLE, TRUE);
    printf("\n");

    Roebling_Run(rbl);
    Debug_Print((void *)ctx, 0, "Xml", COLOR_PURPLE, TRUE);
    printf("\n");

    /*
    Roebling_Run(rbl);
    Debug_Print((void *)ctx, 0, "Xml", COLOR_PURPLE, TRUE);
    printf("\n");
    */

    r |= SUCCESS;

    MemCtx_Free(m);
    return r;
}
