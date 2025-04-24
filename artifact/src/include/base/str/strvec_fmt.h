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

i64 StrVec_FmtHandle(Stream *sm, char *fmt, Single args[]);
i64 StrVec_Fmt(Stream *sm, char *fmt, Single args[]);
