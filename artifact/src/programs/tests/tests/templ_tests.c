#include <external.h>
#include <caneka.h>

static char *keyTestContent = ""
    "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<meta charset=\"utf-8\">\n<title>My Fancy Page</title>\n<link href=\"style.css\" rel=\"stylesheet\" />\n  <meta \n    name=\"viewport\"\n    content=\"width=device-width,maximum-scale=1.0,initial-scale=1.0,minimum-scale=1.0,user-scalable=yes,shrink-to-fit=no\"\n  >\n</head>\n<body>\n<header>\n    <div class=\"section\">\n        <div class=\"logo-blocker\"></div>\n        <a class=\"hero\" href=\"/\">\n        <h1>caneka</h1>\n        </a>\n        <h2>The Anti-Syntax Runtime</h2>\n        <span class=\"menu-action\" onclick=\"menu()\">=</span>\n    </div>\n    <nav>\n  <ul>\n    <li>item one</li>\n    <li>item two</li>\n  </ul>\n</nav>\n    <script type=\"text/javascript\" src=\"menu.js\"></script>\n</header>\n"
"";

static char *logicTestContent = ""
"<div class=\"nav-wrapper\" id=\"nav-wrapper\">\n"
"    <div class=\"section\">\n"
"        <div class=\"logo-blocker\"></div>\n"
"        <a class=\"hero\" href=\"/\">\n"
"        <h1>caneka</h1>\n"
"        </a>\n"
"        <h2>The Anti-Syntax Runtime</h2>\n"
"        <span class=\"menu-action\" onclick=\"menuClose()\">X</span>\n"
"    </div>\n"
"    <section>\n"
"    <nav>\n"
"        <ul><a href=\"/\" class=\"active\">Home</a></ul>\n"
"        <ul><span class=\"fo-link\">Links</span>\n"
"            <ul>\n"
"                <li><a href=\"base/mem\">/base/mem.html</a></li>\n"
"                <li><a href=\"base/str\">/base/str.html</a></li>\n"
"                <li><a href=\"base/io\">/base/io.html</a></li>\n"
"                <li><a href=\"base/suite\">/base/suite.html</a></li>\n"
"            </ul>\n"
"        </ul>\n"
"    </nav>\n"
"    </section>\n"
"</div>\n"
"";

status Templ_Tests(MemCh *gm){
    status r = READY;
    MemCh *m = MemCh_Make();

    Str *path = IoUtil_GetAbsPath(m, Str_CstrRef(m, "./docs/html/header.html"));
    File *f = File_Make(m, path, NULL, STREAM_STRVEC);
    File_Open(f);
    File_Read(f, FILE_READ_MAX);
    File_Close(f);

    Cursor *curs = File_GetCurs(f);
    CashCtx *ctx = CashCtx_FromCurs(m, curs, NULL);
    
    r |= Test(ctx->type.state & SUCCESS,
            "Templ: Roebling finished with state SUCCESS for Cash with keys", 
        NULL);

    OrdTable *data = OrdTable_Make(m);
    StrVec *menu = StrVec_From(m,
        Str_CstrRef(m, "<nav>\n  <ul>\n    <li>item one</li>\n    <li>item two</li>\n  </ul>\n</nav>"));
    Str *title = Str_CstrRef(m, "My Fancy Page");
    OrdTable_Set(data, (Abstract *)Str_CstrRef(m, "menu"), (Abstract *)menu);
    OrdTable_Set(data, (Abstract *)Str_CstrRef(m, "title"), (Abstract *)title);

    Stream *sm = Stream_MakeStrVec(m);
    
    Templ *templ = (Templ *)Templ_Make(m, ctx->it.p, data);
    i64 total = Templ_ToS(templ, sm);

    Str *expected = Str_CstrRef(m, keyTestContent);
    r |= Test(Equals((Abstract *)expected, (Abstract *)sm->dest.curs->v), "Temple key value test has expected content", NULL);

    MemCh_Free(m);
    return r;
}

status TemplLogic_Tests(MemCh *gm){
    status r = READY;
    MemCh *m = MemCh_Make();

    Str *path = IoUtil_GetAbsPath(m, Str_CstrRef(m, "./docs/html/nav.html"));
    File *f = File_Make(m, path, NULL, STREAM_STRVEC);
    File_Open(f);
    File_Read(f, FILE_READ_MAX);
    File_Close(f);

    Cursor *curs = File_GetCurs(f);
    CashCtx *ctx = CashCtx_FromCurs(m, curs, NULL);
    
    r |= Test(ctx->type.state & SUCCESS,
            "Templ: Roebling finished with state SUCCESS for Cash with keys", 
        NULL);

    OrdTable *data = OrdTable_Make(m);
    OrdTable *menuItems = OrdTable_Make(m);
    OrdTable_Set(menuItems, (Abstract *)Str_CstrRef(m, "base/mem"),
        (Abstract *)Str_CstrRef(m, "/base/mem.html"));
    OrdTable_Set(menuItems, (Abstract *)Str_CstrRef(m, "base/str"),
        (Abstract *)Str_CstrRef(m, "/base/str.html"));
    OrdTable_Set(menuItems, (Abstract *)Str_CstrRef(m, "base/io"),
        (Abstract *)Str_CstrRef(m, "/base/io.html"));
    OrdTable_Set(menuItems, (Abstract *)Str_CstrRef(m, "base/suite"),
        (Abstract *)Str_CstrRef(m, "/base/suite.html"));

    OrdTable_Set(data, (Abstract *)Str_CstrRef(m, "menu-items"), (Abstract *)menuItems);

    Stream *sm = Stream_MakeStrVec(m);
    
    Templ *templ = (Templ *)Templ_Make(m, ctx->it.p, data);
    i64 total = Templ_ToS(templ, sm);

    Str *expected = Str_CstrRef(m, logicTestContent);
    r |= Test(Equals((Abstract *)expected, (Abstract *)sm->dest.curs->v), "Temple key value test has expected content", NULL);


    MemCh_Free(m);
    return r;
}

