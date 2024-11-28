#include <external.h> 
#include <caneka.h> 

static void setFlags(Mess *e){
    String *value = NULL;
    if(e->atts != NULL){
        if((value = (String *)Table_Get(e->atts, (Abstract *)String_Make(e->atts->m, bytes("if")))) != NULL){
            e->type.state |= FLAG_XML_IF;
            value->type.state |= FLAG_XML_HIDDEN;
        }
        if((value = (String *)Table_Get(e->atts, (Abstract *)String_Make(e->atts->m, bytes("if-not")))) != NULL){
            e->type.state |= FLAG_XML_IF_NOT;
            value->type.state |= FLAG_XML_HIDDEN;
        }
        if((value = (String *)Table_Get(e->atts, (Abstract *)String_Make(e->atts->m, bytes("include")))) != NULL){
            e->type.state |= FLAG_XML_INCLUDE;
            value->type.state |= FLAG_XML_HIDDEN;
        }
        if((value = (String *)Table_Get(e->atts, (Abstract *)String_Make(e->atts->m, bytes("for")))) != NULL){
            e->type.state |= FLAG_XML_FOR;
            value->type.state |= FLAG_XML_HIDDEN;
        }
        if((value = (String *)Table_Get(e->atts, (Abstract *)String_Make(e->atts->m, bytes("with")))) != NULL){
            e->type.state |= FLAG_XML_WITH;
            value->type.state |= FLAG_XML_HIDDEN;
        }
    }
    e->type.state |= TRACKED;
}

status XmlT_Template(XmlTCtx *xmlt, Mess *e, NestedD *nd, OutFunc func){
    MemCtx *m = xmlt->m;
    status r = ERROR;


    Span *tbl = nd->current_tbl;
    boolean outdent = FALSE;
    if((e->type.state & TRACKED) == 0){
        setFlags(e); 
    }

    if(DEBUG_XML_TEMPLATE){
        Debug_Print((void *)e, 0, "Start of Elem: ", DEBUG_XML_TEMPLATE, FALSE);
        printf("\x1b[%dmflags: %d\n", DEBUG_XML_TEMPLATE, e->type.state);
    }

    if((e->type.state & FLAG_XML_IF) != 0 && (e->type.state & FLAG_XML_SATISFIED) == 0){
        Abstract *if_t = Table_Get(e->atts, (Abstract *)String_Make(m, bytes("if")));
        Abstract *ifValue_t = NestedD_Get(nd, if_t);
        if(ifValue_t == NULL || !Caneka_Truthy(ifValue_t)){
            return NOOP;
        }
    }

    if((e->type.state & FLAG_XML_IF_NOT) != 0 && (e->type.state & FLAG_XML_SATISFIED) == 0){
        Abstract *ifNot_t = Table_Get(e->atts, (Abstract *)String_Make(m, bytes("if-not")));
        Abstract *ifValue_t = Table_Get(tbl, ifNot_t);
        if(ifValue_t != NULL && Caneka_Truthy(ifValue_t)){
            return NOOP;
        }
    }

    if((e->type.state & FLAG_XML_WITH) != 0){
        outdent = TRUE;
        Abstract *with_s = Table_Get(e->atts, (Abstract *)String_Make(m, bytes("with")));
        if(with_s != NULL){
            r = NestedD_With(m, nd, with_s);
            if(r != SUCCESS){
                return r;
            }
            tbl = nd->current_tbl;
        }
    }else if((e->type.state & FLAG_XML_IN_PROGRESS) != 0 && 
            (e->type.state & FLAG_XML_FOR) != 0){
        outdent = TRUE;
        Abstract *for_s = Table_Get(e->atts, (Abstract *)String_Make(m, bytes("for")));
        r = NestedD_For(nd, for_s);
        if(r != SUCCESS){
            printf("For nest did not succeed\n");
            return NOOP;
        }

        tbl = nd->current_tbl;
        e->type.state |= FLAG_XML_IN_PROGRESS;
        e->type.state |= FLAG_XML_SATISFIED;
        while((NestedD_Next(nd) & END) == 0){
            XmlT_Template(xmlt, e, nd, func);
        }
        e->type.state &= ~(FLAG_XML_IN_PROGRESS|FLAG_XML_SATISFIED);
        NestedD_Outdent(nd);
        return SUCCESS;
    }

    if(DEBUG_XML_TEMPLATE){
        printf("\n");
        Debug_Print((void *)e, 0, "(Below) Elem: ", DEBUG_XML_TEMPLATE, TRUE);
        Debug_Print((void *)tbl, 0, " USING DATA: ", DEBUG_XML_TEMPLATE, TRUE);
        printf("\n");
    }

    String *build = String_Init(m, STRING_EXTEND);

    boolean hasBody = e->body != NULL;
    boolean hasChildren = e->firstChild != NULL;
    boolean selfContained = !hasBody && !hasChildren;

    if(e->name != NULL){
        String_AddBytes(m, build, bytes("<"), 1);
        String_Add(m, build, e->name);
        XmlT_AddAttsStr(xmlt, e, build);

        if(selfContained){
            String_AddBytes(m, build, bytes("/>"), 2); 
        }else{
            String_AddBytes(m, build, bytes(">"), 1); 
        }
    }

    func(m, build, (Abstract *)xmlt);

    if(hasBody){
        String *body = e->body;
        if((body->type.state & FLAG_STRING_IS_CASH) != 0){
            body = Cash_Replace(xmlt->m, xmlt->cash, body);
        }
        func(m, body, (Abstract *)xmlt); 
    }

    if(e->name != NULL){
        if(hasChildren){
            Mess *child = e->firstChild;
            while(child != NULL){
                XmlT_Template(xmlt, child, nd, func);
                child = child->next;
            }
        }

        if(!selfContained){
            String *closeTag = String_Init(m, STRING_EXTEND);
            String_AddBytes(m, closeTag, bytes("</"), 2);
            String_Add(m, closeTag, e->name);
            String_AddBytes(m, closeTag, bytes(">"), 1);
            func(m, closeTag, (Abstract *)xmlt); 
        }
    }

    if(outdent){
        NestedD_Outdent(nd);
    }

    return SUCCESS;
}
