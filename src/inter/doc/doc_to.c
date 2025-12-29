#include <external.h>
#include <caneka.h>

status Doc_ToHtmlToS(Buff *bf, void *_a, word tagIdx, word flags){
    void *args[6];
    Abstract *a = _a;

    if(tagIdx == DOC_HTML_OPEN){
        Fmt(bf, "<section class=\"doc-html\">\n", NULL);
    }else if(tagIdx == DOC_HTML_CLOSE){
        Fmt(bf, "</section>\n", NULL);
    }else if(tagIdx == DOC_HTML_HEADER){
        Abstract *header = (Abstract *)a;
        if(a->type.of == TYPE_TEMPL){
            Templ *templ = (Templ *)a;
            Templ_Reset(templ);
            return Templ_ToS(templ, bf, NULL, NULL);
        }else if(a->type.of == TYPE_STRVEC){
            StrVec *path = (StrVec *)a;
            Buff *cbf = Buff_Make(bf->m, BUFF_UNBUFFERED);
            File_Open(cbf, StrVec_Str(bf->m, path), O_RDONLY);
            Buff_Pipe(bf, cbf);
        }
    }else if(tagIdx == DOC_HTML_FOOTER){
        if(a->type.of == TYPE_TEMPL){
            Templ *templ = (Templ *)a;
            Templ_Reset(templ);
            return Templ_ToS(templ, bf, NULL, NULL);
        }else if(a->type.of == TYPE_STRVEC){
            StrVec *path = (StrVec *)a;
            Buff *cbf = Buff_Make(bf->m, BUFF_UNBUFFERED);
            File_Open(cbf, StrVec_Str(bf->m, path), O_RDONLY);
            Buff_Pipe(bf, cbf);
        }
    }else if(tagIdx == DOC_HTML_FUNC_SECTION){
        Fmt(bf, "<h2>Functions</h2>\n", NULL);
    }else if(tagIdx == DOC_HTML_FUNC_START){
        Fmt(bf, "<article>\n", NULL);
    }else if(tagIdx == DOC_HTML_FUNC_NAME){
        args[0] = a;
        args[1] = NULL;
        Fmt(bf, "<h3>$</h3>\n", args);
    }else if(tagIdx == DOC_HTML_FUNC_COMMENT){
        args[0] = a;
        args[1] = NULL;
        Fmt(bf, "<pre class=\"wrap-text\">$</pre>\n", args);
    }else if(tagIdx == DOC_HTML_FUNC_DECL){
        args[0] = a;
        args[1] = NULL;
        Fmt(bf, "<pre class=\"func-decl\">$</pre>\n", args);
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

status Doc_To(Buff *bf, DocComp *comp, ToSFunc func){
    MemCh *m = bf->m;

    StrVec *name = Seel_Get(comp, K(m, "name"));

    NodeObj *page = Seel_Get(comp, K(m, "page"));

    Abstract *header = Inst_Att(page, K(m, "header"));
    if(header != NULL){
        if(header->type.of == TYPE_TEMPL){
            Templ *templ = (Templ *)header;
            Templ_SetData(templ, comp);
        }

        func(bf, header, DOC_HTML_HEADER, ZERO);
    }

    func(bf, NULL, DOC_HTML_OPEN, ZERO);
    func(bf, name, DOC_HTML_TITLE, ZERO);
    Span *comments = Seel_Get(comp, K(m, "comments"));
    if(comments != NULL){
        DocComment *comm = Span_Get(comments, 0);
        StrVec *desc = Seel_Get(comm, K(m, "body"));
        func(bf, desc, DOC_HTML_MOD_COMMENT, ZERO);
    }

    Table *tbl = Seel_Get(comp, K(m, "functions"));
    if(tbl != NULL && tbl->nvalues > 0){
        func(bf, NULL, DOC_HTML_FUNC_SECTION, ZERO);
        Iter it;
        Iter_Init(&it, Table_Ordered(m, tbl));
        while((Iter_Next(&it) & END) == 0){
            Hashed *h = Iter_Get(&it);
            if(h != NULL){
                func(bf, NULL, DOC_HTML_FUNC_START, ZERO);
                StrVec *name = h->key;
                func(bf, name, DOC_HTML_FUNC_NAME, ZERO);
                DocFunc *funcObj = h->value;
                StrVec *decl = StrVec_Make(m);
                StrVec_AddVec(decl, Seel_Get(funcObj, K(m, "ret")));
                StrVec_Add(decl, S(m, " "));
                StrVec_AddVec(decl, name);
                StrVec_AddVec(decl, Seel_Get(funcObj, K(m, "args")));
                StrVec_Add(decl, S(m, ";"));

                func(bf, decl, DOC_HTML_FUNC_DECL, ZERO);

                Span *comments = Seel_Get(funcObj, K(m, "comments"));
                if(comments != NULL && comments->nvalues > 0){
                    DocComment *comm = Span_Get(comments, 0);
                    StrVec *desc = Seel_Get(comm, K(m, "body"));
                    func(bf, desc, DOC_HTML_FUNC_COMMENT, ZERO);
                }

                func(bf, NULL, DOC_HTML_FUNC_END, ZERO);
            }
        }
    }
    func(bf, NULL, DOC_HTML_CLOSE, ZERO);
    Abstract *footer = Inst_Att(page, K(m, "footer"));
    if(footer != NULL){
        func(bf, footer, DOC_HTML_FOOTER, ZERO);
    }
    return ZERO;
}
