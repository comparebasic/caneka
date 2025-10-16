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
    char *cstr = "<h1>";
    s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)+1, ZERO);
    StrVec *v = StrVec_From(m, s);
    Span_Add(expected, (Abstract *)v);

    fch = Fetcher_Make(m);
    fch->type.state |= FETCHER_VAR;
    cstr = "title";
    s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)+1, ZERO);
    tg = FetchTarget_MakeKey(m, s);
    Span_Add(fch->val.targets, (Abstract *)tg);
    Span_Add(expected, (Abstract *)fch);

    cstr = "</h1>\n<p>";
    s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)+1, ZERO);
    v = StrVec_From(m, s);
    Span_Add(expected, (Abstract *)v);

    fch = Fetcher_Make(m);
    fch->type.state |= FETCHER_VAR;
    cstr = "para";
    s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)+1, ZERO);
    tg = FetchTarget_MakeKey(m, s);
    Span_Add(fch->val.targets, (Abstract *)tg);
    Span_Add(expected, (Abstract *)fch);

    cstr = "</p>\n<ul>\n";
    s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)+1, ZERO);
    v = StrVec_From(m, s);
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

    cstr = "    <li><a href=\"";
    s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)+1, ZERO);
    v = StrVec_From(m, s);
    Span_Add(expected, (Abstract *)v);

    fch = Fetcher_Make(m);
    fch->type.state = FETCHER_VAR;
    cstr = "local";
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
    cstr = "local";
    cstr = "name";
    s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)+1, ZERO);
    tg = FetchTarget_MakeAtt(m, s);
    Span_Add(fch->val.targets, (Abstract *)tg);
    Span_Add(expected, (Abstract *)fch);

    cstr = "</a></li>\n";
    s = Str_Ref(m, (byte *)cstr, strlen(cstr), strlen(cstr)+1, ZERO);
    v = StrVec_From(m, s);
    Span_Add(expected, (Abstract *)v);

    fch = Fetcher_Make(m);
    fch->type.state = FETCHER_END;
    Span_Add(expected, (Abstract *)fch);

    cstr = "</ul>\n";
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
        "Mismatch in content found in example templ, expected:\n&\nhave:\n &", args);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}

status Templ_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    Abstract *args[5];
    status r = READY;
    MemCh *m = MemCh_Make();
    TranspFile *tp = NULL;

    Str *path = IoUtil_GetAbsPath(m, Str_CstrRef(m, "./examples/example.templ"));
    File *f = File_Make(m, path, NULL, STREAM_STRVEC);
    File_Open(f);
    File_Read(f, FILE_READ_MAX);
    File_Close(f);

    Cursor *curs = File_GetCurs(f);
    TemplCtx *ctx = TemplCtx_FromCurs(m, curs, NULL);
    
    r |= Test(ctx->type.state & SUCCESS,
            "Templ: Roebling finished with state SUCCESS with keys", 
        NULL);

    if(r & ERROR){
        MemCh_Free(m);
        DebugStack_Pop();
        return r;
    }

    Object *data = Object_Make(m, ZERO);
    Object *menu = Object_Make(m, ZERO);
    tp = TranspFile_Make(m);
    tp->local = StrVec_From(m, Str_CstrRef(m, "/things/one"));
    tp->name = StrVec_From(m, Str_CstrRef(m, "One"));
    Object_Set(menu, (Abstract *)StrVec_From(m, Str_CstrRef(m, "one")), (Abstract *)tp);
    tp = TranspFile_Make(m);
    tp->local = StrVec_From(m, Str_CstrRef(m, "/things/two"));
    tp->name = StrVec_From(m, Str_CstrRef(m, "Two"));
    Object_Set(menu, (Abstract *)StrVec_From(m, Str_CstrRef(m, "two")), (Abstract *)tp);
    tp = TranspFile_Make(m);
    tp->local = StrVec_From(m, Str_CstrRef(m, "/things/three"));
    tp->name = StrVec_From(m, Str_CstrRef(m, "Three"));
    Object_Set(menu, (Abstract *)StrVec_From(m, Str_CstrRef(m, "three")), (Abstract *)tp);

    Str *title = Str_CstrRef(m, "My Fancy Page");
    Str *para = Str_CstrRef(m, "And here is the masterful list of menu items!");
    Object_Set(data, (Abstract *)Str_CstrRef(m, "title"), (Abstract *)title);
    Object_Set(data, (Abstract *)Str_CstrRef(m, "para"), (Abstract *)para);
    Object_ByPath(data, StrVec_From(m, Str_CstrRef(m, "items/menu")), (Abstract *)menu, SPAN_OP_SET);

    Stream *sm = Stream_MakeStrVec(m);
    
    Templ *templ = (Templ *)Templ_Make(m, ctx->it.p);
    status result = Templ_Prepare(templ);

    args[0] = (Abstract *)templ;
    args[1] = (Abstract *)ctx->it.p;
    args[2] = (Abstract *)templ->content.p;
    args[3] = (Abstract *)NULL;
    r |= TestShow((result & PROCESSING),
        "Templ_Prepare has result PROCESSING",
        "Templ_Prepare did not finish properly @ & -> &", args);

    /*
    args[0] = (Abstract *)templ->content.p;
    args[1] = NULL;
    Out("^p.Content &^0\n", args);
    */

    if(r & ERROR){
        MemCh_Free(m);
        DebugStack_Pop();
        return r;
    }

    i64 total = Templ_ToS(templ, sm, (Abstract *)data, NULL);

    Str *expected = Str_CstrRef(m, keyTestContent);
    args[0] = (Abstract *)expected;
    args[1] = (Abstract *)sm->dest.curs->v;
    args[2] = (Abstract *)NULL;

    r |= TestShow(Equals((Abstract *)expected, (Abstract *)sm->dest.curs->v), 
        "Templ key value test has expected content", 
        "Templ key value mismatch test has expected content, expected:\n&\n\nhave:\n&", 
        args);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}

status TemplLogic_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    Abstract *args[5];
    status r = READY;
    MemCh *m = MemCh_Make();

    Str *path = IoUtil_GetAbsPath(m, Str_CstrRef(m, "./examples/nav.templ"));
    File *f = File_Make(m, path, NULL, STREAM_STRVEC);
    File_Open(f);
    File_Read(f, FILE_READ_MAX);
    File_Close(f);

    Cursor *curs = File_GetCurs(f);
    curs->type.state |= DEBUG;
    TemplCtx *ctx = TemplCtx_FromCurs(m, curs, NULL);

    r |= Test(ctx->type.state & SUCCESS,
            "Templ: Roebling finished with state SUCCESS with keys", 
        NULL);

    /*
    Abstract *args2[] = {
        (Abstract *)ctx->it.p,
        NULL
    };
    Out("^b.TemplContent: @^0.\n", args2);
    */

    Page *pg = NULL;
    Nav *nav = NULL;

    Object *data = Object_Make(m, ZERO);
    Nav *menuItems = Object_Make(m, TYPE_HTML_NAV);

    FetchTarget *indexTarget = FetchTarget_MakeProp(m, Str_CstrRef(m, "index")); 
    FetchTarget_Resolve(m, indexTarget, TYPE_HTML_NAV);

    FetchTarget *urlTarget = FetchTarget_MakeProp(m, Str_CstrRef(m, "url")); 
    FetchTarget_Resolve(m, urlTarget, TYPE_HTML_PAGE);

    FetchTarget *nameTarget = FetchTarget_MakeProp(m, Str_CstrRef(m, "name")); 
    FetchTarget_Resolve(m, nameTarget, TYPE_HTML_PAGE);

    FetchTarget *navNameTarget = FetchTarget_MakeProp(m, Str_CstrRef(m, "navName")); 
    FetchTarget_Resolve(m, navNameTarget, TYPE_HTML_PAGE);

    pg = Object_Make(m, TYPE_HTML_PAGE);
    Object_SetPropByIdx(pg, nameTarget->idx, (Abstract *)Str_CstrRef(m, "Docs"));
    Object_SetPropByIdx(pg, navNameTarget->idx, (Abstract *)Str_CstrRef(m, "docs"));
    Object_SetPropByIdx(pg, urlTarget->idx,
        (Abstract *)Str_CstrRef(m, "/docs/"));

    Object_SetPropByIdx(menuItems, indexTarget->idx, (Abstract *)pg);


    nav = Object_Make(m, TYPE_HTML_NAV);
    pg = Object_Make(m, TYPE_HTML_PAGE);
    Object_SetPropByIdx(pg, nameTarget->idx, (Abstract *)Str_CstrRef(m, "Str"));
    Object_SetPropByIdx(pg, navNameTarget->idx, (Abstract *)Str_CstrRef(m, "base/str"));
    Object_SetPropByIdx(pg, urlTarget->idx,
        (Abstract *)Str_CstrRef(m, "/docs/base/str.html"));
    Object_SetPropByIdx(nav, indexTarget->idx, (Abstract *)pg);
    Object_Set(menuItems, Object_GetPropByIdx(pg, nameTarget->idx), 
        (Abstract *)nav);

    nav = Object_Make(m, TYPE_HTML_NAV);
    pg = Object_Make(m, TYPE_HTML_PAGE);
    Object_SetPropByIdx(pg, nameTarget->idx, (Abstract *)Str_CstrRef(m, "Mem"));
    Object_SetPropByIdx(pg, navNameTarget->idx, (Abstract *)Str_CstrRef(m, "base/mem"));
    Object_SetPropByIdx(pg, urlTarget->idx,
        (Abstract *)Str_CstrRef(m, "/docs/base/mem.html"));
    Object_SetPropByIdx(nav, indexTarget->idx, (Abstract *)pg);
    Object_Set(menuItems, Object_GetPropByIdx(pg, nameTarget->idx), 
        (Abstract *)nav);

    nav = Object_Make(m, TYPE_HTML_NAV);
    pg = Object_Make(m, TYPE_HTML_PAGE);
    Object_SetPropByIdx(pg, nameTarget->idx, (Abstract *)Str_CstrRef(m, "Io"));
    Object_SetPropByIdx(pg, navNameTarget->idx, (Abstract *)Str_CstrRef(m, "base/io"));
    Object_SetPropByIdx(pg, urlTarget->idx,
        (Abstract *)Str_CstrRef(m, "/docs/base/io.html"));
    Object_SetPropByIdx(nav, indexTarget->idx, (Abstract *)pg);
    Object_Set(menuItems, Object_GetPropByIdx(pg, nameTarget->idx), 
        (Abstract *)nav);

    nav = Object_Make(m, TYPE_HTML_NAV);
    pg = Object_Make(m, TYPE_HTML_PAGE);
    Object_SetPropByIdx(pg, nameTarget->idx, (Abstract *)Str_CstrRef(m, "Suite"));
    Object_SetPropByIdx(pg, navNameTarget->idx, (Abstract *)Str_CstrRef(m, "base/suite"));
    Object_SetPropByIdx(pg, urlTarget->idx,
        (Abstract *)Str_CstrRef(m, "/docs/base/suite.html"));
    Object_SetPropByIdx(nav, indexTarget->idx, (Abstract *)pg);
    Object_Set(menuItems, Object_GetPropByIdx(pg, nameTarget->idx), 
        (Abstract *)nav);

    Object_Set(data, (Abstract *)Str_CstrRef(m, "menu-items"), (Abstract *)menuItems);

    Stream *sm = Stream_MakeStrVec(m);

    DebugStack_SetRef(data, data->type.of);
    
    Templ *templ = (Templ *)Templ_Make(m, ctx->it.p);

    args[0] = (Abstract *)templ->content.p;
    args[1] = NULL;
    Out("^p.Content &^0\n", args);

    templ->type.state |= DEBUG;
    i64 total = Templ_ToS(templ, sm, (Abstract *)data, NULL);

    Str *expected = Str_CstrRef(m, logicTestContent);
    args[0] = (Abstract *)expected;
    args[1] = (Abstract *)sm->dest.curs->v;
    args[2] = NULL;
    r |= TestShow(Equals((Abstract *)expected, (Abstract *)sm->dest.curs->v), 
        "Temple key value test has expected content", 
        "Temple key value test mismatch, expected @\nhave\n@", 
        args);


    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}

