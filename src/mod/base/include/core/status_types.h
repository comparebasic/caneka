extern word NORMAL_FLAGS;
extern word UPPER_FLAGS;
extern word GLOBAL_flags;
extern word OUTCOME_FLAGS;

enum status_types {
    READY = 0,
    ERROR = 1,
    PROCESS = 1 << 1,
    FOCUS = 1 << 2,
    NOOP = 1 << 3,
    MORE = 1 << 4,
    WAIT = 1 << 5,
    TAIL = 1 << 6,
    END = 1 << 7,
    /* class speciric */
    CLS_FLAG_ALPHA = 1 << 8,
    CLS_FLAG_BRAVO = 1 << 9,
    CLS_FLAG_CHARLIE = 1 << 10,
    CLS_FLAG_DELTA = 1 << 11,
    CLS_FLAG_ECHO = 1 << 12,
    CLS_FLAG_FOXTROT = 1 << 13,
    CLS_FLAG_GOLF = 1 << 14,
    CLS_FLAG_HOTEL = 1 << 15,
};
