extern int XML_START;
extern int XML_TAG;
extern int XML_ATTRIBUTE;
extern int XML_ATTR_VALUE;
extern int XML_BODY;
extern int XML_END;

typedef struct xml_ctx {
    Type type;
    MemCtx *m;
    int count;
    Mess *root;
    Mess *current;
    void *source;
} XmlCtx;

XmlCtx *XmlCtx_Make(MemHandle *mh, void *source);
Span *XmlParser_Make(MemCtx *m, ProtoDef *def);
status XmlCtx_Open(XmlCtx *ctx, String *tagName);
status XmlCtx_Close(XmlCtx *ctx, String *tagName);
