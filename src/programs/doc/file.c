#include <external.h>
#include <caneka.h>
#include <doc_module.h>

void Doc_GenPage(WwwPage *page, StrVec *header, StrVec *footer){
    MemCh *m = page->m;

    Seel_Set(page, S(m, "name"), Sv(m, "docPage"));

    Gen *headerGen = Gen_FromPath(m, header, NULL);
    Gen_Setup(m, headerGen, NULL);

    Gen *footerGen = Gen_FromPath(m, footer, NULL);
    Gen_Setup(m, footerGen, NULL);

    Span *gens = Seel_Get(page, K(m, "gens"));
    Span_Add(gens, headerGen);
    Span_Add(gens, footerGen);
}

void Doc_FileOut(WwwPage *page, WwwNav *item, StrVec *dest){
    MemCh *m = page->m;

    Buff *bf = Buff_Make(m, BUFF_UNBUFFERED|BUFF_CLOBBER);
    Dir_CheckCreateFor(m, dest);
    File_Open(bf, StrVec_Str(m, dest), O_WRONLY|O_CREAT);

    Span *gens = Seel_Get(page, K(m, "gens")); 
    Table *data = Table_Make(m);
    Table_Set(data, S(m, "page"), page);

    Gen_Run(Span_Get(gens, 0), bf, data);

    StrVec *fpath = Inst_Att(item, K(m, "fpath"));
    StrVec *ext = IoUtil_GetExt(m, fpath);
    if(Equals(ext, S(m, "c"))){
        StrVec *content = File_ToVec(m, StrVec_Str(m, fpath));
        Cursor *curs = Cursor_Make(m, content);
        DocComp *comp = DocComp_FromStr(m,
            fpath, Inst_Att(item, K(m, "display-path")));

        Roebling *rbl = Doc_MakeRoebling(m, curs, comp);
        Roebling_Run(rbl);
        Doc_To(bf, comp, Doc_ToHtmlToS);
    }else if(Equals(ext, S(m, "txt"))){
        DocComp *comp = DocComp_DocOnly(m, Seel_Get(item, K(m, "name")));
        Span *comments = Span_Make(m);
        StrVec *content = File_ToVec(m, StrVec_Str(m, fpath));
        if(content != NULL){
            DocComment *comm = Inst_Make(m, TYPE_DOC_COMMENT); 
            Seel_Set(comm, S(m, "body"), content);
            Span_Add(comments, comm);
        }
        Seel_Set(comp, S(m, "comments"), comments);
        Doc_To(bf, comp, Doc_ToHtmlToS);
    }

    Gen_Run(Span_Get(gens, 1), bf, data);
    return;
}
