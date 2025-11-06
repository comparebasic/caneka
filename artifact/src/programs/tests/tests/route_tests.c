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
    ;

static Object *getGenericData(MemCh *m, Route *rt){
    Object *data = Object_Make(m, ZERO);

    Table *atts = Table_Make(m);
    Table_Set(atts,
        (Abstract *)Str_FromCstr(m, "title", ZERO), 
        (Abstract *)Str_FromCstr(m, "Example Title", ZERO));
    NodeObj *page = Object_Make(m, TYPE_NODEOBJ);
    Object_SetPropByIdx(page, NODEOBJ_PROPIDX_ATTS, (Abstract *)atts);
    Object *config = Object_Make(m, ZERO);
    Object_Set(config, (Abstract *)Str_CstrRef(m, "page"), (Abstract *)page);
    Object_Set(data, (Abstract *)Str_CstrRef(m, "config"), (Abstract *)config);
    
    Object *nav = Object_Make(m, ZERO);
    Object_Set(nav, (Abstract *)Str_CstrRef(m, "pages"), (Abstract *)rt);
    Object_Set(data, (Abstract *)Str_CstrRef(m, "nav"), (Abstract *)nav);
    return data;
}

status WwwRoute_Tests(MemCh *gm){
    DebugStack_Push(NULL, ZERO);
    Abstract *args[5];
    status r = READY;
    MemCh *m = MemCh_Make();

    Route *rt = Route_Make(m);
    StrVec *path = IoUtil_GetAbsVec(m,
        Str_CstrRef(m, "./examples/web-server/pages/public"));
    Route_Collect(rt, path);

    path = StrVec_From(m, Str_FromCstr(m, "/account/", ZERO));
    IoUtil_Annotate(m, path);
    Route *account = Object_ByPath(rt, path, NULL, SPAN_OP_GET);

    Hashed *h = Object_GetByIdx(account, ROUTE_PROPIDX_MIME);
    args[0] = h->value;
    args[1] = NULL;
    r |= Test(Equals((Abstract *)h->value, (Abstract *)Str_FromCstr(m, "text/html", ZERO)), 
        "account index page is mime type text/html, have $", args);
    h = Object_GetByIdx(account, ROUTE_PROPIDX_TYPE);
    args[0] = h->value;
    args[1] = NULL;
    r |= Test(Equals((Abstract *)h->value, (Abstract *)Str_FromCstr(m, "html", ZERO)), 
        "account index page is type html, have $", args);

    path = StrVec_From(m, Str_FromCstr(m, "/account/profile", ZERO));
    IoUtil_Annotate(m, path);
    Route *profile = Object_ByPath(rt, path, NULL, SPAN_OP_GET);
    h = Object_GetByIdx(profile, ROUTE_PROPIDX_MIME);
    r |= Test(Equals((Abstract *)h->value, (Abstract *)Str_FromCstr(m, "text/html", ZERO)),
        "profile index page is mime type text/html", NULL);
    h = Object_GetByIdx(profile, ROUTE_PROPIDX_TYPE);
    r |= Test(Equals((Abstract *)h->value, (Abstract *)Str_FromCstr(m, "templ", ZERO)),
        "profile index page is mime type templ", NULL);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}

status WwwRouteTempl_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    Abstract *args[5];
    status r = READY;
    MemCh *m = MemCh_Make();

    Route *rt = Route_Make(m);
    StrVec *navPath = IoUtil_GetAbsVec(m,
        Str_CstrRef(m, "./examples/web-server/pages/public"));
    StrVec *navAbs = IoUtil_AbsVec(m, navPath);
    Route_Collect(rt, navAbs);

    Route *inc = Route_Make(m);
    StrVec *incPath = IoUtil_GetAbsVec(m,
        Str_CstrRef(m, "./examples/web-server/pages/inc"));
    StrVec *incAbs = IoUtil_AbsVec(m, incPath);
    Route_Collect(inc, incAbs);

    args[0] = (Abstract *)inc;
    args[1] = NULL;
    Out("^p.Include Routes @^0\n", args);

    Object *data = getGenericData(m, rt);

    Str *now = MicroTime_ToStr(m, MicroTime_Now());
    Object_Set(data, (Abstract *)Str_CstrRef(m, "now"), (Abstract *)now);
    StrVec *title = StrVec_From(m, Str_CstrRef(m, "The Title of the Master Page"));
    Object_Set(data, (Abstract *)Str_CstrRef(m, "title"), (Abstract *)title);

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

    Templ_ToS(templ, bf, (Abstract *)data, NULL);
    Str *expected = Str_FromCstr(m, cstr, ZERO);
    args[0] = (Abstract *)out;
    args[1] = (Abstract *)data;
    args[2] = NULL;
    r |= TestShow(Equals((Abstract *)out, (Abstract *)expected),
        "Templ from Route has expected output",
        "Templ output does not match @", args);

    /* login.templ no header with user */
    path = StrVec_From(m, Str_CstrRef(m,
        "/login"));
    IoUtil_Annotate(m, path);
    Route *handler = Object_ByPath(rt, path, NULL, SPAN_OP_GET);


    data = getGenericData(m, rt);
    Object *user = Object_Make(m, ZERO);
    Object_Set(user,
        (Abstract *)Str_FromCstr(m, "name", ZERO),
        (Abstract *)Str_FromCstr(m, "Fancy Pantsy", ZERO));
    Object_Set(data,
       (Abstract *) Str_FromCstr(m, "user", ZERO),
       (Abstract *)user);

    Route *header = Object_ByPath(inc,
        StrVec_From(bf->m, Str_FromCstr(bf->m, "header", ZERO)),
            NULL, SPAN_OP_GET);

    bf = Buff_Make(m, ZERO);
    Route_Handle(handler, bf, data, NULL);
    
    expected = Str_FromCstr(m, loginCstr, ZERO);
    args[0] = (Abstract *)bf->v;
    args[1] = NULL;
    r |= TestShow(Equals((Abstract *)expected, (Abstract *)bf->v),
        "Expected template value with no header and a user name", 
        "Expected template value with no header and a user name: $", 
    args);

    Route *footer = Object_ByPath(inc,
        StrVec_From(bf->m, Str_FromCstr(bf->m, "footer", ZERO)),
            NULL, SPAN_OP_GET);

    /* login.templ no user with header and footer */

    args[0] = (Abstract *)data;
    args[1] = NULL;
    Out("^p.Header with data: @^0\n", args);

    bf = Buff_Make(m, ZERO);
    Route_Handle(header, bf, data, NULL);

    Route_Handle(handler, bf, Object_Make(m, ZERO), NULL);
    Buff *dest = Buff_Make(m, ZERO);
    Buff_Pipe(dest, bf);

    bf = Buff_Make(m, ZERO);
    Route_Handle(footer, bf, data, NULL);
    Buff_Pipe(dest, bf);

    expected = Str_FromCstr(m, loginNoUserCstr, ZERO);
    args[0] = (Abstract *)dest->v;
    args[1] = NULL;
    r |= TestShow(Equals((Abstract *)expected, (Abstract *)bf->v), 
        "Expected template value with no user object and a header",
        "Expected template value with no user object and a header: $", 
    args);

    /* index.fmt with header */
    data = getGenericData(m, rt);
    user = Object_Make(m, ZERO);
    Object_Set(user,
        (Abstract *)Str_FromCstr(m, "name", ZERO),
        (Abstract *)Str_FromCstr(m, "Fancy Pantsy", ZERO));
    Object_Set(data,
       (Abstract *) Str_FromCstr(m, "user", ZERO),
       (Abstract *)user);

    args[0] = (Abstract *)data;
    args[1] = NULL;
    Out("^p.Header with data: @^0\n", args);

    bf = Buff_Make(m, ZERO);
    Route_Handle(header, bf, data, NULL);

    path = StrVec_From(m, Str_CstrRef(m, "/"));
    IoUtil_Annotate(m, path);
    handler = Object_ByPath(rt, path, NULL, SPAN_OP_GET);

    args[0] = (Abstract *)data;
    args[1] = NULL;
    Out("^p.Fmt with data: @^0\n", args);

    Route_Handle(handler, bf, data, NULL);
    dest = Buff_Make(m, ZERO);
    Buff_Pipe(dest, bf);

    bf = Buff_Make(m, ZERO);
    Route_Handle(footer, bf, data, NULL);
    Buff_Pipe(dest, bf);
    
    expected = Str_FromCstr(m, "", ZERO);
    args[0] = (Abstract *)dest->v;
    args[1] = NULL;
    r |= TestShow(Equals((Abstract *)expected, (Abstract *)bf->v),
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
    Abstract *args[6];
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

    r |= Test(Equals((Abstract *)dest->v, (Abstract *)expected), "Content from Buff piped from route matches reading file directly", NULL);

    r |= ERROR;
    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
