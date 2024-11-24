enum xmlt_flags {
    FLAG_XML_HIDDEN = 1 << 8,
    FLAG_XML_INCLUDE = 1 << 9,
    FLAG_XML_FOR = 1 << 10,
    FLAG_XML_WITH = 1 << 11,
    FLAG_XML_IF = 1 << 12,
    FLAG_XML_IF_NOT = 1 << 13,
};

typedef struct xmlt_ctx {
    Type type;
    MemCtx *m;
    Cash *cash;
    Span *tbl;
    XmlParser *xml;
    Abstract *source;
} XmlTCtx;

status XmlT_AddAttsStr(XmlTCtx *xmlt, Mess *e, String *s);
status XmlT_Out(XmlTCtx *xmlt, Mess *e, OutFunc func);
status XmlT_GetPropFlags(Mess *e, String *prop, Abstract *value);
XmlTCtx *XmlT_Make(MemCtx *m);

