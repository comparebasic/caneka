#include <basictypes_external.h> 
#include <caneka/caneka.h> 

status XmlElem_Template(XmlTCtx *xmlt, XmlElem *e, NestedD *nd, OutFunc func){
    MemCtx *m = 
    lifecycle_t r = ERROR;
    lifecycle_t prevCycle = m->destCycle;
    if(m->destCycle == 0){
        m->destCycle = MEM_TEMP;
    }

    if(DEBUG_XML_TEMPLATE){
        String_PrintColor(String_FormatN(m, 7,
            "Start of Elem: ", "%S", Typed_ToString(m, (Typed *)e), "flags:", "%d", &(e->flags), "\n"), ansi_yellow);
    }

    Span *tbl = nd->current_tbl;
    cbool_t outdent = false;
    if(HasFlag(e, FLAG_XML_IF) && !HasFlag(e, FLAG_XML_IF_SATISFIED)){
        Typed *if_t = Table_Get(m, e->props_tbl, (Typed *)String_Static("if"));
        Typed *ifValue_t = Table_Get(m, tbl, if_t);
        if(ifValue_t == NULL || Typed_Truthy(ifValue_t) != SUCCESS){
            return NOOP;
        }
    }

    if(HasFlag(e, FLAG_XML_IF_NOT) && !HasFlag(e, FLAG_XML_IF_SATISFIED)){
        Typed *ifNot_t = Table_Get(m, e->props_tbl, (Typed *)String_Static("if-not"));
        Typed *ifValue_t = Table_Get(m, tbl, ifNot_t);
        if(ifValue_t != NULL && Typed_Truthy(ifValue_t) == SUCCESS){
            return NOOP;
        }
    }

    if(HasFlag(e, FLAG_XML_WITH)){
        outdent = true;
        Typed *with_s = Table_Get(m, e->props_tbl, (Typed *)String_Static("with"));
        if(with_s != NULL){
            r = NestedD_With(m, nd, with_s);
            if(r != SUCCESS){
                return r;
            }
            tbl = nd->current_tbl;
        }
    }else if(!HasFlag(e, FLAG_XML_FOR_IN_PROGRESS) && HasFlag(e, FLAG_XML_FOR)){
        outdent = true;
        Typed *for_s = Table_Get(m, e->props_tbl, (Typed *)String_Static("for"));
        r = NestedD_For(m, nd, for_s);
        if(r != SUCCESS){
            printf("For nest did not succeed\n");
            return NOOP;
        }

        tbl = nd->current_tbl;
        SetFlag(e, FLAG_XML_FOR_IN_PROGRESS);
        SetFlag(e, FLAG_XML_IF_SATISFIED);
        while(NestedD_Next(m, nd) == SUCCESS){
            XmlElem_Template(m, e, nd, func, dest);
        }
        UnsetFlag(e, FLAG_XML_FOR_IN_PROGRESS);
        UnsetFlag(e, FLAG_XML_IF_SATISFIED);
        NestedD_Outdent(m, nd);
        return SUCCESS;
    }

    if(DEBUG_XML_TEMPLATE){
        String_PrintColor(String_FormatN(m, 7, "(Below) Elem: ", "%S", Typed_ToString(m, (Typed *)e), " USING DATA: ", "%S", Typed_ToString(m, (Typed *)tbl), "\n"), ansi_yellow);

    }

    String *build =  String_FormatN(m, 3, "<", "%S", String_Clone(m, e->tag_s)); 

    String *atts_s = XmlElem_MakeAttsStr(m, e, tbl);
    if(Typed_Truthy((Typed *)atts_s) == SUCCESS){
        String_Extend(build, Nstr(m, " "));
        String_Extend(build, atts_s); 
    }

    cbool_t hasBody = Typed_Truthy((Typed *)e->body_s) == SUCCESS;
    cbool_t hasChildren = Typed_Truthy((Typed *)e->children_sp) == SUCCESS;
    cbool_t selfContained = !hasBody && !hasChildren;

    if(selfContained){
        String_Extend(build, Nstr(m, "/>")); 
    }else{
        String_Extend(build, Nstr(m, ">")); 
    }

    m->destCycle = prevCycle;
    SetFlag(build, STRING_REALIGN);
    func(m, dest, String_Clone(m, build));

    if(hasBody){
        SetFlag(e->body_s, STRING_REPLAY);
        String *body_s = Cash_Replace(m, e->body_s, tbl);
        func(m, dest, body_s); 
    }

    if(hasChildren){
        Iter *it = Iter_Make(m, (Typed *)e->children_sp);
        XmlElem *e2 = NULL;
        while(!IsDone(it)){
            e2 = (XmlElem *)Iter_Next(it);
            XmlElem_Template(m, e2, nd, func, dest);
        }
    }

    if(!selfContained){
        func(m, dest, String_FormatN(m, 4, "</", "%S", String_Clone(m, e->tag_s), ">")); 
    }

    if(outdent){
        NestedD_Outdent(m, nd);
    }

    return SUCCESS;
}
