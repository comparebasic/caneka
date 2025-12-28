#include <external.h>
#include <caneka.h>
#include <test_module.h>

status Doc_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    status r = READY;
    void *args[5];

    StrVec *src = IoAbsPath(m, "src");
    DocComp *comp = DocComp_FromStr(m, src, S(m, "base.bytes.Str"));

    Str *path = StrVec_Str(m, Inst_Att(comp, K(m, "src")));
    StrVec *content = File_ToVec(m, path);

    Cursor *curs = Cursor_Make(m, content);
    Roebling *rbl = Doc_MakeRoebling(m, curs, comp);
    Roebling_Run(rbl);


    StrVec *path = IoAbsPath(bf->m, "examples/doc/header.templ");
    StrVec *content = File_ToVec(bf->m, StrVec_Str(bf->m, path));
    if(content == NULL){
        Error(bf->m, FUNCNAME, FILENAME, LINENUMBER,
            "header content is NULL", NULL);
        return ERROR;
    }

    Cursor *curs = Cursor_Make(bf->m, content);
    TemplCtx *ctx = TemplCtx_FromCurs(bf->m, curs, NULL);
    Templ *templ = (Templ *)Templ_Make(bf->m, ctx->it.p);
    templ->type.state |= DEBUG;
    if((Templ_Prepare(templ) & PROCESSING) == 0){
        void *args[2];
        args[0] = path;
        args[1] = NULL;
        Error(bf->m, FUNCNAME, FILENAME, LINENUMBER,
            "Error preparing template for $", args);
        return ERROR;
    }

    Templ_Reset(templ);

    Inst_SetAtt(comp, K(m, "header"), templ); 
    Inst_SetAtt(comp, K(m, "footer"), StrVec *path = IoAbsPath(bf->m, "examples/doc/footer.html")); 

    Buff *bf = Buff_Make(m, ZERO);
    Doc_To(bf, comp, Doc_ToHtmlToS);
    args[0] = comp;
    args[1] = bf->v;
    args[2] = NULL;
    Out("^p.doc object @\n^y.@^0\n", args);

    DebugStack_Pop();
    return r;
}
