enum iter_higher_flags {
    UFLAG_ITER_INVERT = 1 << 8,
    UFLAG_ITER_FOCUS = 1 << 9,
    UFLAG_ITER_ACTION = 1 << 10,
    UFLAG_ITER_FINISH = 1 << 11,
    UFLAG_ITER_SKIP = 1 << 12,
    UFLAG_ITER_SIBLING = 1 << 13,
    UFLAG_ITER_LEAF = 1 << 14,
    UFLAG_ITER_ENCLOSE = 1 << 15,
};

enum iter_higher_flags_idx {
    UFLAG_ITER_INVERT_IDX = 0,
    UFLAG_ITER_FOCUS_IDX = 1,
    UFLAG_ITER_ACTION_IDX = 2,
    UFLAG_ITER_FINISH_IDX = 3,
    UFLAG_ITER_SKIP_IDX = 4,
    UFLAG_ITER_SIBLING_IDX = 5,
    UFLAG_ITER_LEAF_IDX = 6,
    UFLAG_ITER_ENCLOSE_IDX = 7,
};
void *IterUpper_Combine(MemCh *m, void *orig, void *add);
