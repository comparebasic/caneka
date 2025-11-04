#include <external.h>
#include <caneka.h>

static char *cstr = ""
    "<!DOCTYPE html>\n"
    "<html lang=\"en\">\n"
    "<head>\n"
    "<meta charset=\"utf-8\">\n"
    "<title>Compare Basic - The Title of the Master Page</title>\n"
    "<meta property=\"og:title\" content=\"The Title of the Master Page - Compare Basic\" />\n"
    "<meta property=\"og:description\" content=\"The proximity you have to the things you value defines your quality of life.\"/>\n"
    "<meta property=\"og:image\" content=\"/triangle-diagram.png\" />\n"
    "<script src=\"/dom.js\" type=\"text/javascript\"></script>\n"
    "<link rel=\"stylesheet\" href=\"/style.css\" />\n"
    "  <meta \n"
    "    name=\"viewport\"\n"
    "    content=\"width=device-width,maximum-scale=1.0,initial-scale=1.0,minimum-scale=1.0,user-scalable=yes,shrink-to-fit=no\"\n"
    "  >\n"
    "</head>\n"
    "<body>\n"
    "<header>\n"
    "    <div class=\"inner\">\n"
    "        <div class=\"bread-crumbs\">\n"
    "        <ul>\n"
    "            <li>\n"
    "                <a href=\"/logo-transparent.png\">/logo-transparent.png</a>\n"
    "            </li>\n"
    "            <li>\n"
    "                <a href=\"/dom.js\">/dom.js</a>\n"
    "            </li>\n"
    "            <li>\n"
    "                <a href=\"/account/\">/account/</a>\n"
    "            </li>\n"
    "            <li>\n"
    "                <a href=\"/style.css\">/style.css</a>\n"
    "            </li>\n"
    "        </ul>\n"
    "        <div style=\"clear\"></div>\n"
    "        </div>\n"
    "    </div>\n"
    "</header>\n"
;

status WwwRoute_Tests(MemCh *gm){
    DebugStack_Push(NULL, ZERO);
    Abstract *args[5];
    status r = READY;
    MemCh *m = MemCh_Make();

    Route *rt = Route_Make(m);

    StrVec *path = IoUtil_GetAbsVec(m, Str_CstrRef(m, "./examples/web-server/pages/public"));
    Route_Collect(rt, path);

    path = StrVec_From(m, Str_FromCstr(m, "/account/", ZERO));
    IoUtil_Annotate(m, path);
    Route *account = Object_ByPath(rt, path, NULL, SPAN_OP_GET);
    Hashed *h = Object_GetByIdx(account, ROUTE_PROPIDX_MIME);
    r |= Test(Equals((Abstract *)h->value, (Abstract *)Str_FromCstr(m, "text/html", ZERO)), 
        "account index page is mime type text/html", NULL);
    h = Object_GetByIdx(account, ROUTE_PROPIDX_TYPE);
    r |= Test(Equals((Abstract *)h->value, (Abstract *)Str_FromCstr(m, "html", ZERO)), 
        "account index page is type html", NULL);

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

    StrVec *path = StrVec_From(m, Str_CstrRef(m,
        "./examples/web-server/pages/inc/header.templ"));
    StrVec *abs = IoUtil_AbsVec(m, path);

    Route *rt = (Object *)Route_Make(m);
    StrVec *home = StrVec_From(m, Str_CstrRef(m, "/"));
    Object_SetPropByIdx(rt, ROUTE_PROPIDX_PATH, (Abstract *)home);

    StrVec *navPath = StrVec_From(m, Str_CstrRef(m,
        "./examples/web-server/pages/public"));
    StrVec *navAbs = IoUtil_AbsVec(m, navPath);
    Route_Collect(rt, navAbs);

    Object *data = Object_Make(m, ZERO);
    Object_Set(data, (Abstract *)Str_CstrRef(m, "menu-items"), (Abstract *)rt);
    Str *now = MicroTime_ToStr(m, MicroTime_Now());
    Object_Set(data, (Abstract *)Str_CstrRef(m, "now"), (Abstract *)now);
    StrVec *title = StrVec_From(m, Str_CstrRef(m, "The Title of the Master Page"));
    Object_Set(data, (Abstract *)Str_CstrRef(m, "title"), (Abstract *)title);

    StrVec *v = File_ToVec(m, StrVec_Str(m, abs));

    Cursor *curs = Cursor_Make(m, v);
    TemplCtx *ctx = TemplCtx_FromCurs(m, curs, NULL);
    Templ *templ = (Templ *)Templ_Make(m, ctx->it.p);
    Templ_Prepare(templ);

    StrVec *out = StrVec_Make(m);
    Buff *bf = Buff_From(m, out);
    Templ_ToS(templ, bf, (Abstract *)data, NULL);

    Str *expected = Str_FromCstr(m, cstr, ZERO);
    args[0] = (Abstract *)expected;
    args[1] = (Abstract *)I64_Wrapped(m, out->total);
    args[2] = NULL;
    Out("^p.expected\n$\n\nout\n$^0", args);

    out->type.state |= DEBUG;
    r |= Test(Equals((Abstract *)out, (Abstract *)expected),
        "Templ from Route has expected output", NULL);

    r |= ERROR;
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
