#include <external.h> 
#include <caneka.h> 

status XmlT_AddAttsStr(XmlTCtx *xmlt, Mess *e, String *s){
    MemCtx *m = xmlt->m;
    Hashed *att = e->atts;
    while(att != NULL){
        String_Add(m, s, (String *)att->item); 
        if(att->value != NULL){
            String *value = asIfc(att->value, TYPE_STRING);
            value = Cash_Replace(xmlt->m, xmlt->cash, (String *)value);
            String_AddBytes(m, s, bytes("="), 1);
            String_Add(m, s, (String *)value);

        }

        att = att->next;
        if(att != NULL){
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
    String_Add(m, s, (String *)e->value->item);


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
        func(m, e->body, source); 
    }

    if(e->firstChild){
        XmlT_Out(xmlt, e->next, func);
    }

    if(e->next){
        XmlT_Out(xmlt, e->next, func);
    }

    if(!selfContained){
        func(m, String_Make(m, bytes("</")), source);
        func(m, (String *)e->value->item, source);
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
