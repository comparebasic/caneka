typedef struct fmt_line {
    Type type;
    char *fmt;
    Abstract **args;
} FmtLine;

enum formatter_types {
    FMT_TYPE_TYPED = 1 << 8,
    FMT_TYPE_STR = 1 << 9,
    FMT_TYPE_TYPEOF = 1 << 10,
    FMT_TYPE_CLS = 1 << 11,
    FMT_TYPE_VERBOSE = 1 << 12,
    FMT_TYPE_BITS = 1 << 13,
    FMT_TYPE_EXTRA_ARG = 1 << 14,
    FMT_TYPE_ANSI = 1 << 15,
};
status Fmt(struct buff *bf, char *fmt, Abstract *args[]);
FmtLine *FmtLine_FromSpan(MemCh *m, char *fmt, Span *p);
FmtLine *FmtLine_Make(MemCh *m, char *fmt, Abstract **args);
StrVec *Fmt_ToStrVec(MemCh *m, char *fmt, Abstract **args);
Abstract *FmtVar_Get(MemCh *m, Str *key, Abstract *arg);
