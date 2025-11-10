#include <external.h>
#include <caneka.h>

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

status TemplCtx_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    void *args[5];
    status r = READY;
    MemCh *m = MemCh_Make();
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

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}

status Templ_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    void *args[5];
    status r = READY;
    MemCh *m = MemCh_Make();
    TranspFile *tp = NULL;

    Str *path = IoUtil_GetAbsPath(m, Str_CstrRef(m, "./examples/example.templ"));
    StrVec *content = File_ToVec(m, path);

    Cursor *curs = Cursor_Make(m, content);
    TemplCtx *ctx = TemplCtx_FromCurs(m, curs, NULL);
    
    r |= Test(ctx->type.state & SUCCESS,
            "Templ: Roebling finished with state SUCCESS with keys", 
        NULL);

    if(r & ERROR){
        MemCh_Free(m);
        DebugStack_Pop();
        return r;
    }

    Span *seps = Span_Make(m);
    Span_Add(seps, B_Wrapped(m, (byte)'/', ZERO, MORE));

    Object *data = Object_Make(m, ZERO);
    Object *menu = Object_Make(m, ZERO);
    tp = TranspFile_Make(m);
    tp->local = StrVec_From(m, Str_CstrRef(m, "/things/one"));
    tp->name = StrVec_From(m, Str_CstrRef(m, "One"));
    Object_Set(menu, StrVec_From(m, Str_CstrRef(m, "one")), tp);
    tp = TranspFile_Make(m);

    tp->local = StrVec_From(m, Str_CstrRef(m, "/things/two"));
    tp->name = StrVec_From(m, Str_CstrRef(m, "Two"));
    Object_Set(menu, StrVec_From(m, Str_CstrRef(m, "two")), tp);
    tp = TranspFile_Make(m);
    
    tp->local = StrVec_From(m, Str_CstrRef(m, "/things/three"));
    tp->name = StrVec_From(m, Str_CstrRef(m, "Three"));
    Object_Set(menu, StrVec_From(m, Str_CstrRef(m, "three")), tp);

    Str *title = Str_CstrRef(m, "My Fancy Page");
    Str *para = Str_CstrRef(m, "And here is the masterful list of menu items!");
    Object_Set(data, Str_CstrRef(m, "title"), title);
    Object_Set(data, Str_CstrRef(m, "para"), para);

    StrVec *key = StrVec_From(m, Str_FromCstr(m, "items/menu", STRING_COPY));
    Path_Annotate(m, key, seps);

    Object_ByPath(data, key, menu, SPAN_OP_SET);

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
        MemCh_Free(m);
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

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
