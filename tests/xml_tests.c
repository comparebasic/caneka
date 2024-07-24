#include <external.h>
#include <caneka.h>

status Xml_Tests(MemCtx *gm){
    status r = TEST_OK;
    MemCtx *m = MemCtx_Make();

    String *s = NULL;
    Roebling *rbl = NULL;

    Span *xmlParsersMk = XmlParser_Make(m, NULL);

    XmlCtx *ctx = XmlCtx_Make((MemHandle *)m, NULL);
    s = String_Make(m, bytes("<main/>"));
    rbl = Roebling_Make(m, TYPE_RBL_XML, xmlParsersMk, s, (Abstract *)ctx);  

    Roebling_Run(rbl);

    s = String_Make(m, bytes("<main alpha=\"apples\" one=1 horizontal>And here is cool stuff</main>"));

    return r;
}
