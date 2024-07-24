extern int XML_START;
extern int XML_TAG;
extern int XML_ATTRIBUTE;
extern int XML_ATTR_VALUE;
extern int XML_BODY;


typedef struct xml_ctx {
    Type type;
    MemCtx *m;
    Mess *root;
    Mess *current;
    void *source;
} XmlCtx;

XmlCtx *XmlCtx_Make(MemHandle *mh, void *source);
Span *XmlParser_Make(MemCtx *m, ProtoDef *def);
