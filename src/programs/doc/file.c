#include <external.h>
#include <caneka.h>
#include <doc_module.h>

void Doc_FileOut(NodeObj *config, StrVec *src, StrVec *dest){
    void *ar[] = {src, dest, NULL};
    Out("^c. FileOut @ -> @^0\n", ar);
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
    return;
}
