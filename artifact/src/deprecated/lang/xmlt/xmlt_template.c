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
    e->type.state |= PERSIST_TRACKED;
}

static boolean earlyOutAtts(Mess *e, String *key, NestedD *nd){
    Abstract *if_t = Table_Get(e->atts, (Abstract *)key);
    Abstract *ifValue_t = NestedD_Get(nd, if_t);
    if(DEBUG_XML_TEMPLATE){
        Debug_Print((void *)key, 0, "Falsey?-", DEBUG_XML_TEMPLATE, FALSE);
        Debug_Print((void *)if_t, 0, ": ", DEBUG_XML_TEMPLATE, FALSE);
        Debug_Print((void *)ifValue_t, 0, " -> ", DEBUG_XML_TEMPLATE, FALSE);
        printf("\n");
    }
    if(ifValue_t == NULL || !Caneka_Truthy(ifValue_t)){
        return TRUE;
    }
    return FALSE;
}

status XmlT_Template(XmlTCtx *xmlt, MessSet *set, Abstract *a, NestedD *nd, OutFunc func){
    MemCtx *m = xmlt->m;
    status r = ERROR;

    String *body = NULL;
    Mess *e = NULL;
    if(a->type.of == TYPE_STRING){
        body = (String *)a; 
    }else if(a->type.of == TYPE_STRING){
        e = (Mess *)a;
    }

    Span *tbl = nd->current_tbl;
    boolean outdent = FALSE;
    if((e->type.state & PERSIST_TRACKED) == 0){
        setFlags(e); 
    }

    if((e->type.state & FLAG_XML_IF) != 0 && (e->type.state & FLAG_XML_IN_PROGRESS) == 0){
        if(earlyOutAtts(e, String_Make(m, bytes("if")), nd)){
            return NOOP;
        }
    }

    if((e->type.state & FLAG_XML_IF_NOT) != 0 && (e->type.state & FLAG_XML_IN_PROGRESS) == 0){
        if(!earlyOutAtts(e, String_Make(m, bytes("if-not")), nd)){
            return NOOP;
        }
    }

    if((e->type.state & FLAG_XML_WITH) != 0){
        outdent = TRUE;
        Abstract *with_s = Table_Get(e->atts, (Abstract *)String_Make(m, bytes("with")));
        if(DEBUG_XML_TEMPLATE_NESTING){
            Debug_Print((void *)with_s, 0, "With: ", DEBUG_XML_TEMPLATE_NESTING, TRUE);
            printf("\n");
        }
        if(with_s != NULL){
            r = NestedD_With(m, nd, with_s);
            if(r != SUCCESS){
                return r;
            }
            tbl = nd->current_tbl;
        }
    }else if((e->type.state & FLAG_XML_FOR) != 0 && (e->type.state & FLAG_XML_IN_PROGRESS) == 0){

        if((e->type.state & FLAG_XML_IF)){
            if(earlyOutAtts(e, String_Make(m, bytes("if")), nd)){
                return NOOP;
            }
        }

        if((e->type.state & FLAG_XML_IF_NOT)){
            if(earlyOutAtts(e, String_Make(m, bytes("if-not")), nd)){
                return NOOP;
            }
        }

        outdent = TRUE;
        Abstract *for_s = Table_Get(e->atts, (Abstract *)String_Make(m, bytes("for")));
        if(DEBUG_XML_TEMPLATE_NESTING){
            Debug_Print((void *)for_s, 0, "For: ", DEBUG_XML_TEMPLATE_NESTING, TRUE);
            printf("\n");
        }
        r = NestedD_For(nd, for_s);
        if(r != SUCCESS){
            printf("For nest did not succeed\n");
            return NOOP;
        }

        tbl = nd->current_tbl;
        e->type.state |= FLAG_XML_IN_PROGRESS;
        while((NestedD_Next(nd) & END) == 0){
            XmlT_Template(xmlt, set, (Abstract *)e, nd, func);
        }
        e->type.state &= ~(FLAG_XML_IN_PROGRESS);
        NestedD_Outdent(nd);
        return SUCCESS;
    }

    if(0 && DEBUG_XML_TEMPLATE){
        printf("\n");
        Debug_Print((void *)e, 0, "(Below) Elem: ", DEBUG_XML_TEMPLATE, TRUE);
        Debug_Print((void *)tbl, 0, " USING DATA: ", DEBUG_XML_TEMPLATE, TRUE);
        printf("\n");
    }

    String *build = String_Init(m, STRING_EXTEND);

    boolean hasBody = e->type.of == TYPE_STRING;
    boolean hasChildren = e->children != NULL && e->children->nvalues > 0;
    boolean selfContained = !hasBody && !hasChildren;

    if(DEBUG_XML_TEMPLATE){
        printf("\x1b[%dmHasBody:%d,HasChildren:%d,selfContained:%d\x1b[0m\n", DEBUG_XML_TEMPLATE, hasBody, hasChildren, selfContained);
    }

    String *tag = NULL;
    if(e->tagIdx != 0){
        tag = Span_Get(set->tagTbl, e->tagIdx);
        String_AddBytes(m, build, bytes("<"), 1);
        String_Add(m, build, tag);
        XmlT_AddAttsStr(xmlt, e, build);

        if(selfContained){
            String_AddBytes(m, build, bytes("/>"), 2); 
        }else{
            String_AddBytes(m, build, bytes(">"), 1); 
        }
    }

    func(m, build, (Abstract *)xmlt);

    if(hasBody){
        String *body = (String *)e;
        if((body->type.state & FLAG_STRING_IS_CASH) != 0){
            body = Cash_Replace(xmlt->m, xmlt->cash, body);
        }
        func(m, body, (Abstract *)xmlt); 
    }

    if(e->tagIdx != 0){
        if(hasChildren){
            Iter it;
            Iter_Init(&it, e->children);
            while((Iter_Next(&it) & END) == 0){
                Mess *child = (Mess *)Iter_Get(&it);
                XmlT_Template(xmlt, set, (Abstract *)child, nd, func);
            }
        }

        if(!selfContained){
            String *closeTag = String_Init(m, STRING_EXTEND);
            String_AddBytes(m, closeTag, bytes("</"), 2);
            String_Add(m, closeTag, tag);
            String_AddBytes(m, closeTag, bytes(">"), 1);
            func(m, closeTag, (Abstract *)xmlt); 
        }
    }

    if(outdent){
        if(DEBUG_XML_TEMPLATE){
            printf("\x1b[%dmOutdent\x1b[0m\n", DEBUG_XML_TEMPLATE);
        }
        NestedD_Outdent(nd);
    }

    return SUCCESS;
}
