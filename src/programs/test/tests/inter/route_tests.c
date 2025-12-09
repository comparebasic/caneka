#include <external.h>
#include <caneka.h>
#include <test_module.h>

static char *cstr = ""
    "<!DOCTYPE html>\n"
    "<html lang=\"en\">\n"
    "<head>\n"
    "<meta charset=\"utf-8\">\n"
    "<title>Caneka Example - Stats </title>\n"
    "  <link rel=\"stylesheet\" href=\"/static/style.css\" />\n"
    "  <meta name=\"viewport\" content=\"width=device-width,maximum-scale=1.0,initial-scale=1.0,minimum-scale=1.0,user-scalable=yes,shrink-to-fit=no\">\n"
    "</head>\n"
    "<body>\n"
    "<header>\n"
    "    <span class=\"logo\">Caneka Demo Site</span>\n"
    "    <nav class=\"bread-crumbs\">\n"
    "    <ul>\n"
    "        <li>\n"
    "            <a href=\"/\">About</a>\n"
    "        </li>\n"
    "        <li>\n"
    "            <a href=\"/stats\">Stats</a>\n"
    "        </li>\n"
    "        <li>\n"
    "            <a href=\"/tests\">Tests</a>\n"
    "        </li>\n"
    "        <li>\n"
    "            <a href=\"/schedule\">Features</a>\n"
    "        </li>\n"
    "    </ul>\n"
    "    </nav>\n"
    "</header>\n"
    "<section id=\"page-page\">\n"
    ;

static char *noMemCstr  = ""
    "<h1>Statistics of the Example</h1>\n"
    "<p>Server running since 2025-11-10T23:24:13.127000000000+00</p>\n"
    ;

static char *noMemWHeaderCstr  = ""
    "<!DOCTYPE html>\n"
    "<html lang=\"en\">\n"
    "<head>\n"
    "<meta charset=\"utf-8\">\n"
    "<title>Caneka Example - Stats </title>\n"
    "  <link rel=\"stylesheet\" href=\"/static/style.css\" />\n"
    "  <meta name=\"viewport\" content=\"width=device-width,maximum-scale=1.0,initial-scale=1.0,minimum-scale=1.0,user-scalable=yes,shrink-to-fit=no\">\n"
    "</head>\n"
    "<body>\n"
    "<header>\n"
    "    <span class=\"logo\">Caneka Demo Site</span>\n"
    "    <nav class=\"bread-crumbs\">\n"
    "    <ul>\n"
    "        <li>\n"
    "            <a href=\"/\">About</a>\n"
    "        </li>\n"
    "        <li>\n"
    "            <a href=\"/stats\">Stats</a>\n"
    "        </li>\n"
    "        <li>\n"
    "            <a href=\"/tests\">Tests</a>\n"
    "        </li>\n"
    "        <li>\n"
    "            <a href=\"/schedule\">Features</a>\n"
    "        </li>\n"
    "    </ul>\n"
    "    </nav>\n"
    "</header>\n"
    "<section id=\"page-page\">\n"
    "<h1>Statistics of the Example</h1>\n"
    "<p>Server running since 2025-11-10T23:24:13.127000000000+00</p>\n"
    "</section>\n"
    "<script type=\"text/javascript\" src=\"/static/ui.js\"></script>\n"
    "<footer>\n"
    "    <img alt=\"logo\" src=\"/static/logo-transparent-white_256.png\" class=\"Compare Basic logo\" />\n"
    "    <div>\n"
    "    <p>\n"
    "    Caneka is a runtime from <a href=\"https://comparebasic.com\">Compare Basic</a> and \n"
    "    is available for use under a \n"
    "        <a href=\"https://github.com/comparebasic/caneka/blob/main/LICENCE\">3-Clause BSD Licence</a>.\n"
    "    </p>\n"
    "    <p>\n"
    "        More details at <a href=\"https://caneka.org\">caneka.org</a>.\n"
    "    </p>\n"
    "    <p>\n"
    "        &copy; Copyright 2025 Compare Basic Incorporated\n"
    "    </p>\n"
    "    </div>\n"
    "</footer>\n"
    "</body>\n"
    "</html>\n"
    ;

static char *memWHeaderCstr  = ""
    "<!DOCTYPE html>\n"
    "<html lang=\"en\">\n"
    "<head>\n"
    "<meta charset=\"utf-8\">\n"
    "<title>Caneka Example - Stats </title>\n"
    "  <link rel=\"stylesheet\" href=\"/static/style.css\" />\n"
    "  <meta name=\"viewport\" content=\"width=device-width,maximum-scale=1.0,initial-scale=1.0,minimum-scale=1.0,user-scalable=yes,shrink-to-fit=no\">\n"
    "</head>\n"
    "<body>\n"
    "<header>\n"
    "    <span class=\"logo\">Caneka Demo Site</span>\n"
    "    <nav class=\"bread-crumbs\">\n"
    "    <ul>\n"
    "        <li>\n"
    "            <a href=\"/\">About</a>\n"
    "        </li>\n"
    "        <li>\n"
    "            <a href=\"/stats\">Stats</a>\n"
    "        </li>\n"
    "        <li>\n"
    "            <a href=\"/tests\">Tests</a>\n"
    "        </li>\n"
    "        <li>\n"
    "            <a href=\"/schedule\">Features</a>\n"
    "        </li>\n"
    "    </ul>\n"
    "    </nav>\n"
    "</header>\n"
    "<section id=\"page-page\">\n"
    "<h1>Statistics of the Example</h1>\n"
    "<p>Server running since 2025-11-10T23:24:13.127000000000+00</p>\n"
    "<h2>Memory Heap</h2>\n"
    "<p>Total Heap Size: <b>904k</b> of 16m</p>\n"
    "<ul>\n"
    "    <li>\n"
    "        <p><b>recycled</b>: 4</p>\n"
    "    </li>\n"
    "    <li>\n"
    "        <p><b>total</b>: 222</p>\n"
    "    </li>\n"
    "    <li>\n"
    "        <p><b>bookIdx</b>: 0</p>\n"
    "    </li>\n"
    "    <li>\n"
    "        <p><b>pageIdx</b>: 226</p>\n"
    "    </li>\n"
    "</ul>\n"
    ;

static char *homeCstr = ""
    "<!DOCTYPE html>\n"
    "<html lang=\"en\">\n"
    "<head>\n"
    "<meta charset=\"utf-8\">\n"
    "<title>Caneka Example - Stats </title>\n"
    "  <link rel=\"stylesheet\" href=\"/static/style.css\" />\n"
    "  <meta name=\"viewport\" content=\"width=device-width,maximum-scale=1.0,initial-scale=1.0,minimum-scale=1.0,user-scalable=yes,shrink-to-fit=no\">\n"
    "</head>\n"
    "<body>\n"
    "<header>\n"
    "    <span class=\"logo\">Caneka Demo Site</span>\n"
    "    <nav class=\"bread-crumbs\">\n"
    "    <ul>\n"
    "        <li>\n"
    "            <a href=\"/\">About</a>\n"
    "        </li>\n"
    "        <li>\n"
    "            <a href=\"/stats\">Stats</a>\n"
    "        </li>\n"
    "        <li>\n"
    "            <a href=\"/tests\">Tests</a>\n"
    "        </li>\n"
    "        <li>\n"
    "            <a href=\"/schedule\">Features</a>\n"
    "        </li>\n"
    "    </ul>\n"
    "    </nav>\n"
    "</header>\n"
    "<section id=\"page-page\">\n"
    "<H1>About </H1>\n"
    "<P>Yay, homepage loads</P>\n"
    "</section>\n"
    "<script type=\"text/javascript\" src=\"/static/ui.js\"></script>\n"
    "<footer>\n"
    "    <img alt=\"logo\" src=\"/static/logo-transparent-white_256.png\" class=\"Compare Basic logo\" />\n"
    "    <div>\n"
    "    <p>\n"
    "    Caneka is a runtime from <a href=\"https://comparebasic.com\">Compare Basic</a> and \n"
    "    is available for use under a \n"
    "        <a href=\"https://github.com/comparebasic/caneka/blob/main/LICENCE\">3-Clause BSD Licence</a>.\n"
    "    </p>\n"
    "    <p>\n"
    "        More details at <a href=\"https://caneka.org\">caneka.org</a>.\n"
    "    </p>\n"
    "    <p>\n"
    "        &copy; Copyright 2025 Compare Basic Incorporated\n"
    "    </p>\n"
    "    </div>\n"
    "</footer>\n"
    "</body>\n"
    "</html>\n"
    ;

static Table *getGenericData(MemCh *m, Route *rt){
    Table *data = Table_Make(m);

    Table *atts = Table_Make(m);
    Table_Set(atts, K(m, "title"), 
        K(m, "Example Title"));

    Str *path = IoUtil_GetAbsPath(m,
        Str_CstrRef(m, "./examples/test/pages/public/stats.config"));
    Inst *config = Config_FromPath(m, path);
    Table_Set(data, Str_CstrRef(m, "config"), config);

    StrVec *navPath = IoUtil_GetAbsVec(m,
        K(m, "./examples/test/pages/nav.config"));

    Span *nav = Nav_TableFromPath(m, rt, navPath);
    Table_Set(data, Str_CstrRef(m, "nav"), nav);
    return data;
}

status WwwRoute_Tests(MemCh *m){
    DebugStack_Push(NULL, ZERO);
    void *args[5];
    status r = READY;

    StrVec *path = IoPath(m, "examples/test/pages/public");
    Route *rt = Route_From(m, path);

    path = IoPath(m, "/tests");
    Route *tests = Inst_ByPath(rt, path, NULL, SPAN_OP_GET);

    args[0] = path;
    args[1] = NULL;
    r |= Test(tests != NULL, "Route for @ is not NULL", args);

    Str *mime = Seel_Get(tests, K(m, "mime"));
    args[0] = mime;
    args[1] = NULL;
    r |= Test(Equals(mime, K(m, "text/html")), 
        "account tests page is mime type html, have @", args);

    Str *type = Seel_Get(tests, K(m, "type"));
    args[0] = type;
    args[1] = NULL;
    r |= Test(Equals(type, K(m, "fmt")), 
        "account tests page is type fmt, have @", args);

    path = StrVec_From(m, K(m, "/stats"));
    IoUtil_Annotate(m, path);
    Route *profile = Inst_ByPath(rt, path, NULL, SPAN_OP_GET);
    mime = Seel_Get(profile, K(m, "mime"));
    r |= Test(Equals(mime, K(m, "text/html")),
        "profile stat page is mime type text/html", NULL);

    type = Seel_Get(profile, K(m, "mime"));
    args[0] = type;
    args[1] = NULL;
    r |= Test(Equals(type, K(m, "text/html")),
        "profile stat page is mime type templ, have @", args);

    DebugStack_Pop();
    return r;
}

status WwwRouteTempl_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    void *args[5];
    status r = READY;

    DebugStack_SetRef("header.templ", TYPE_CSTR);

    StrVec *path = IoPath(m, "examples/test/pages/public");
    Route *rt = Route_From(m, path);

    Route *inc = Route_Make(m);
    StrVec *incPath = IoUtil_GetAbsVec(m,
        Str_CstrRef(m, "./examples/test/pages/inc"));
    StrVec *incAbs = IoUtil_AbsVec(m, incPath);
    Route_Collect(inc, incAbs);

    Table *data = getGenericData(m, rt);

    struct timespec now;
    Time_Now(&now);
    Table_Set(data, Str_CstrRef(m, "now"), Time_Wrapped(m, &now));
    StrVec *title = StrVec_From(m, Str_CstrRef(m, "The Title of the Master Page"));
    Table_Set(data, Str_CstrRef(m, "title"), title);

    path = StrVec_From(m, Str_CstrRef(m,
        "./examples/test/pages/inc/header.templ"));
    StrVec *abs = IoUtil_AbsVec(m, path);

    StrVec *v = File_ToVec(m, StrVec_Str(m, abs));

    Cursor *curs = Cursor_Make(m, v);
    TemplCtx *ctx = TemplCtx_FromCurs(m, curs, NULL);

    Templ *templ = (Templ *)Templ_Make(m, ctx->it.p);
    Templ_Prepare(templ);

    StrVec *out = StrVec_Make(m);
    Buff *bf = Buff_From(m, out);

    Templ_ToS(templ, bf, data, NULL);
    Str *expected = Str_FromCstr(m, cstr, ZERO);
    args[0] = out;
    args[1] = data;
    args[2] = NULL;
    expected->type.state |= DEBUG;
    r |= TestShow(Equals(out, expected),
        "Templ from Route has expected output",
        "Templ output does not match @", args);

    /* stats.templ no mem details */
    DebugStack_SetRef("stats.templ no mem details", TYPE_CSTR);

    path = IoPath(m, "/stats");
    Route *handler = Inst_ByPath(rt, path, NULL, SPAN_OP_GET);

    data = getGenericData(m, rt);
    Table *stats = Table_Make(m);

    struct timespec fictional = {1762817053, 127000000000};

    Table_Set(stats, K(m, "uptime"),
        Time_ToStr(m, &fictional));
    Table_Set(data, K(m, "stats"), stats);

    bf = Buff_Make(m, ZERO);
    Route_Handle(handler, bf, data, NULL);
    
    expected = Str_FromCstr(m, noMemCstr, ZERO);
    args[0] = bf->v;
    args[1] = NULL;
    expected->type.state |= DEBUG;
    r |= TestShow(Equals(expected, bf->v),
        "Handler: Expected template value with no header and mem details", 
        "Handler: Expected template value with no header and mem: $", args);

    DebugStack_SetRef("stats.templ no mem details + header", TYPE_CSTR);

    StrVec *hv = IoPath(bf->m, "header");
    Route *header = Inst_ByPath(inc, hv, NULL, SPAN_OP_GET);
    StrVec *fv = IoPath(bf->m, "footer");
    Route *footer = Inst_ByPath(inc, fv, NULL, SPAN_OP_GET);

    Buff *dest = Buff_Make(m, ZERO);

    bf = Buff_Make(m, ZERO);
    Route_Handle(header, bf, data, NULL);
    Buff_Pipe(dest, bf);

    bf = Buff_Make(m, ZERO);
    Route_Handle(handler, bf, data, NULL);
    Buff_Pipe(dest, bf);

    bf = Buff_Make(m, ZERO);
    Route_Handle(footer, bf, data, NULL);
    Buff_Pipe(dest, bf);

    expected = Str_FromCstr(m, noMemWHeaderCstr, ZERO);
    args[0] = expected;
    args[1] = dest->v;
    args[2] = NULL;
    expected->type.state |= DEBUG;
    r |= TestShow(Equals(dest->v, expected), 
        "Footer: Expected template value with no mem object and a header",
        "Footer: Expected template value with no mem object and a header: expected:$\nhave:$", 
    args);

    DebugStack_SetRef("stats.templ mem details", TYPE_CSTR);

    MemBookStats st = {
        .type = {TYPE_BOOK_STATS, ZERO},
        .bookIdx = 0,
        .pageIdx = 226,
        .recycled = 4,
        .total = 222,
    };

    data = getGenericData(m, rt);
    stats = Table_Make(m);
    Table_Set(stats, K(m, "uptime"),
        Time_ToStr(m, &fictional));

    Table *mem = Table_Make(m);
    Table_Set(mem, K(m, "mem-used"), Str_MemCount(m, st.pageIdx * PAGE_SIZE));
    Table_Set(mem, K(m, "mem-total"), Str_MemCount(m, PAGE_COUNT * PAGE_SIZE));

    Table_Set(mem, K(m, "mem-details"), Map_ToTable(m, &st));
    Table_Set(stats, K(m, "mem"), mem);
    Table_Set(data, K(m, "stats"), stats);

    bf = Buff_Make(m, ZERO);
    Route_Handle(header, bf, data, NULL);

    path = StrVec_From(m, Str_CstrRef(m, "/stats"));
    IoUtil_Annotate(m, path);
    handler = Inst_ByPath(rt, path, NULL, SPAN_OP_GET);

    Route_Handle(handler, bf, data, NULL);
    dest = Buff_Make(m, ZERO);
    Buff_Pipe(dest, bf);

    expected = Str_FromCstr(m, memWHeaderCstr, ZERO);
    args[0] = dest->v;
    args[1] = NULL;
    expected->type.state |= DEBUG;
    r |= TestShow(Equals(expected, dest->v),
        "Expected mem details in template", 
        "Expected mem details in template $", args);

    DebugStack_Pop();
    return r;
}

status WwwRouteFmt_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    void *args[5];
    status r = READY;
    DebugStack_SetRef("fmt value", TYPE_CSTR);

    StrVec *path = IoPath(m, "examples/test/pages/public");
    Route *rt = Route_From(m, path);

    Route *inc = Route_Make(m);
    StrVec *incPath = IoUtil_GetAbsVec(m,
        Str_CstrRef(m, "./examples/test/pages/inc"));
    StrVec *incAbs = IoUtil_AbsVec(m, incPath);
    Route_Collect(inc, incAbs);

    StrVec *hv = IoPath(m, "header");
    Route *header = Inst_ByPath(inc, hv, NULL, SPAN_OP_GET);
    StrVec *fv = IoPath(m, "footer");
    Route *footer = Inst_ByPath(inc, fv, NULL, SPAN_OP_GET);

    Table *data = getGenericData(m, rt);
    Buff *bf = Buff_Make(m, ZERO);
    Route_Handle(header, bf, data, NULL);

    path = StrVec_From(m, Str_CstrRef(m, "/"));
    IoUtil_Annotate(m, path);
    Route *handler = Inst_ByPath(rt, path, NULL, SPAN_OP_GET);

    r |= Test(handler != NULL, "Default / handler is not null", args);

    Route_Handle(handler, bf, data, NULL);
    Buff *dest = Buff_Make(m, ZERO);
    Buff_Pipe(dest, bf);

    bf = Buff_Make(m, ZERO);
    Route_Handle(footer, bf, data, NULL);
    Buff_Pipe(dest, bf);
    
    Str *expected = Str_FromCstr(m, homeCstr, ZERO);
    args[0] = dest->v;
    args[1] = NULL;
    expected->type.state |= DEBUG;
    r |= TestShow(Equals(expected, dest->v),
        "Expected fmt value", 
        "Expected fmt value $", args);

    DebugStack_Pop();
    return r;
}

static boolean _fired = FALSE;

static status fakeStep(Step *st, Task *tsk){
    _fired = TRUE;
    return NOOP;
}

status WwwPath_Tests(MemCh *m){
    DebugStack_Push(NULL, ZERO);
    void *args[6];
    status r = READY;

    Route *pages = Route_Make(m);
    r |= Route_Collect(pages, IoAbsPath(m, "examples/test/pages/public"));

    StrVec *name = IoPath(m, "/stats");

    Route *statHandler = Inst_ByPath(pages, name, NULL, SPAN_OP_GET);
    Single *funcW = Ptr_Wrapped(m, fakeStep, TYPE_STEP_FUNC);
    Span_Set(statHandler, ROUTE_PROPIDX_ADD_STEP, funcW);

    StrVec *navPath = IoAbsPath(m,"examples/test/pages/nav.config");
    Span *nav = Nav_TableFromPath(m, pages, navPath);

    Route *inc = Route_Make(m);
    r |= Route_Collect(inc, IoAbsPath(m, "examples/test/pages/inc"));

    Route *stat = Route_From(m, IoAbsPath(m, "examples/test/pages/static"));
    Inst_ByPath(pages, IoPath(m, "/static/"), stat, SPAN_OP_SET);

    Route *sys = Route_From(m, IoAbsPath(m, "examples/test/pages/system"));
    Inst_ByPath(pages, IoPath(m, "/system/"), sys, SPAN_OP_SET);

    Route *route = Route_GetHandler(pages,
        IoPath(m, "/static/logo-transparent-white_256.png")); 
    args[0] = IoAbsPath(m,
        "examples/test/pages/static/logo-transparent-white_256.png");
    args[1] = Seel_Get(route, S(m, "file"));
    args[2] = NULL;
    r |= Test(route != NULL && Equals(args[1], args[0]), 
        "Route has expected file expected @, found @", args);

    route = Route_GetHandler(pages, IoPath(m, "/")); 
    args[0] = IoAbsPath(m, "examples/test/pages/public/index.fmt");
    args[1] = Seel_Get(route, S(m, "file"));
    args[2] = NULL;
    r |= Test(route != NULL && Equals(args[1], args[0]), 
        "Route has expected file expected @, found @", args);

    route = Route_GetHandler(pages, IoPath(m, "/system/not-found")); 
    args[0] = IoAbsPath(m, "examples/test/pages/system/not-found.templ");
    args[1] = Seel_Get(route, S(m, "file"));
    args[2] = NULL;
    r |= Test(route != NULL && Equals(args[1], args[0]), 
        "Route has expected file expected @, found @", args);

    DebugStack_Pop();
    return r;
}

status WwwRouteMime_Tests(MemCh *m){
    DebugStack_Push(NULL, ZERO);
    void *args[6];
    status r = READY;

    Route *rt = Route_Make(m);

    StrVec *path = IoUtil_GetAbsVec(m, Str_CstrRef(m, "./examples/test/pages/static"));
    Route_Collect(rt, path);

    StrVec *key = StrVec_From(m, Str_FromCstr(m, "/style.css", STRING_COPY));
    IoUtil_Annotate(m, key);
    Path_JoinBase(m, key);

    Route *subRt = Table_ByPath(
        Span_Get(rt, ROUTE_PROPIDX_CHILDREN), key, NULL, SPAN_OP_GET);

    r |= Test(subRt != NULL, "Static route is not null", NULL);

    Buff *bf = Buff_Make(m, ZERO);
    Route_Handle(subRt, bf, NULL, NULL);

    Buff *dest = Buff_Make(m, ZERO);
    Buff_Pipe(dest, bf);

    Str *pathS = IoUtil_GetAbsPath(m, K(m, "./examples/test/pages/static/style.css"));
    StrVec *expected = File_ToVec(m, pathS);

    r |= Test(Equals(dest->v, expected),
        "Content from Buff piped from route matches reading file directly", NULL);

    DebugStack_Pop();
    return r;
}

status WwwRouteRbs_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    void *args[5];
    status r = READY;

    ProtoCtx *proto = HttpProto_Make(m);
    HttpCtx *ctx = (HttpCtx*)as(proto->ctx, TYPE_HTTP_CTX);

    Str *content = S(m, 
        "{\"email\": \"fancy.pantsy@example.com\", \"first-name\": \"Fantsy\"}");

    args[0] = Str_FromI64(m, content->length);
    args[1] = content;
    args[2] = NULL;
    StrVec *v = Fmt_ToStrVec(m, 
        "POST /forms/signup?action=add HTTP/1.1\r\n"
        "User-Agent: Firefudge/Aluminum\r\n"
        "Content-Type: application/json\r\n"
        "Accept: text/html\r\n"
        "Content-Length: $\r\n"
        "\r\n"
        "$", args);

    Cursor *curs = Cursor_Make(m, v);
    Roebling *rbl = HttpRbl_Make(m, curs, proto);
    Roebling_Run(rbl);

    r |= Test(rbl->type.state & SUCCESS, "Roebling finished with state SUCCESS", NULL);

    NodeObj *config = Inst_Make(m, TYPE_NODEOBJ);
    HttpCtx_ParseBody(ctx, config, curs);

    Buff *bf = Buff_Make(m, BUFF_UNBUFFERED); 
    StrVec *rbsPath = IoAbsPath(m, "examples/test/pages/forms/");

    Route *root = Route_Make(m);
    Route_Collect(root, rbsPath);

    StrVec *url = IoPath(m, "/signup"); 
    Route *rt = Route_GetHandler(root, url);

    args[0] = ctx;
    args[1] = rt;
    Out("^p.Ctx @\n\nRoute @^0\n", args);

    r |= ERROR;

    DebugStack_Pop();
    return r;
}
