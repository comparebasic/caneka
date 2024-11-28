#include <external.h> 
#include <caneka.h> 

status XmlT_AddAttsStr(XmlTCtx *xmlt, Mess *e, String *s){
    MemCtx *m = xmlt->m;
    Iter it;
    Iter_Init(&it, e->atts);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = (Hashed *)Iter_Get(&it);
        String_Add(m, s, (String *)h->item); 
        if(h->value != NULL){
            String *value = asIfc(h->value, TYPE_STRING);
            if((value->type.state & FLAG_STRING_IS_CASH) != 0){
                value = Cash_Replace(xmlt->m, xmlt->cash, (String *)value);
            }
            String_AddBytes(m, s, bytes("="), 1);
            String_Add(m, s, (String *)value);
        }
        if((it.type.state & FLAG_ITER_LAST) == 0){
            String_AddBytes(m, s, bytes(" "), 1);
        }
    }

    return SUCCESS;
}

status XmlT_Out(XmlTCtx *xmlt, Mess *e, OutFunc func){
    MemCtx *m = xmlt->m;
    Abstract *source = xmlt->source;

    String *s = String_Init(m, STRING_EXTEND);
    String_AddBytes(m, s, bytes("<"), 1);
    String_Add(m, s, (String *)e->name);


    if(e->atts != NULL){
        String_AddBytes(m, s, bytes(" "), 1);
        XmlT_AddAttsStr(xmlt, e, s);
    }

    boolean hasBody = e->body != NULL;
    boolean hasChildren = e->firstChild != NULL;
    boolean selfContained = !hasBody && !hasChildren;

    if(selfContained){
        String_AddBytes(m, s, bytes("/>"), 2);
    }else{
        String_AddBytes(m, s, bytes(">"), 1);
    }

    func(m, s, source);

    if(hasBody){
        String *body = e->body;
        if((body->type.state & FLAG_STRING_IS_CASH) != 0){
            body = Cash_Replace(xmlt->m, xmlt->cash, body);
        }
        func(m, body, source); 
    }

    if(e->firstChild){
        XmlT_Out(xmlt, e->next, func);
    }

    if(e->next){
        XmlT_Out(xmlt, e->next, func);
    }

    if(!selfContained){
        func(m, String_Make(m, bytes("</")), source);
        func(m, (String *)e->name, source);
        func(m, String_Make(m, bytes(">")), source);
    }

    return SUCCESS;
}

status XmlT_GetPropFlags(Mess *e, String *prop, Abstract *value){
    if(String_EqualsBytes(prop, bytes("include"))){
        return (FLAG_XML_INCLUDE|FLAG_XML_HIDDEN);
    }else if(String_EqualsBytes(prop, bytes("for"))){
        return (FLAG_XML_FOR|FLAG_XML_HIDDEN);
    }else if(String_EqualsBytes(prop, bytes("with"))){
        return (FLAG_XML_WITH|FLAG_XML_HIDDEN);
    }else if(String_EqualsBytes(prop, bytes("if"))){
        return (FLAG_XML_IF|FLAG_XML_HIDDEN);
    }else if(String_EqualsBytes(prop, bytes("if-not"))){
        return (FLAG_XML_IF_NOT|FLAG_XML_HIDDEN);
    }

    return ZERO;
}

XmlTCtx *XmlTCtx_Make(MemCtx *m){
    XmlTCtx *xmlt = (XmlTCtx *)MemCtx_Alloc(m, sizeof(XmlTCtx));
    xmlt->type.of = TYPE_XMLT;

    xmlt->parser = XmlParser_Make(m);
    xmlt->nd = NestedD_Make(m, NULL);
    xmlt->cash = Cash_Make(m, (Getter)NestedD_Get, (Abstract *)xmlt->nd);

    return xmlt;
}
