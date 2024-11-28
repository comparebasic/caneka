#include <external.h>
#include <caneka.h>

status XmlTTemplate_Tests(MemCtx *gm){

    status r = READY;
    MemCtx *m = MemCtx_Make();

    XmlTCtx *xmlt = XmlT_Make(m, NULL);
    XmlParser *xml = xmlt->parser;
    XmlCtx *ctx = xml->ctx;
    Roebling *rbl = xml->rbl;

    String *s = NULL;
    String *xml_s = NULL;
    String *out_s = NULL;
    String *expected_s = NULL;
    Span *tbl = NULL;




    Span *session = Span_Make(m, TYPE_TABLE);
    Table_Set(session, (Abstract *)String_Make(m, bytes("scid")), (Abstract *)String_Make(m, bytes("a87c782a")));
    Table_Set(session, (Abstract *)String_Make(m, bytes("domain")), (Abstract *)String_Make(m, bytes("customer")));
    struct timespec ts = {1709645616, 277122774};
    Table_Set(session, (Abstract *)String_Make(m, bytes("expires")), (Abstract *)I64_Wrapped(m, Time64_FromSpec(&ts)));

    Span *items = Span_Make(m, TYPE_SPAN);

    tbl = Span_Make(m, TYPE_TABLE);
    Table_Set(tbl, (Abstract *)String_Make(m, bytes("item-color")), (Abstract *)String_Make(m, bytes("blue")));
    Table_Set(tbl, (Abstract *)String_Make(m, bytes("msg")), (Abstract *)String_Make(m, bytes("Blue Sky")));
    Table_Set(tbl, (Abstract *)String_Make(m, bytes("details")), (Abstract *)String_Make(m, bytes("Look Up")));
    Span_Add(items, (Abstract *)tbl);

    tbl = Span_Make(m, TYPE_TABLE);
    Table_Set(tbl, (Abstract *)String_Make(m, bytes("item-color")), (Abstract *)String_Make(m, bytes("green")));
    Table_Set(tbl, (Abstract *)String_Make(m, bytes("msg")), (Abstract *)String_Make(m, bytes("Grass")));
    Table_Set(tbl, (Abstract *)String_Make(m, bytes("details")), (Abstract *)String_Make(m, bytes("Walk barefoot")));
    Span_Add(items, (Abstract *)tbl);

    tbl = Span_Make(m, TYPE_TABLE);
    Table_Set(tbl, (Abstract *)String_Make(m, bytes("item-color")), (Abstract *)String_Make(m, bytes("yellow")));
    Table_Set(tbl, (Abstract *)String_Make(m, bytes("msg")), (Abstract *)String_Make(m, bytes("Sun")));
    Table_Set(tbl, (Abstract *)String_Make(m, bytes("no-details")), (Abstract *)Bool_Wrapped(m, FALSE));
    Span_Add(items, (Abstract *)tbl);

    tbl = Span_Make(m, TYPE_TABLE);
    Table_Set(tbl, (Abstract *)String_Make(m, bytes("item-color")), (Abstract *)String_Make(m, bytes("red")));
    Table_Set(tbl, (Abstract *)String_Make(m, bytes("msg")), (Abstract *)String_Make(m, bytes("Blooooooooooood")));
    Span_Add(items, (Abstract *)tbl);

    tbl = Span_Make(m, TYPE_TABLE);
    Table_Set(tbl, (Abstract *)String_Make(m, bytes("session")), (Abstract *)session);
    Table_Set(tbl, (Abstract *)String_Make(m, bytes("items")), (Abstract *)items);
    Table_Set(tbl, (Abstract *)String_Make(m, bytes("show-items")), (Abstract *)Bool_Wrapped(m, TRUE));
    NestedD *nd = NestedD_Make(m, tbl);

    Debug_Print((void *)items, 0, "items: ", COLOR_PURPLE, TRUE);
    printf("\n");

    Debug_Print((void *)session, 0, "session: ", COLOR_PURPLE, TRUE);
    printf("\n");

    Debug_Print((void *)tbl, 0, "tbl: ", COLOR_PURPLE, TRUE);
    printf("\n");

    Debug_Print((void *)nd, 0, "NestedD: ", COLOR_PURPLE, TRUE);
    printf("\n");

    xml_s = String_Make(m, bytes("<root><h1 with=\"session\">Session expires: ${expires}</h1><e for=\"items\" if=\"show-items\" color=\"${item-color}\"><p>${msg}</p><span if-not=\"no-details\" if=\"details\">${details}</span></e></root>"));

    XmlT_Parse(xmlt, xml_s, tbl);

    s = ctx->root->firstChild->name;
    Debug_Print((void *)s, 0, "FirstChild", COLOR_PURPLE, TRUE);
    printf("\n");

    expected_s = String_Make(m, bytes("<root><h1>Session expires: 2024-03-05T07:33:36.277122774+00</h1><e color=\"green\"><p>Grass</p><span>Walk barefoot</span></e><e color=\"yellow\"><p>Sun</p></e><e color=\"red\"><p>Blooooooooooood</p></e></root>"));

    r |= Test(String_Equals(xmlt->result, expected_s), "XmlT created expected output");
    printf("\n");
    Debug_Print((void *)xmlt->result, 0, "XmlT Result: ", COLOR_CYAN, TRUE);
    printf("\n\n");
    Debug_Print((void *)expected_s, 0, "Expected: ", COLOR_BLUE, TRUE);
    printf("\n");


    MemCtx_Free(m);
    return r;
}
