#include <external.h>
#include <caneka.h>

status Doc_ToHtmlToS(Buff *bf, void *a, word tagIdx, word flags){
    void *args[6];
    if(tagIdx == DOC_HTML_OPEN){
        Fmt(bf, "<section class=\"doc-html\">\n", NULL);
    }else if(tagIdx == DOC_HTML_CLOSE){
        Fmt(bf, "</section>\n", NULL);
    }else if(tagIdx == DOC_HTML_FUNC_SECTION){
        Fmt(bf, "<h2>Functions</h2>\n", NULL);
    }else if(tagIdx == DOC_HTML_FUNC_START){
        Fmt(bf, "<article>\n", NULL);
    }else if(tagIdx == DOC_HTML_FUNC_NAME){
        args[0] = a;
        args[1] = NULL;
        Fmt(bf, "<h3>$</h3>\n", NULL);
    }else if(tagIdx == DOC_HTML_FUNC_DECL){
        args[0] = a;
        args[1] = NULL;
        Fmt(bf, "<pre class=\"func-decl\">$</pre>\n", NULL);
    }else if(tagIdx == DOC_HTML_FUNC_END){
        Fmt(bf, "</article>\n", NULL);
    }else if(tagIdx == DOC_HTML_TITLE){
        args[0] = a;
        args[1] = NULL;
        Fmt(bf, "<h1>$</h1>\n", args);
    }else if(tagIdx == DOC_HTML_MOD_COMMENT){
        args[0] = a;
        args[1] = NULL;
        Fmt(bf, "<div>@</h1>\n", args);
    }
    return ZERO;
}

status Doc_To(Buff *bf, DocComp *comp, ToSFunc func, TosFunc head, TosFunc foot){
    MemCh *m = bf->m;
    func(bf, NULL, DOC_HTML_OPEN, ZERO);
    StrVec *name = Seel_Get(comp, K(m, "name"));
    func(bf, name, DOC_HTML_TITLE, ZERO);
    Span *comments = Seel_Get(comp, K(m, "comments"));
    if(comments != NULL){
        DocComment *comm = Span_Get(comments, 0);
        StrVec *desc = Seel_Get(comm, K(m, "body"));
        func(bf, desc, DOC_HTML_MOD_COMMENT, ZERO);
    }

    Table *tbl = Inst_GetTblOfAtt(comp, K(m, "functions"));
    if(tbl != NULL && tbl->nvalues > 0){
        func(bf, NULL, DOC_HTML_FUNC_SECTION, ZERO);
        Iter it;
        Iter_Init(&it, tbl);
        while((Iter_It(&it) & END) == 0){
            Hashed *h = Iter_Get(&it);
            while(h != NULL){
                func(bf, NULL, DOC_HTML_FUNC_START, ZERO);
                StrVec *name = it->key;
                DocFunc *funcObj = it->value;
                StrVec *decl = StrVec_Make(m);
                StrVec_AddVec(decl, Seel_Get(funcObj, K(m, "ret"));
                StrVec_Add(decl, S(m, " "));
                StrVec_AddVec(decl, name);
                StrVec_AddVec(decl, Seel_Get(funcObj, K(m, "args"));
                StrVec_Add(decl, S(m, ";"));
                func(bf, decl, DOC_HTML_FUNC_DECL, ZERO);
                func(bf, NULL, DOC_HTML_FUNC_END, ZERO);
            }
        }
    }
    func(bf, NULL, DOC_HTML_CLOSE, ZERO);
    return ZERO;
}
