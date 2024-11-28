#include <external.h> 
#include <caneka.h> 

status XmlT_AddAttsStr(XmlTCtx *xmlt, Mess *e, String *s){
    MemCtx *m = xmlt->m;
    Iter it;
    Iter_Init(&it, e->atts);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = (Hashed *)Iter_Get(&it);
        if(h != NULL){
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
    }

    return SUCCESS;
}

status XmlT_Out(MemCtx *_, String *s, Abstract *source){
    XmlTCtx *xmlt = (XmlTCtx *) as(source, TYPE_XMLT);
    return String_Add(xmlt->m, xmlt->result, s); 
}

status XmlT_Parse(XmlTCtx *xmlt, String *s, Span *tbl){
    status r = (XmlParser_Parse(xmlt->parser, s) & SUCCESS);
    if((r & SUCCESS) != 0){
        NestedD_Init(xmlt->m, xmlt->nd, tbl);
        r = XmlT_Template(xmlt, xmlt->parser->ctx->root->firstChild, xmlt->nd, XmlT_Out);   
    }
    return r;
}

XmlTCtx *XmlT_Make(MemCtx *m, String *result){
    XmlTCtx *xmlt = (XmlTCtx *)MemCtx_Alloc(m, sizeof(XmlTCtx));
    xmlt->type.of = TYPE_XMLT;
    xmlt->m = m;

    xmlt->parser = XmlParser_Make(m);
    xmlt->nd = NestedD_Make(m, NULL);
    xmlt->cash = Cash_Make(m, (Getter)NestedD_Get, (Abstract *)xmlt->nd);
    if(result == NULL){
        xmlt->result = String_Init(m, STRING_EXTEND);
    }

    return xmlt;
}
