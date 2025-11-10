#include <external.h>
#include <caneka.h>

static char *cstr = ""
    "<!DOCTYPE html>\n"
    "<html lang=\"en\">\n"
    "<head>\n"
    "<meta charset=\"utf-8\">\n"
    "<title>Example Title</title>\n"
    "  <link rel=\"stylesheet\" href=\"/style.css\" />\n"
    "  <meta name=\"viewport\" content=\"width=device-width,maximum-scale=1.0,initial-scale=1.0,minimum-scale=1.0,user-scalable=yes,shrink-to-fit=no\">\n"
    "</head>\n"
    "<body>\n"
    "<header>\n"
    "    <nav class=\"bread-crumbs\">\n"
    "    <ul>\n"
    "        <li>\n"
    "            <a href=\"/login\">/login</a>\n"
    "        </li>\n"
    "        <li>\n"
    "            <a href=\"/logo-transparent.png\">/logo-transparent.png</a>\n"
    "        </li>\n"
    "        <li>\n"
    "            <a href=\"/dom.js\">/dom.js</a>\n"
    "        </li>\n"
    "        <li>\n"
    "            <a href=\"/account/\">/account/</a>\n"
    "        </li>\n"
    "        <li>\n"
    "            <a href=\"/style.css\">/style.css</a>\n"
    "        </li>\n"
    "    </ul>\n"
    "    </nav>\n"
    "</header>\n"
;

static char *loginCstr  = ""
    "<h1>Caneka Example - Login</h1>\n"
    "<p>Login to the example site.</p>\n"
    "<div>\n"
    "    <span>Logged in as Fancy Pantsy</span>\n"
    "    <form>\n"
    "        <label name=\"name>\n"
    "            <input type=\"text\" size=\"120\" />\n"
    "        </label>\n"
    "        <button type=\"submit\">Login</button>\n"
    "    </form>\n"
    "</div>\n"
    ;

static char *loginNoUserCstr  = ""
    "<!DOCTYPE html>\n"
    "<html lang=\"en\">\n"
    "<head>\n"
    "<meta charset=\"utf-8\">\n"
    "<title>Caneka Example - Login</title>\n"
    "  <link rel=\"stylesheet\" href=\"/style.css\" />\n"
    "  <meta name=\"viewport\" content=\"width=device-width,maximum-scale=1.0,initial-scale=1.0,minimum-scale=1.0,user-scalable=yes,shrink-to-fit=no\">\n"
    "</head>\n"
    "<body>\n"
    "<header>\n"
    "    <nav class=\"bread-crumbs\">\n"
    "    <ul>\n"
    "        <li>\n"
    "            <a href=\"/login\">/login</a>\n"
    "        </li>\n"
    "        <li>\n"
    "            <a href=\"/logo-transparent.png\">/logo-transparent.png</a>\n"
    "        </li>\n"
    "        <li>\n"
    "            <a href=\"/dom.js\">/dom.js</a>\n"
    "        </li>\n"
    "        <li>\n"
    "            <a href=\"/account/\">/account/</a>\n"
    "        </li>\n"
    "        <li>\n"
    "            <a href=\"/style.css\">/style.css</a>\n"
    "        </li>\n"
    "    </ul>\n"
    "    </nav>\n"
    "</header>\n"
    "<h1>Caneka Example - Login</h1>\n"
    "<p>Login to the example site.</p>\n"
    "<div>\n"
    "    <form>\n"
    "        <label name=\"name>\n"
    "            <input type=\"text\" size=\"120\" />\n"
    "        </label>\n"
    "        <button type=\"submit\">Login</button>\n"
    "    </form>\n"
    "</div>\n"
    "<footer>\n"
    "    Caneka example site - <a href=\"https://caneka.org\">view caneka.org</a>\n"
    "</footer>\n"
    "</body>\n"
    "</html>\n"
    ;

static char *homeCstr = ""
    "<!DOCTYPE html>\n"
    "<html lang=\"en\">\n"
    "<head>\n"
    "<meta charset=\"utf-8\">\n"
    "<title>Example Title</title>\n"
    "  <link rel=\"stylesheet\" href=\"/style.css\" />\n"
    "  <meta name=\"viewport\" content=\"width=device-width,maximum-scale=1.0,initial-scale=1.0,minimum-scale=1.0,user-scalable=yes,shrink-to-fit=no\">\n"
    "</head>\n"
    "<body>\n"
    "<header>\n"
    "    <nav class=\"bread-crumbs\">\n"
    "    <ul>\n"
    "        <li>\n"
    "            <a href=\"/login\">/login</a>\n"
    "        </li>\n"
    "        <li>\n"
    "            <a href=\"/logo-transparent.png\">/logo-transparent.png</a>\n"
    "        </li>\n"
    "        <li>\n"
    "            <a href=\"/dom.js\">/dom.js</a>\n"
    "        </li>\n"
    "        <li>\n"
    "            <a href=\"/account/\">/account/</a>\n"
    "        </li>\n"
    "        <li>\n"
    "            <a href=\"/style.css\">/style.css</a>\n"
    "        </li>\n"
    "    </ul>\n"
    "    </nav>\n"
    "</header>\n"
    "<H1>Home</H1>\n"
    "<P>Yay, homepage loads</P>\n"
    "<footer>\n"
    "    Caneka example site - <a href=\"https://caneka.org\">view caneka.org</a>\n"
    "</footer>\n"
    "</body>\n"
    "</html>\n"
    ;

static Object *getGenericData(MemCh *m, Route *rt){
    Object *data = Object_Make(m, ZERO);

    Table *atts = Table_Make(m);
    Table_Set(atts, Str_FromCstr(m, "title", ZERO), 
        Str_FromCstr(m, "Example Title", ZERO));
    NodeObj *page = Object_Make(m, TYPE_NODEOBJ);
    Object_SetPropByIdx(page, NODEOBJ_PROPIDX_ATTS, atts);
    Object *config = Object_Make(m, ZERO);
    Object_Set(config, Str_CstrRef(m, "page"), page);
    Object_Set(data, Str_CstrRef(m, "config"), config);

    StrVec *navPath = IoUtil_GetAbsVec(m,
        Str_FromCstr(m, "./examples/web-server/pages/nav.config", ZERO));

    Object *nav = Nav_TableFromPath(m, rt, navPath);
    Object_Set(data, Str_CstrRef(m, "nav"), nav);
    return data;
}

status WwwRoute_Tests(MemCh *gm){
    DebugStack_Push(NULL, ZERO);
    void *args[5];
    status r = READY;
    MemCh *m = MemCh_Make();

    StrVec *path = IoUtil_GetAbsVec(m,
        Str_FromCstr(m, "./examples/web-server/pages/public", ZERO));
    Route *rt = Route_From(m, path);

    path = StrVec_From(m, Str_FromCstr(m, "/tests", ZERO));
    IoUtil_Annotate(m, path);
    Route *account = Object_ByPath(rt, path, NULL, SPAN_OP_GET);

    Hashed *h = Object_GetByIdx(account, ROUTE_PROPIDX_MIME);
    args[0] = h->value;
    args[1] = NULL;
    r |= Test(Equals(h->value, Str_FromCstr(m, "text/html", ZERO)), 
        "account index page is mime type text/html, have @", args);
    h = Object_GetByIdx(account, ROUTE_PROPIDX_TYPE);
    args[0] = h->value;
    args[1] = NULL;
    r |= Test(Equals(h->value, Str_FromCstr(m, "body", ZERO)), 
        "account index page is type html, have @", args);

    path = StrVec_From(m, Str_FromCstr(m, "/stats", ZERO));
    IoUtil_Annotate(m, path);
    Route *profile = Object_ByPath(rt, path, NULL, SPAN_OP_GET);
    h = Object_GetByIdx(profile, ROUTE_PROPIDX_MIME);
    r |= Test(Equals(h->value, Str_FromCstr(m, "text/html", ZERO)),
        "profile index page is mime type text/html", NULL);

    h = Object_GetByIdx(profile, ROUTE_PROPIDX_TYPE);
    r |= Test(Equals(h->value, Str_FromCstr(m, "templ", ZERO)),
        "profile index page is mime type templ", NULL);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}

status WwwRouteTempl_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    void *args[5];
    status r = READY;
    MemCh *m = MemCh_Make();

    Route *rt = Route_Make(m);
    StrVec *pagesPath = IoUtil_GetAbsVec(m,
        Str_CstrRef(m, "./examples/web-server/pages/public"));
    StrVec *pageAbs = IoUtil_AbsVec(m, pagesPath);
    Route_Collect(rt, pageAbs);

    Route *inc = Route_Make(m);
    StrVec *incPath = IoUtil_GetAbsVec(m,
        Str_CstrRef(m, "./examples/web-server/pages/inc"));
    StrVec *incAbs = IoUtil_AbsVec(m, incPath);
    Route_Collect(inc, incAbs);

    Object *data = getGenericData(m, rt);

    args[0] = data;
    args[1] = NULL;
    Out("^y.data @^0\n", args);
    exit(1);

    Str *now = MicroTime_ToStr(m, MicroTime_Now());
    Object_Set(data, Str_CstrRef(m, "now"), now);
    StrVec *title = StrVec_From(m, Str_CstrRef(m, "The Title of the Master Page"));
    Object_Set(data, Str_CstrRef(m, "title"), title);

    StrVec *path = StrVec_From(m, Str_CstrRef(m,
        "./examples/web-server/pages/inc/header.templ"));
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
    r |= TestShow(Equals(out, expected),
        "Templ from Route has expected output",
        "Templ output does not match @", args);

    /* login.templ no header with user */
    path = StrVec_From(m, Str_CstrRef(m, "/stats"));
    IoUtil_Annotate(m, path);
    Route *handler = Object_ByPath(rt, path, NULL, SPAN_OP_GET);

    /* login.templ no user with header and footer */
    MemBookStats st;
    MemBook_GetStats(m, &st);

    data = getGenericData(m, rt);
    Object *stats = Object_Make(m, ZERO);
    Object_Set(stats, Str_FromCstr(m, "uptime", ZERO),
        MicroTime_ToStr(m, MicroTime_Now()));
    Object_Set(data, Str_FromCstr(m, "stats", ZERO), stats);

    bf = Buff_Make(m, ZERO);
    Route_Handle(handler, bf, data, NULL);
    
    expected = Str_FromCstr(m, loginCstr, ZERO);
    args[0] = bf->v;
    args[1] = NULL;
    r |= TestShow(Equals(expected, bf->v),
        "Handler: Expected template value with no header and a user name", 
        "Handler: Expected template value with no header and a user name: $", args);

    Route *header = Object_ByPath(inc,
        StrVec_From(bf->m, Str_FromCstr(bf->m, "header", ZERO)), NULL, SPAN_OP_GET);
    Route *footer = Object_ByPath(inc,
        StrVec_From(bf->m, Str_FromCstr(bf->m, "footer", ZERO)),
            NULL, SPAN_OP_GET);

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

    expected = Str_FromCstr(m, loginNoUserCstr, ZERO);
    args[0] = dest->v;
    args[1] = NULL;
    r |= TestShow(Equals(expected, dest->v), 
        "Footer: Expected template value with no user object and a header",
        "Footer: Expected template value with no user object and a header: $", 
    args);

    /* index.fmt with header */
    data = getGenericData(m, rt);
    stats = Object_Make(m, ZERO);
    Object_Set(stats, Str_FromCstr(m, "uptime", ZERO),
        MicroTime_ToStr(m, MicroTime_Now()));

    Object *mem = Object_Make(m, ZERO);
    Object_Set(mem, Str_FromCstr(m, "mem-total", ZERO),
        Str_MemCount(m, st.pageIdx * PAGE_SIZE));
    Object_Set(mem, Str_FromCstr(m, "mem-details", ZERO), Map_ToTable(m, &st));
    Object_Set(stats, Str_FromCstr(m, "mem", ZERO), mem);
    Object_Set(data, Str_FromCstr(m, "stats", ZERO), stats);

    bf = Buff_Make(m, ZERO);
    Route_Handle(header, bf, data, NULL);

    path = StrVec_From(m, Str_CstrRef(m, "/"));
    IoUtil_Annotate(m, path);
    handler = Object_ByPath(rt, path, NULL, SPAN_OP_GET);

    Route_Handle(handler, bf, data, NULL);
    dest = Buff_Make(m, ZERO);
    Buff_Pipe(dest, bf);

    bf = Buff_Make(m, ZERO);
    Route_Handle(footer, bf, data, NULL);
    Buff_Pipe(dest, bf);
    
    expected = Str_FromCstr(m, homeCstr, ZERO);
    args[0] = dest->v;
    args[1] = NULL;
    r |= TestShow(Equals(expected, dest->v),
        "Expected fmt value with user name", 
        "Expected fmt value with user name: $", 
    args);

    r &= ~ERROR;

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}

status WwwRouteMime_Tests(MemCh *gm){
    DebugStack_Push(NULL, ZERO);
    void *args[6];
    status r = READY;
    MemCh *m = MemCh_Make();

    Route *rt = Route_Make(m);

    StrVec *path = IoUtil_GetAbsVec(m, Str_CstrRef(m, "./examples/web-server/pages/public"));
    Route_Collect(rt, path);

    StrVec *key = StrVec_From(m, Str_FromCstr(m, "/style.css", STRING_COPY));
    IoUtil_Annotate(m, key);
    Path_JoinBase(m, key);

    Route *subRt = Object_ByPath(rt, key, NULL, SPAN_OP_GET);
    Buff *bf = Buff_Make(m, ZERO);
    Route_Handle(subRt, bf, NULL, NULL);

    Buff *dest = Buff_Make(m, ZERO);
    Buff_Pipe(dest, bf);

    Str *pathS = IoUtil_GetAbsPath(m,
        Str_FromCstr(m, "./examples/web-server/pages/public/style.css", ZERO));
    StrVec *expected = File_ToVec(m, pathS);

    r |= Test(Equals(dest->v, expected), "Content from Buff piped from route matches reading file directly", NULL);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
