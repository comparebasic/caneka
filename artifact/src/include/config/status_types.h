extern word NORMAL_FLAGS;
extern word UPPER_FLAGS;
extern word GLOBAL_flags;

enum status_types {
    READY = 0,
    SUCCESS = 1,
    ERROR = 1 << 1,
    NOOP = 1 << 2,
    DEBUG = 1 << 3,
    MORE = 1 << 4,
    LOCAL_PTR = 1 << 5,
    END = 1 << 6,
    PROCESSING = 1 << 7,
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
