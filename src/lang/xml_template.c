#include <external.h> 
#include <caneka.h> 

static void setFlags(Mess *e){
    if(e->atts != NULL){
        if(Table_Get(e->atts, (Abstract *)String_Make(e->atts->m, bytes("if"))) != NULL){
            e->type.state |= FLAG_XML_IF;
        }
        if(Table_Get(e->atts, (Abstract *)String_Make(e->atts->m, bytes("if-not"))) != NULL){
            e->type.state |= FLAG_XML_IF_NOT;
        }
        if(Table_Get(e->atts, (Abstract *)String_Make(e->atts->m, bytes("include"))) != NULL){
            e->type.state |= FLAG_XML_INCLUDE;
        }
        if(Table_Get(e->atts, (Abstract *)String_Make(e->atts->m, bytes("for"))) != NULL){
            e->type.state |= FLAG_XML_FOR;
        }
    }
    e->type.state |= TRACKED;
}

status XmlTCtx_Template(XmlTCtx *xmlt, Mess *e, NestedD *nd, OutFunc func){
    MemCtx *m = xmlt->m;
    status r = ERROR;

    if(DEBUG_XML_TEMPLATE){
        Debug_Print((void *)e, 0, "Start of Elem: ", DEBUG_XML_TEMPLATE, FALSE);
        printf("\x1b[%dmflags: %d\n", DEBUG_XML_TEMPLATE, e->type.state);
    }

    Span *tbl = nd->current_tbl;
    boolean outdent = FALSE;
    if((e->type.state & TRACKED) == 0){
        setFlags(e); 
    }

    if((e->type.state & FLAG_XML_IF) != 0 && (e->type.state & FLAG_XML_SATISFIED) == 0){
        Abstract *if_t = Table_Get(e->atts, (Abstract *)String_Make(m, bytes("if")));
        Abstract *ifValue_t = NestedD_Get(nd, if_t);
        if(ifValue_t == NULL || !Caneka_Truthy(ifValue_t)){
            return NOOP;
        }
    }
    /*

    if(HasFlag(e, FLAG_XML_IF_NOT) && !HasFlag(e, FLAG_XML_IF_SATISFIED)){
        Abstract *ifNot_t = Table_Get(m, e->props_tbl, (Abstract *)String_Static("if-not"));
        Abstract *ifValue_t = Table_Get(m, tbl, ifNot_t);
        if(ifValue_t != NULL && Abstract_Truthy(ifValue_t) == SUCCESS){
            return NOOP;
        }
    }

    if(HasFlag(e, FLAG_XML_WITH)){
        outdent = true;
        Abstract *with_s = Table_Get(m, e->props_tbl, (Abstract *)String_Static("with"));
        if(with_s != NULL){
            r = NestedD_With(m, nd, with_s);
            if(r != SUCCESS){
                return r;
            }
            tbl = nd->current_tbl;
        }
    }else if(!HasFlag(e, FLAG_XML_FOR_IN_PROGRESS) && HasFlag(e, FLAG_XML_FOR)){
        outdent = true;
        Abstract *for_s = Table_Get(m, e->props_tbl, (Abstract *)String_Static("for"));
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
        String_PrintColor(String_FormatN(m, 7, "(Below) Elem: ", "%S", Abstract_ToString(m, (Abstract *)e), " USING DATA: ", "%S", Abstract_ToString(m, (Abstract *)tbl), "\n"), ansi_yellow);

    }

    String *build =  String_FormatN(m, 3, "<", "%S", String_Clone(m, e->tag_s)); 

    String *atts_s = XmlElem_MakeAttsStr(m, e, tbl);
    if(Abstract_Truthy((Abstract *)atts_s) == SUCCESS){
        String_Extend(build, Nstr(m, " "));
        String_Extend(build, atts_s); 
    }

    boolean hasBody = Abstract_Truthy((Abstract *)e->body_s) == SUCCESS;
    boolean hasChildren = Abstract_Truthy((Abstract *)e->children_sp) == SUCCESS;
    boolean selfContained = !hasBody && !hasChildren;

    if(selfContained){
        String_Extend(build, Nstr(m, "/>")); 
    }else{
        String_Extend(build, Nstr(m, ">")); 
    }

    SetFlag(build, STRING_REALIGN);
    func(m, dest, String_Clone(m, build));

    if(hasBody){
        SetFlag(e->body_s, STRING_REPLAY);
        String *body_s = Cash_Replace(m, e->body_s, tbl);
        func(m, dest, body_s); 
    }

    if(hasChildren){
        Iter *it = Iter_Make(m, (Abstract *)e->children_sp);
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
    */

    return SUCCESS;
}
