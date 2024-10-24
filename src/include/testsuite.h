typedef status (*TestFunc)(MemCtx *m);
status Test(int condition, char *msg, ...);
status Test_Runner(MemCtx *m, char *name, void *tests[]);
status Tests_Init(MemCtx *m);

#define TEST_PORT 1200

typedef struct req_test_spec {
    int direction;
    char *content;
    int length;
    int delay;
    int pos;
} ReqTestSpec;

#define TEST_BUFF_SIZE 1023

#define TEST_SEND SOCK_OUT
#define TEST_RECV SOCK_IN
#define TEST_SERVE_END -1 
#define TEST_DELAY_ONLY 0 

/* utils */
int ServeTests_ForkRequest(MemCtx *m, char *msg, ReqTestSpec[]);
status TestChild(int child);
status ServeTests_SpawnRequest(MemCtx *m, char *msg);

#include <tests.h>
