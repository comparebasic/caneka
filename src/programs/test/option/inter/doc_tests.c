#include <external.h>
#include <caneka.h>
#include <test_module.h>

static NodeObj *getPage(MemCh *m){
    WwwPage *page = Inst_Make(m, TYPE_WWW_PAGE);
    StrVec *path = IoAbsPath(m, "examples/doc/header.templ");
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
    Seel_Set(page, K(m, "footer"), IoAbsPath(m, "examples/doc/footer.html")); 
    NodeObj *nav = Inst_Make(m, TYPE_NODEOBJ);
    Seel_Set(page, K(m, "nav"), nav);
    return page;
}

status Doc_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    status r = READY;
    void *args[5];

    NodeObj *page = getPage(m);
    NodeObj *nav = Seel_Get(page, K(m, "nav"));

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

    DebugStack_Pop();
    return r;
}
