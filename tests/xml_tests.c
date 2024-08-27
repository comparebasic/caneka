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
    rbl = Roebling_Make(m, TYPE_ROEBLING, parsers_do, 0, String_Init(m, STRING_EXTEND), (Abstract *)ctx); 
    s = String_Make(m, bytes("<main/>"));
    Roebling_AddBytes(rbl, s->bytes, s->length);

    Debug_Print((void *)rbl, 0, "XmlRbl: ", COLOR_CYAN, TRUE);
    printf("\n");

    /*
    Roebling_Run(rbl);
    */

    r |= SUCCESS;

    MemCtx_Free(m);
    return r;
}
