enum argflags {
    ARG_DEFAULT = 1 << 8,
    ARG_OPTIONAL = 1 << 9,
    ARG_MULTIPLE = 1 << 10,
    ARG_ABS_PATH = 1 << 11,
    ARG_CHOICE = 1 << 12,
};

status CharPtr_ToTbl(MemCh *m, Table *resolve, int argc, char **argv, Table *dest);
status CharPtr_ToHelp(MemCh *m, Str *name, Table *resolve, int argc, char **argv);
status Args_Add(Table *resolve, Str *key, Abstract *value, word flags);
