#include <external.h>
#include <caneka.h>
#include <test_module.h>

static char *keyTestContent = ""
"<h1>My Fancy Page</h1>\n"
"<p>And here is the masterful list of menu items!</p>\n"
"<ul>\n"
"    <li><a href=\"/things/one\">One</a></li>\n"
"    <li><a href=\"/things/two\">Two</a></li>\n"
"    <li><a href=\"/things/three\">Three</a></li>\n"
"</ul>\n"
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

static NodeObj *navMake(MemCh *m){

    NodeObj *node = Inst_Make(m, TYPE_NODEOBJ);
    Table *coordTbl = Table_Make(m);

    StrVec *path = IoPath_From(m, S(m, "/docs/base/bytes/str"));
    Span *coords = Span_Make(m);
    WwwNav *item = Inst_Make(m, TYPE_WWW_NAV);
    Seel_Set(item, K(m, "url"), path);
    StrVec *name = Sv(m, "Str");
    Seel_Set(item, K(m, "name"), name);
    Inst_ByPath(node, path, item, SPAN_OP_SET, coords);
    Table_Set(coordTbl, name, coords);

    path = IoPath_From(m, S(m, "/docs/base/bytes/fmt"));
    coords = Span_Make(m);
    item = Inst_Make(m, TYPE_WWW_NAV);
    Seel_Set(item, K(m, "url"), path);
    name = Sv(m, "Fmt");
    Seel_Set(item, K(m, "name"), name);
    Inst_ByPath(node, path, item, SPAN_OP_SET, coords);
    Table_Set(coordTbl, name, coords);

    path = IoPath_From(m, S(m, "/docs/base/bytes/tos"));
    coords = Span_Make(m);
    item = Inst_Make(m, TYPE_WWW_NAV);
    Seel_Set(item, K(m, "url"), path);
    name = Sv(m, "ToS");
    Seel_Set(item, K(m, "name"), name);
    Inst_ByPath(node, path, item, SPAN_OP_SET, coords);
    Table_Set(coordTbl, name, coords);

    path = IoPath_From(m, S(m, "/docs/base/mem/span"));
    coords = Span_Make(m);
    item = Inst_Make(m, TYPE_WWW_NAV);
    Seel_Set(item, K(m, "url"), path);
    name = Sv(m, "Span");
    Seel_Set(item, K(m, "name"), name);
    Inst_ByPath(node, path, item, SPAN_OP_SET, coords);
    Table_Set(coordTbl, name, coords);

    path = IoPath_From(m, S(m, "/docs/base/mem/memch"));
    coords = Span_Make(m);
    item = Inst_Make(m, TYPE_WWW_NAV);
    Seel_Set(item, K(m, "url"), path);
    name = Sv(m, "MemCh");
    Seel_Set(item, K(m, "name"), name);
    Inst_ByPath(node, path, item, SPAN_OP_SET, coords);
    Table_Set(coordTbl, name, coords);

    Str *s = S(m, "Fmt");
    Span *crd = Table_Get(coordTbl, s);
    Iter *it = Iter_Make(m, NULL);
    NestSel_Init(it, node, crd);

    return node;
}

status TemplCtx_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    void *args[5];
    status r = READY;
    Str *s = NULL; 
    Fetcher *fch = NULL;
    FetchTarget *tg = NULL;

    Str *path = IoUtil_GetAbsPath(m, Str_CstrRef(m, "examples/example.templ"));
    StrVec *content = File_ToVec(m, path);

    Cursor *curs = Cursor_Make(m, content);
    TemplCtx *ctx = TemplCtx_FromCurs(m, curs, NULL);
    
    r |= Test(ctx->type.state & SUCCESS,
            "Roebling finished with state SUCCESS with keys", NULL);

    Span *expected = Span_Make(m);
    char *cstr = "<h1>";
    s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)+1, ZERO);
    StrVec *v = StrVec_From(m, s);
    Span_Add(expected, v);

    fch = Fetcher_Make(m);
    fch->type.state |= FETCHER_VAR;
    cstr = "title";
    s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)+1, ZERO);
    tg = FetchTarget_MakeKey(m, s);
    Span_Add(fch->val.targets, tg);
    Span_Add(expected, fch);

    cstr = "</h1>\n<p>";
    s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)+1, ZERO);
    v = StrVec_From(m, s);
    Span_Add(expected, v);

    fch = Fetcher_Make(m);
    fch->type.state |= FETCHER_VAR;
    cstr = "para";
    s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)+1, ZERO);
    tg = FetchTarget_MakeKey(m, s);
    Span_Add(fch->val.targets, tg);
    Span_Add(expected, fch);

    cstr = "</p>\n<ul>\n";
    s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)+1, ZERO);
    v = StrVec_From(m, s);
    Span_Add(expected, v);

    fch = Fetcher_Make(m);
    fch->type.state |= FETCHER_FOR;
    cstr = "items";
    s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)+1, ZERO);
    tg = FetchTarget_MakeKey(m, s);
    Span_Add(fch->val.targets, tg);
    cstr = "menu";
    s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)+1, ZERO);
    tg = FetchTarget_MakeKey(m, s);
    Span_Add(fch->val.targets, tg);
    tg = FetchTarget_MakeIter(m);
    Span_Add(fch->val.targets, tg);
    Span_Add(expected, fch);

    fch = Fetcher_Make(m);
    fch->type.state |= FETCHER_WITH;
    cstr = "value";
    s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)+1, ZERO);
    tg = FetchTarget_MakeAtt(m, s);
    Span_Add(fch->val.targets, tg);
    Span_Add(expected, fch);

    cstr = "    <li><a href=\"";
    s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)+1, ZERO);
    v = StrVec_From(m, s);
    Span_Add(expected, v);

    fch = Fetcher_Make(m);
    fch->type.state = FETCHER_VAR;
    cstr = "local";
    s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)+1, ZERO);
    tg = FetchTarget_MakeAtt(m, s);
    Span_Add(fch->val.targets, tg);
    Span_Add(expected, fch);

    cstr = "\">";
    s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)+1, ZERO);
    v = StrVec_From(m, s);
    Span_Add(expected, v);

    fch = Fetcher_Make(m);
    fch->type.state = FETCHER_VAR;
    cstr = "local";
    cstr = "name";
    s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)+1, ZERO);
    tg = FetchTarget_MakeAtt(m, s);
    Span_Add(fch->val.targets, tg);
    Span_Add(expected, fch);

    cstr = "</a></li>\n";
    s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)+1, ZERO);
    v = StrVec_From(m, s);
    Span_Add(expected, v);

    fch = Fetcher_Make(m);
    fch->type.state = FETCHER_END;
    Span_Add(expected, fch);

    fch = Fetcher_Make(m);
    fch->type.state = FETCHER_END;
    Span_Add(expected, fch);

    cstr = "</ul>\n";
    s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)+1, ZERO);
    v = StrVec_From(m, s);
    Span_Add(expected, v);

    args[0] = expected,
    args[1] = ctx->it.p,
    args[2] = NULL;

    r |= TestShow(Exact(expected, ctx->it.p), 
        "Expected content found in example templ", 
        "Mismatch in content found in example templ, expected:\n&\nhave:\n &", args);

    r &= ~ERROR;

    DebugStack_Pop();
    return r;
}

status Templ_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    void *args[5];
    status r = READY;
    TranspFile *tp = NULL;

    Str *path = IoUtil_GetAbsPath(m, Str_CstrRef(m, "./examples/example.templ"));
    StrVec *content = File_ToVec(m, path);

    Cursor *curs = Cursor_Make(m, content);
    TemplCtx *ctx = TemplCtx_FromCurs(m, curs, NULL);
    
    r |= Test(ctx->type.state & SUCCESS,
            "Templ: Roebling finished with state SUCCESS with keys", 
        NULL);

    if(r & ERROR){
        DebugStack_Pop();
        return r;
    }

    Span *seps = Span_Make(m);
    Span_Add(seps, B_Wrapped(m, (byte)'/', ZERO, MORE));

    Table *data = Table_Make(m);
    Table *menu = Table_Make(m);
    tp = TranspFile_Make(m);
    tp->local = StrVec_From(m, Str_CstrRef(m, "/things/one"));
    tp->name = StrVec_From(m, Str_CstrRef(m, "One"));
    Table_Set(menu, StrVec_From(m, Str_CstrRef(m, "one")), tp);
    tp = TranspFile_Make(m);

    tp->local = StrVec_From(m, Str_CstrRef(m, "/things/two"));
    tp->name = StrVec_From(m, Str_CstrRef(m, "Two"));
    Table_Set(menu, StrVec_From(m, Str_CstrRef(m, "two")), tp);
    tp = TranspFile_Make(m);
    
    tp->local = StrVec_From(m, Str_CstrRef(m, "/things/three"));
    tp->name = StrVec_From(m, Str_CstrRef(m, "Three"));
    Table_Set(menu, StrVec_From(m, Str_CstrRef(m, "three")), tp);

    Str *title = Str_CstrRef(m, "My Fancy Page");
    Str *para = Str_CstrRef(m, "And here is the masterful list of menu items!");
    Table_Set(data, Str_CstrRef(m, "title"), title);
    Table_Set(data, Str_CstrRef(m, "para"), para);

    StrVec *key = StrVec_From(m, Str_FromCstr(m, "items/menu", STRING_COPY));
    Path_Annotate(m, key, seps);

    Table_ByPath(data, key, Table_Ordered(m, menu), SPAN_OP_SET);

    Buff *bf = Buff_Make(m, ZERO);
    
    Templ *templ = (Templ *)Templ_Make(m, ctx->it.p);
    status result = Templ_Prepare(templ);

    args[0] = templ;
    args[1] = ctx->it.p;
    args[2] = templ->content.p;
    args[3] = NULL;
    r |= TestShow((result & PROCESSING),
        "Templ_Prepare has result PROCESSING",
        "Templ_Prepare did not finish properly @ & -> &", args);

    if(r & ERROR){
        r |= Test((r & ERROR) == 0, "Error found returning", NULL);
        DebugStack_Pop();
        return r;
    }

    i64 total = Templ_ToS(templ, bf, data, NULL);

    Str *expected = Str_CstrRef(m, keyTestContent);
    args[0] = expected;
    args[1] = bf->v;
    args[2] = NULL;

    r |= TestShow(Equals(expected, bf->v), 
        "Templ key value test has expected content", 
        "Templ key value mismatch test has expected content, expected:\n&\n\nhave:\n&", 
        args);

    DebugStack_Pop();
    return r;
}

status TemplNav_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    void *args[5];
    status r = READY;
    TranspFile *tp = NULL;

    Str *path = IoUtil_GetAbsPath(m, Str_CstrRef(m, "./examples/doc/nav.templ"));
    StrVec *content = File_ToVec(m, path);

    Cursor *curs = Cursor_Make(m, content);
    TemplCtx *ctx = TemplCtx_FromCurs(m, curs, NULL);
    Templ *templ = (Templ *)Templ_Make(m, ctx->it.p);
    
    r |= Test(ctx->type.state & SUCCESS,
            "Templ: Roebling finished with state SUCCESS with keys", 
        NULL);

    if(r & ERROR){
        DebugStack_Pop();
        return r;
    }

    NodeObj *nav = navMake(m);
    Inst *page = Inst_Make(m, TYPE_WWW_PAGE);
    Seel_Set(page, S(m, "nav"), nav); 
    Table *data = Table_Make(m);
    Table_Set(data, S(m, "page"), page);
    Buff *bf = Buff_Make(m, ZERO);

    status result = Templ_Prepare(templ);
    i64 total = Templ_ToS(templ, bf, data, NULL);

    args[0] = templ->content.p;
    args[1] = nav;
    args[2] = bf->v;
    args[3] = NULL;
    Out("^y.Templ->v &\n^c.@\n^p.$^0\n", args);

    r |= ERROR;

    DebugStack_Pop();
    return r;
}
