#include <external.h>
#include <caneka.h>
#include <test_module.h>

static WwwNav *getNav(MemCh *m, Table *coordTbl){
    WwwNav *nav = Inst_Make(m, TYPE_WWW_NAV);
    WwwNav *item = NULL;

    item = WwwNav_Make(m, Sv(m, "/docs"), Sv(m, "Docs"));
    WwwNav_Add(nav, item, coordTbl); 

    item = WwwNav_Make(m, Sv(m, "/docs/base"), Sv(m, "Base"));
    WwwNav_Add(nav, item, coordTbl); 

    item = WwwNav_Make(m, Sv(m, "/docs/base/bytes"), Sv(m, "Bytes"));
    WwwNav_Add(nav, item, coordTbl); 

    item = 
        WwwNav_Make(m, Sv(m, "/docs/base/types/error"), Sv(m, "Error"));
    WwwNav_Add(nav, item, coordTbl); 

    item = 
        WwwNav_Make(m, Sv(m, "/docs/base/bytes/str"), Sv(m, "Str"));
    WwwNav_Add(nav, item, coordTbl); 

    item = 
        WwwNav_Make(m, Sv(m, "/docs/base/bytes/strvec"), Sv(m, "StrVec"));
    WwwNav_Add(nav, item, coordTbl); 

    item = 
        WwwNav_Make(m, Sv(m, "/docs/base/bytes/fmt"), Sv(m, "Fmt"));
    WwwNav_Add(nav, item, coordTbl); 

    item = 
        WwwNav_Make(m, Sv(m, "/docs/base/bytes/tos"), Sv(m, "ToS"));
    WwwNav_Add(nav, item, coordTbl); 

    item = 
        WwwNav_Make(m, Sv(m, "/docs/base/mem"), Sv(m, "Mem"));
    WwwNav_Add(nav, item, coordTbl); 

    item = 
        WwwNav_Make(m, Sv(m, "/docs/base/mem/membook"), Sv(m, "MemBook"));
    WwwNav_Add(nav, item, coordTbl); 

    item = 
        WwwNav_Make(m, Sv(m, "/docs/base/mem/memch"), Sv(m, "MemCh"));
    WwwNav_Add(nav, item, coordTbl); 

    item = 
        WwwNav_Make(m, Sv(m, "/docs/base/mem/mempage"), Sv(m, "MemPage"));
    WwwNav_Add(nav, item, coordTbl); 

    item = 
        WwwNav_Make(m, Sv(m, "/docs/base/mem/span"), Sv(m, "Span"));
    WwwNav_Add(nav, item, coordTbl); 

    item = 
        WwwNav_Make(m, Sv(m, "/docs/base/mem/iter"), Sv(m, "Iter"));
    WwwNav_Add(nav, item, coordTbl); 
    return nav;
}


static WwwPage *getPage(MemCh *m){
    WwwPage *page = Inst_Make(m, TYPE_WWW_PAGE);
    StrVec *path = IoAbsPath(m, "fixtures/doc/header.templ");
    StrVec *content = File_ToVec(m, StrVec_Str(m, path));
    if(content == NULL){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "header content is NULL", NULL);
        return NULL;
    }

    Cursor *curs = Cursor_Make(m, content);
    TemplCtx *ctx = TemplCtx_FromCurs(m, curs, NULL);
    Templ *templ = (Templ *)Templ_Make(m, ctx->it.p);

    Seel_Set(page, K(m, "header"), templ); 
    Seel_Set(page, K(m, "footer"), IoAbsPath(m, "fixtures/doc/footer.html")); 
    return page;
}

status Doc_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    status r = READY;
    void *args[5];

    Str *path = IoUtil_GetAbsPath(m, Str_CstrRef(m, "./fixtures/doc/nav.templ"));
    StrVec *content = File_ToVec(m, path);
    Cursor *curs = Cursor_Make(m, content);

    Table *coordTbl = Table_Make(m);
    WwwPage *page = getPage(m);
    WwwNav *nav = getNav(m, coordTbl);;
    args[0] = nav;
    args[1] = page;
    args[2] = coordTbl;
    args[3] = NULL;
    Out("^p.nav @\npage @\n^0\ncoords @\n", args);

    Iter *it = Iter_Make(m, NULL);
    Str *s = S(m, "StrVec");
    Span *crd = Table_Get(coordTbl, s);
    NestSel_Init(it, nav, crd);

    while((NestSel_Next(it) & END) == 0){
        Hashed *h = NestSel_Get(it);
        WwwNav *item = h->value;

        StrVec *v = Seel_Get(item, K(m, "name"));
        Span *crd = Table_Get(coordTbl, v);
        Iter *navIt = Iter_Make(m, NULL);

        if(Empty(v)){
            void *ar[] = {crd, v, NULL};
            Out("^y.Warning missing Coords @ of @^0\n", ar);
            continue;
        }

        NestSel_Init(navIt, nav, crd);
        Seel_Set(page, S(m, "nav"), navIt); 
        Table *data = Table_Make(m);
        Table_Set(data, S(m, "page"), page);

        Buff *bf = Buff_Make(m, ZERO);
        Cursor_Reset(curs);
        TemplCtx *ctx = TemplCtx_FromCurs(m, curs, NULL);
        r |= Test(ctx->type.state & SUCCESS,
                "Templ: Roebling finished with state SUCCESS with keys", 
            NULL);

        if(r & ERROR){
            DebugStack_Pop();
            return r;
        }

        Templ *templ = (Templ *)Templ_Make(m, ctx->it.p);
        status re = Templ_Prepare(templ);

        args[0] = path;
        args[1] = NULL;
        r |= Test(re & SUCCESS, "Templ prepare for @\n", args);
        re = Templ_ToS(templ, bf, data, NULL);
        r |= Test(re & SUCCESS, "Templ Tos for @\n", args);

        args[0] = Seel_Get(item, K(m, "name"));
        args[1] = bf->v;
        args[2] = NULL;
        r |= Test(re & SUCCESS, "Templ for @ ->\n$\n", args);
    }

    /*
    StrVec *src = IoAbsPath(m, "src");
    DocComp *comp = DocComp_FromStr(m, src, S(m, "base.bytes.Str"));

    StrVec *name = Seel_Get(comp, K(m, "name"));
    Inst_ByPath(nav, name, Seel_Get(comp, K(m, "atts")), SPAN_OP_SET, NULL);

    args[0] = Seel_Get(comp, K(m, "atts"));
    args[1] = NULL;

    Seel_Set(comp, K(m, "page"), page);

    StrVec *path = Inst_Att(comp, K(m, "src"));
    StrVec *content = File_ToVec(m, StrVec_Str(m, path));

    Cursor *curs = Cursor_Make(m, content);
    Roebling *rbl = Doc_MakeRoebling(m, curs, comp);
    Roebling_Run(rbl);

    Buff *bf = Buff_Make(m, ZERO);
    Doc_To(bf, comp, Doc_ToHtmlToS);

    r |= Test(bf->v->total > 0 && (bf->type.state & ERROR) == 0,
        "Content from doc engine exists without errot", NULL);

    Buff *dist = Buff_Make(m, BUFF_UNBUFFERED|BUFF_CLOBBER);
    path = IoAbsPath(m, "dist/doc/html/");
    StrVec_AddVec(path, Inst_Att(comp, K(m, "url")));
    IoUtil_AddExt(m, path, S(m, "html"));

    File_Open(dist, StrVec_Str(m, path), O_CREAT|O_TRUNC|O_WRONLY);
    Buff_Pipe(dist, bf);
    File_Close(dist);
    */

    r |= ERROR;

    DebugStack_Pop();
    return r;
}
