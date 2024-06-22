#include "external.h"
#include "filestore.h"

status Test(int condition, char *msg, ...){
	va_list args;
    va_start(args, msg);
    if(!condition){
        printf("\x1b[31mFAIL: ");
        vprintf(msg, args);
        printf("\x1b[0m\n");
        Fatal("Test Failure", TYPE_TESTSUITE);
        return ERROR;
    }else{
        printf("\x1b[32mPASS: ");
        vprintf(msg, args);
        printf("\x1b[0m\n");
        return 0;
    }
}
