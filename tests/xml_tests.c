#include <external.h>
#include <caneka.h>

status Xml_Tests(MemCtx *gm){
    status r = TEST_OK;
    MemCtx *m = MemCtx_Make();

    String *s = NULL;
    Roebling *rbl = NULL;
    XmlCtx *ctx = NULL;

    Span *xmlParsersMk = XmlParser_Make(m, NULL);

    ctx = XmlCtx_Make((MemHandle *)m, NULL);
    s = String_Make(m, bytes("<main/>"));
    rbl = Roebling_Make(m, TYPE_RBL_XML, xmlParsersMk, s, (Abstract *)ctx);  

    Roebling_Run(rbl);

    Debug_Print((void *)ctx, 0, "Xml: ", COLOR_PURPLE, TRUE);
    printf("\n");

    ctx = XmlCtx_Make((MemHandle *)m, NULL);
    s = String_Make(m, bytes("<main alpha=\"apples\" one=1 horizontal>And here is cool stuff</main>"));
    rbl = Roebling_Make(m, TYPE_RBL_XML, xmlParsersMk, s, (Abstract *)ctx);  

    Roebling_Run(rbl);

    Debug_Print((void *)ctx, 0, "Xml: ", COLOR_PURPLE, TRUE);
    printf("\n");

    return r;
}
