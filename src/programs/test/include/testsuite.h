#define TEST_BUFF_SIZE 1023

#define TEST_SEND SOCK_OUT
#define TEST_RECV SOCK_IN
#define TEST_SERVE_END -1 
#define TEST_DELAY_ONLY 0 

#define TEST_MEM_MAX_CEILING 512

enum test_types {
    _TYPE_TEST_START = _TYPE_CANEKA_CORE_END,
    TYPE_TEST_SUITE,
};

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

typedef struct test_set {
    char *name;
    TestFunc func; 
    char *description;
    word status;
} TestSet;

typedef struct test_suite {
    Type type; 
    Str *name;
    i32 pass;
    i32 fail;
    TestSet *set;
} TestSuite;

typedef struct req_test_spec {
    int direction;
    char *content;
    int length;
    int delay;
    int pos;
    /* meta */
} ReqTestSpec;

status Test(boolean condition, char *fmt, void *args[]);
status TestShow(boolean condition, char *fmtSuccess, char *fmtError, void *args[]);
status Test_Runner(MemCh *gm, TestSuite *suite);
Str *Test_GetStr512(MemCh *m);

TestSuite *TestSuite_Make(MemCh *m, Str *name, TestSet *set);
