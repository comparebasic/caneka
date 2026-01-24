#include <external.h>
#include <caneka.h>

char *navTwo = ""
"<nav>\n"
"<ul>\n"
"    <li><a href=\"/docs\">Docs</a><ul>\n"
"    <li><span class=\"active\">Base</span></li>\n"
"</ul>\n"
"</li>\n"
"</ul>\n"
"</nav>\n"
;

static WwwNav *navMakeTwo(MemCh *m, Table *coordTbl){

    WwwNav *node = Inst_Make(m, TYPE_WWW_NAV);

    StrVec *path = IoPath_From(m, S(m, "/docs"));
    Span *coords = Span_Make(m);
    WwwNav *item = Inst_Make(m, TYPE_WWW_NAV);
    Seel_Set(item, K(m, "url"), path);
    StrVec *name = Sv(m, "Docs");
    Seel_Set(item, K(m, "name"), name);
    Inst_ByPath(node, path, item, SPAN_OP_SET, coords);
    Table_Set(coordTbl, name, coords);

    path = IoPath_From(m, S(m, "/docs/base"));
    coords = Span_Make(m);
    item = Inst_Make(m, TYPE_WWW_NAV);
    Seel_Set(item, K(m, "url"), path);
    name = Sv(m, "Base");
    Seel_Set(item, K(m, "name"), name);
    Inst_ByPath(node, path, item, SPAN_OP_SET, coords);
    Table_Set(coordTbl, name, coords);

    return node;
}

static WwwNav *navMake(MemCh *m, Table *coordTbl){

    WwwNav *node = Inst_Make(m, TYPE_WWW_NAV);

    StrVec *path = IoPath_From(m, S(m, "/docs"));
    Span *coords = Span_Make(m);
    WwwNav *item = Inst_Make(m, TYPE_WWW_NAV);
    Seel_Set(item, K(m, "url"), path);
    StrVec *name = Sv(m, "Docs");
    Seel_Set(item, K(m, "name"), name);
    Inst_ByPath(node, path, item, SPAN_OP_SET, coords);
    Table_Set(coordTbl, name, coords);

    path = IoPath_From(m, S(m, "/docs/base"));
    coords = Span_Make(m);
    item = Inst_Make(m, TYPE_WWW_NAV);
    Seel_Set(item, K(m, "url"), path);
    name = Sv(m, "Base");
    Seel_Set(item, K(m, "name"), name);
    Inst_ByPath(node, path, item, SPAN_OP_SET, coords);
    Table_Set(coordTbl, name, coords);

    path = IoPath_From(m, S(m, "/docs/base/bytes"));
    coords = Span_Make(m);
    item = Inst_Make(m, TYPE_WWW_NAV);
    Seel_Set(item, K(m, "url"), path);
    name = Sv(m, "bytes");
    Seel_Set(item, K(m, "name"), name);
    Inst_ByPath(node, path, item, SPAN_OP_SET, coords);
    Table_Set(coordTbl, name, coords);

    path = IoPath_From(m, S(m, "/docs/base/bytes/str"));
    coords = Span_Make(m);
    item = Inst_Make(m, TYPE_WWW_NAV);
    Seel_Set(item, K(m, "url"), path);
    name = Sv(m, "Str");
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

    path = IoPath_From(m, S(m, "/docs/base/mem"));
    coords = Span_Make(m);
    item = Inst_Make(m, TYPE_WWW_NAV);
    Seel_Set(item, K(m, "url"), path);
    name = Sv(m, "mem");
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

    Table *coordTbl = Table_Make(m);
    WwwNav *nav = navMakeTwo(m, coordTbl);
    Inst *page = Inst_Make(m, TYPE_WWW_PAGE);

    Iter *navIt = Iter_Make(m, NULL);
    Str *s = S(m, "Base");
    Span *crd = Table_Get(coordTbl, s);
    NestSel_Init(navIt, nav, crd);

    Seel_Set(page, S(m, "nav"), navIt); 
    Table *data = Table_Make(m);
    Table_Set(data, S(m, "page"), page);
    Buff *bf = Buff_Make(m, ZERO);

    DebugStack_SetRef(bf->v, bf->v->type.of);
    status result = Templ_Prepare(templ);

    i64 total = Templ_ToS(templ, bf, data, NULL);

    Str *expected = S(m, navTwo);
    Str *output = StrVec_Str(m, bf->v);
    output->type.state |= DEBUG;
    args[0] = output;
    args[1] = NULL;
    r |= Test(Equals(bf->v, expected), 
        "Nav template for two items is as expected, have:&", args);

    DebugStack_Pop();
    return r;
}

status TemplNavNested_Tests(MemCh *m){
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

    Table *coordTbl = Table_Make(m);
    WwwNav *nav = navMake(m, coordTbl);
    Inst *page = Inst_Make(m, TYPE_WWW_PAGE);

    Iter *navIt = Iter_Make(m, NULL);
    Str *s = S(m, "Fmt");
    Span *crd = Table_Get(coordTbl, s);
    NestSel_Init(navIt, nav, crd);

    Seel_Set(page, S(m, "nav"), navIt); 
    Table *data = Table_Make(m);
    Table_Set(data, S(m, "page"), page);
    Buff *bf = Buff_Make(m, ZERO);

    status result = Templ_Prepare(templ);

    templ->type.state |= DEBUG;

    status re = Templ_ToS(templ, bf, data, NULL);

    args[0] = Type_StateVec(m, TYPE_ITER_UPPER, re);
    args[1] = bf->v;
    args[2] = s;
    args[3] = NULL;
    Out("^p.@ bf->v @\nFocus: @^0\n", args);

    r |= ERROR;

    DebugStack_Pop();
    return r;
}

