typedef status (*TestFunc)(MemCtx *m);
status Test(int condition, char *msg, ...);
status Test_Runner(MemCtx *m, char *name, void *tests[]);
#include <tests.h>
