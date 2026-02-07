#include <external.h>
#include <caneka.h>
#include <doc_module.h>

void Doc_FileOut(WwwPage *page, WwwNav *item, StrVec *dest){
    MemCh *m = page->m;
    Buff *bf = Buff_Make(m, BUFF_UNBUFFERED|BUFF_CLOBBER);
    Dir_CheckCreateFor(m, dest);
    File_Open(bf, StrVec_Str(m, dest), O_WRONLY|O_CREAT);

    Table *data = Table_Make(m);
    Table_Set(data, S(m, "page"), page);

    Str *path = IoUtil_GetAbsPath(m,
        Str_CstrRef(m, "./fixtures/doc/nav.templ"));
    StrVec *content = File_ToVec(m, path);

    Cursor *curs = Cursor_Make(m, content);
    TemplCtx *ctx = TemplCtx_FromCurs(m, curs, NULL);
    Templ *templ = (Templ *)Templ_Make(m, ctx->it.p);
    status result = Templ_Prepare(templ);
    status re = Templ_ToS(templ, bf, data, NULL);

    StrVec *fpath = Inst_Att(item, K(m, "fpath"));

    content = File_ToVec(m, StrVec_Str(m, fpath));
    void *_ar[] = {fpath, NULL};
    Out("^p.Fpath @^0\n", _ar);
    curs = Cursor_Make(m, content);

    DocComp *comp = DocComp_FromStr(m,
        fpath, Inst_Att(item, K(m, "display-path")));

    Roebling *rbl = Doc_MakeRoebling(m, curs, comp);
    Roebling_Run(rbl);
    Doc_To(bf, comp, Doc_ToHtmlToS);

    void *ar[] = {
        item,
        bf->v,
        dest,
        NULL
    };

    Out("^y.Nav Item @ navHtml @^0 -> @\n", ar);

    return;
}
