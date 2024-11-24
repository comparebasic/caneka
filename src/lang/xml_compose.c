#include <basictypes_external.h> 
#include <caneka/caneka.h> 

lifecycle_t XmlCompose_Include(XmlCtx *ctx, XmlElem *e, String *prop_s, Typed *value){
    return NOOP;
}

lifecycle_t XmlCompose_Continue(MemCtx *m, XmlCtx *ctx, String *s){
    int count = 0;
    while(!IsDone(s) && !IsDone(ctx)){
        if(++count > XML_MAX_LOOPS){
            printf("Error: XML_MAX_LOOPS exceeded\n");
            ctx->lifecycle = ERROR;
            break;
        }
        String_FeedChar(m, (Typed *)ctx, s, Xml_FeedChar);
        if(DEBUG_XML_COMPOSE){
            String_PrintColor(String_FormatV(m, "DONE", NULL), DEBUG_XML_COMPOSE);    
        }
    }
    return ctx->lifecycle;
}

XmlElem *XmlCompose_Run(MemCtx *m, String *s, RefFunc include, Typed *ref){
    XmlCompose *ec = XmlCompose_Make(m, include, ref);
    XmlCtx *ctx = XmlCtx_Make(m, onCycle, (Typed *)ec);
    while(!IsDone(ctx) && !IsDone(s)){
        XmlCompose_Continue(m, ctx, s);
    }
    return ec->rootElem_el;

    return NULL;
}

XmlCompose *XmlCompose_New(MemCtx *m){
	XmlCompose *ec = (XmlCompose*)Alloc(m, sizeof(XmlCompose));

    ec->m = m;
    ec->type = TYPE_XML_COMPOSER;

    return ec;
}

XmlCompose *XmlCompose_Make(MemCtx *m, RefFunc include, Typed *ref){
    XmlCompose *ec =  XmlCompose_New(m); 
    ec->include = include;
    ec->ref = ref;

    return ec;
}
