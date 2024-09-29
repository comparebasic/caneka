enum xml_marks {
    XML_START = 1,
    XML_ATTROUTE,
    XML_ATTR_VALUE,
    XML_END,
};

typedef struct xml_ctx {
    Type type;
    MemCtx *m;
    int count;
    Mess *root;
    Mess *current;
    Mess *parent;
    void *source;
} XmlCtx;

XmlCtx *XmlCtx_Make(MemHandle *mh, void *source);
Span *XmlParser_Make(MemCtx *m);
status XmlCtx_Open(XmlCtx *ctx, String *tagName);
status XmlCtx_Close(XmlCtx *ctx, String *tagName);
status XmlCtx_SetAttr(XmlCtx *ctx, String *attName);
status XmlCtx_SetAttrValue(XmlCtx *ctx, Abstract *value);
status XmlCtx_TagClosed(XmlCtx *ctx);
status XmlCtx_BodyAppend(XmlCtx *ctx, String *body);
