enum iter_higher_flags {
    UFLAG_ITER_INVERT = 1 << 8,
    UFLAG_ITER_STRICT = 1 << 9,
    UFLAG_ITER_SELECTED = 1 << 10,
    UFLAG_ITER_INDENT = 1 << 11,
    UFLAG_ITER_OUTDENT = 1 << 12,
    UFLAG_ITER_LEAF = 1 << 13,
    UFLAG_ITER_NEXT = 1 << 14,
    UFLAG_ITER_SKIP = 1 << 15,
};

status IterUpper_FlagCombine(status base, status compare);
