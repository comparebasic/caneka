enum xml_marks {
    XML_START = 1,
    XML_TAG,
    XML_ATTROUTE,
    XML_ATTRIBUTE,
    XML_ATTR_VALUE,
    XML_BODY,
    XML_END,
};

typedef struct xml_ctx {
    Type type;
    MemCtx *m;
    int count;
    Mess *root;
    Mess *current;
    void *source;
} XmlCtx;

XmlCtx *XmlCtx_Make(MemHandle *mh, void *source);
Span *XmlParser_Make(MemCtx *m);
status XmlCtx_Open(XmlCtx *ctx, String *tagName);
status XmlCtx_Close(XmlCtx *ctx, String *tagName);
status XmlCtx_SetAttr(XmlCtx *ctx, String *attName);
