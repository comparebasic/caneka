enum fetch_target_flags {
    FETCHER_TARGET_ATT = 1 << 8,
    FETCHER_TARGET_KEY = 1 << 9,
    FETCHER_TARGET_VALUE = 1 << 10,
    FETCHER_TARGET_IDX = 1 << 11,
    FETCHER_TARGET_NEST = 1 << 12,
    FETCHER_TARGET_ITER = 1 << 13,
    FETCHER_TARGET_FUNC = 1 << 14,
};

typedef struct fetch_target {
    Type type;
    union {
        i32 idx;
        Abstract *key;
        FetchFunc func;
    } val;
} FetchTarget;
