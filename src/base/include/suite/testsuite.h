enum test_status {
    SECTION_LABEL = 1 << 8,
    NOT_STARTED = 1 << 9,
    PREVIOUSLY_WORKING = 1 << 10,
    PARTIAL_FEATURE = 1 << 11,
    FEATURE_COMPLETE = 1 << 12,
    PRE_PRODUCTION = 1 << 13,
    PRODUCTION = 1 << 14,
    PERMANCE_TESTED = 1 << 15,
};

typedef status (*TestFunc)(MemCh *m);

typedef struct req_test_spec {
    int direction;
    char *content;
    int length;
    int delay;
    int pos;
    /* meta */
} ReqTestSpec;

typedef struct test_set {
    char *name;
    TestFunc func; 
    char *description;
    word status;
} TestSet;


#define TEST_BUFF_SIZE 1023

#define TEST_SEND SOCK_OUT
#define TEST_RECV SOCK_IN
#define TEST_SERVE_END -1 
#define TEST_DELAY_ONLY 0 

#define TEST_MEM_MAX_CEILING 256

status Test(boolean condition, char *fmt, void *args[]);
status TestShow(boolean condition, char *fmtSuccess, char *fmtError, void *args[]);
status Test_Runner(MemCh *gm, char *suiteName, TestSet *tests);

Str *Test_GetStr512(MemCh *m);
