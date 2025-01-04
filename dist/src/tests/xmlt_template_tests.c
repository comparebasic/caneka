#include <external.h>
#include <caneka.h>

static status populatePresenters(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_TIME64, (void *)Time64_Present);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_I64, (void *)I64_Present);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_UTIL, (void *)Util_Present);
    r |= Lookup_Add(m, lk, TYPE_WRAPPED_BOOL, (void *)Bool_Present);
    return r;
}

status XmlTTemplate_Tests(MemCtx *gm){
    Stack(bytes("XtmlTTemplate_Tests"), NULL);

    status r = READY;
    MemCtx *m = MemCtx_Make();

    Lookup *funcs = Lookup_Make(m, _TYPE_START, populatePresenters, NULL);

    XmlTCtx *xmlt = XmlT_Make(m, NULL, funcs);
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
    Table_Set(session, (Abstract *)String_Make(m, bytes("expires")), (Abstract *)Time64_Wrapped(m, Time64_FromSpec(&ts)));

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
    Table_Set(tbl, (Abstract *)String_Make(m, bytes("no-details")), (Abstract *)Bool_Wrapped(m, TRUE));
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

    xml_s = String_Make(m, bytes("<root><h1 with=\"session\">Session expires: ${expires}</h1><e for=\"items\" if=\"show-items\" color=\"${item-color}\"><p>${msg}</p><span if-not=\"no-details\" if=\"details\">${details}</span></e></root>"));

    SetRef((Abstract *)xml_s);

    XmlT_Parse(xmlt, xml_s, tbl);

    s = ctx->root->firstChild->name;

    char *xml_cstr = "<root><h1>Session expires: 2024-03-05T13:33:36.277122774+00</h1><e color=\"blue\"><p>Blue Sky</p><span>Look Up</span></e><e color=\"green\"><p>Grass</p><span>Walk barefoot</span></e><e color=\"yellow\"><p>Sun</p></e><e color=\"red\"><p>Blooooooooooood</p></e></root>";
    expected_s = String_Make(m, bytes(xml_cstr));

    r |= Test(String_Equals(xmlt->result, expected_s), "XmlT created expected output from: %s", xml_cstr);

    MemCtx_Free(m);
    Return r;
}
