enum xmlt_flags {
    FLAG_XML_HIDDEN = 1 << 8,
    FLAG_XML_INCLUDE = 1 << 9,
    FLAG_XML_FOR = 1 << 10,
    FLAG_XML_WITH = 1 << 11,
    FLAG_XML_IF = 1 << 12,
    FLAG_XML_IF_NOT = 1 << 13,
    FLAG_XML_SATISFIED = 1 << 14,
    FLAG_XML_IN_PROGRESS = 1 << 15,
};

typedef struct xmlt_ctx {
    Type type;
    MemCtx *m;
    Cash *cash;
    XmlParser *parser;
    NestedD *nd;
    Abstract *source;
    String *result;
} XmlTCtx;

status XmlT_AddAttsStr(XmlTCtx *xmlt, Mess *e, String *s);
status XmlT_GetPropFlags(Mess *e, String *prop, Abstract *value);
XmlTCtx *XmlT_Make(MemCtx *m, String *result, Lookup *presenters);
status XmlT_Parse(XmlTCtx *xmlt, String *s, Span *tbl);
status XmlT_Out(MemCtx *_, String *s, Abstract *source);
status XmlT_Template(XmlTCtx *xmlt, Mess *e, NestedD *nd, OutFunc func);
