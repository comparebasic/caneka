enum xml_marks {
    XML_START = 1,
    XML_ATTROUTE,
    XML_ATTR_VALUE,
    XML_END,
    _XML_MAX = 1 << 15
};

enum xml_captures {
    XML_CAPTURE_OPEN = 1,
    XML_CAPTURE_TAG_ENDED,
    XML_CAPTURE_CLOSE_TAG,
    XML_CAPTURE_TAG_SELFCLOSED,
    XML_CAPTURE_TAG_WHITESPACE_BODY,
    XML_CAPTURE_BODY,
    XML_CAPTURE_BODY_CASH,
    XML_CAPTURE_ATTRIBUTE,
    _XML_CAPTURE_VALUE_START,
    XML_CAPTURE_ATTR_VALUE,
    XML_CAPTURE_ATTR_VALUE_CASH,
    XML_CAPTURE_ATTR_VALUE_NUM,
    _XML_CAPTURE_VALUE_END,
    _XML_CAPTURE_MAX = 1 << 15
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

XmlCtx *XmlCtx_Make(MemHandle *mh);
status XmlCtx_Open(XmlCtx *ctx, String *tagName);
status XmlCtx_Close(XmlCtx *ctx, String *tagName);
status XmlCtx_SetAttr(XmlCtx *ctx, String *attName);
status XmlCtx_SetAttrValue(XmlCtx *ctx, Abstract *value);
status XmlCtx_TagClosed(XmlCtx *ctx);
status XmlCtx_BodyAppend(XmlCtx *ctx, String *body);

typedef struct xml_parser {
    Type type;
    Roebling *rbl;
    XmlCtx *ctx;
} XmlParser;

XmlParser *XmlParser_Make(MemCtx *m);
status XmlParser_Parse(XmlParser *xml, String *s);
