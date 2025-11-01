#include <external.h>
#include <caneka.h>

char *cstr = ""
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

status TemplRoute_Tests(MemCh *gm){
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

    out->type.state |= DEBUG;
    r |= Test(Equals((Abstract *)out, (Abstract *)Str_FromCstr(m, cstr, ZERO)),
        "Templ from Route has expected output", NULL);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
