#include <external.h>
#include <caneka.h>

static char *keyTestContent = ""
"<!DOCTYPE html>\n"
"<html lang=\"en\">\n"
"<head>\n"
"<meta charset=\"utf-8\">\n"
"<title>My Fancy Page</title>\n"
"  <meta \n"
"    name=\"viewport\"\n"
"    content=\"width=device-width,maximum-scale=1.0,initial-scale=1.0,minimum-scale=1.0,user-scalable=yes,shrink-to-fit=no\"\n"
"  >\n"
"</head>\n"
"<body>\n"
"<header>\n"
"    <nav>\n"
"    <ul>\n"
"        <li>item one</li>\n"
"        <li>item two</li>\n"
"    </ul>\n"
"    </nav>\n"
"</header>\n"
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

status TemplCtx_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    MemCh *m = MemCh_Make();
    Str *s = NULL; 
    Fetcher *fch = NULL;
    FetchTarget *tg = NULL;

    Str *path = IoUtil_GetAbsPath(m, Str_CstrRef(m, "examples/example.templ"));
    File *f = File_Make(m, path, NULL, STREAM_STRVEC);
    File_Open(f);
    File_Read(f, FILE_READ_MAX);
    File_Close(f);

    Cursor *curs = File_GetCurs(f);
    TemplCtx *ctx = TemplCtx_FromCurs(m, curs, NULL);
    
    r |= Test(ctx->type.state & SUCCESS,
            "Roebling finished with state SUCCESS with keys", 
        NULL);

    Span *expected = Span_Make(m);
    char *cstr = "<ul>\n    ";
    s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)+1, ZERO);
    StrVec *v = StrVec_From(m, s);
    Span_Add(expected, (Abstract *)v);

    fch = Fetcher_Make(m);
    fch->type.state |= FETCHER_FOR;
    cstr = "items";
    s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)+1, ZERO);
    tg = FetchTarget_MakeKey(m, s);
    Span_Add(fch->val.targets, (Abstract *)tg);
    cstr = "menu";
    s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)+1, ZERO);
    tg = FetchTarget_MakeKey(m, s);
    Span_Add(fch->val.targets, (Abstract *)tg);
    tg = FetchTarget_MakeIter(m);
    Span_Add(fch->val.targets, (Abstract *)tg);
    Span_Add(expected, (Abstract *)fch);

    cstr = "\n        <li><a href=\"";
    s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)+1, ZERO);
    v = StrVec_From(m, s);
    Span_Add(expected, (Abstract *)v);

    fch = Fetcher_Make(m);
    fch->type.state = FETCHER_VAR;
    cstr = "fullName";
    s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)+1, ZERO);
    tg = FetchTarget_MakeAtt(m, s);
    Span_Add(fch->val.targets, (Abstract *)tg);
    Span_Add(expected, (Abstract *)fch);

    cstr = "\">";
    s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)+1, ZERO);
    v = StrVec_From(m, s);
    Span_Add(expected, (Abstract *)v);

    fch = Fetcher_Make(m);
    fch->type.state = FETCHER_VAR;
    cstr = "fullName";
    cstr = "name";
    s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)+1, ZERO);
    tg = FetchTarget_MakeAtt(m, s);
    Span_Add(fch->val.targets, (Abstract *)tg);
    Span_Add(expected, (Abstract *)fch);

    cstr = "</a>\n    ";
    s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)+1, ZERO);
    v = StrVec_From(m, s);
    Span_Add(expected, (Abstract *)v);

    fch = Fetcher_Make(m);
    fch->type.state = FETCHER_END;
    Span_Add(expected, (Abstract *)fch);

    cstr = "\n</ul>\n";
    s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)+1, ZERO);
    v = StrVec_From(m, s);
    Span_Add(expected, (Abstract *)v);

    Abstract *args[] = {
        (Abstract *)expected,
        (Abstract *)ctx->it.p,
        NULL
    };

    r |= TestShow(Exact((Abstract *)expected, (Abstract *)ctx->it.p), 
        "Expected content found in example templ", 
        "Mismatch in content found in example templ, expected:\n$\nhave:\n $", args);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}

status Templ_Tests(MemCh *gm){
    status r = READY;
    MemCh *m = MemCh_Make();

    Str *path = IoUtil_GetAbsPath(m, Str_CstrRef(m, "./examples/header.html"));
    File *f = File_Make(m, path, NULL, STREAM_STRVEC);
    File_Open(f);
    File_Read(f, FILE_READ_MAX);
    File_Close(f);

    Cursor *curs = File_GetCurs(f);
    TemplCtx *ctx = TemplCtx_FromCurs(m, curs, NULL);
    
    r |= Test(ctx->type.state & SUCCESS,
            "Templ: Roebling finished with state SUCCESS with keys", 
        NULL);

    OrdTable *data = OrdTable_Make(m);
    StrVec *menu = StrVec_From(m,
        Str_CstrRef(m, "<nav>\n    <ul>\n        <li>item one</li>\n        <li>item two</li>\n    </ul>\n    </nav>"));
    Str *title = Str_CstrRef(m, "My Fancy Page");
    OrdTable_Set(data, (Abstract *)Str_CstrRef(m, "menu"), (Abstract *)menu);
    OrdTable_Set(data, (Abstract *)Str_CstrRef(m, "title"), (Abstract *)title);

    Stream *sm = Stream_MakeStrVec(m);
    
    Templ *templ = (Templ *)Templ_Make(m, ctx->it.p);
    i64 total = Templ_ToS(templ, sm, data, NULL);

    Str *expected = Str_CstrRef(m, keyTestContent);
    Abstract *args[] = {
        (Abstract *)expected,
        (Abstract *)sm->dest.curs->v,
        NULL
    };
    expected->type.state |= DEBUG;
    r |= TestShow(Equals((Abstract *)expected, (Abstract *)sm->dest.curs->v), 
        "Templ key value test has expected content", 
        "Templ key value mismatch test has expected content, expected:\n&\n\nhave:\n&", 
        args);

    MemCh_Free(m);
    return r;
}

status TemplLogic_Tests(MemCh *gm){
    status r = READY;
    MemCh *m = MemCh_Make();

    Str *path = IoUtil_GetAbsPath(m, Str_CstrRef(m, "./examples/nav.html"));
    File *f = File_Make(m, path, NULL, STREAM_STRVEC);
    File_Open(f);
    File_Read(f, FILE_READ_MAX);
    File_Close(f);

    Cursor *curs = File_GetCurs(f);
    TemplCtx *ctx = TemplCtx_FromCurs(m, curs, NULL);
    
    r |= Test(ctx->type.state & SUCCESS,
            "Templ: Roebling finished with state SUCCESS with keys", 
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
    
    Templ *templ = (Templ *)Templ_Make(m, ctx->it.p);
    i64 total = Templ_ToS(templ, sm, data, NULL);

    Str *expected = Str_CstrRef(m, logicTestContent);
    Abstract *args[] = {
        (Abstract *)expected,
        (Abstract *)sm->dest.curs->v,
        NULL
    };
    r |= TestShow(Equals((Abstract *)expected, (Abstract *)sm->dest.curs->v), 
        "Temple key value test has expected content", 
        "Temple key value test mismatch, expected @\nhave\n@", 
        args);


    MemCh_Free(m);
    return r;
}

