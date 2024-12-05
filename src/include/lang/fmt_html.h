enum formatter_marks {
    FORMATTER_MARK_START,
    FORMATTER_MARK_LINE,
    FORMATTER_MARK_VALUES,
    FORMATTER_MARK_END,
};

enum formatter_captures {
    FORMATTER_INDENT,
    FORMATTER_ITEM,
    FORMATTER_LINE,
    FORMATTER_METHOD,
    _FORMATTER_OUT,
    FORMATTER_VALUE,
    FORMATTER_LAST_VALUE,
    FORMATTER_NEXT,
    _FORMATTER_OUT_END,
};

enum formatter_types {
    _TYPE_FORMATTER_START = _TYPE_CORE_END,
    _TYPE_FORMATTER_END,
};

enum formatter_methods {
    FMT_UL = 1,
    FMT_TBL,
};

typedef struct formatter_ctx {
    Type type;
    MemCtx *m;
    Roebling *rbl;
    int indent;
    cls method;
    String *content;
    Lookup *methods;
    struct {
        File in;
        File out;
    } files;
} FmtHtml;

FmtCtx *FmtHtml_Make(MemCtx *m);
status Fmt_Run(MemCtx *m, char *in, char *out);
