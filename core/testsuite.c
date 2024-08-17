#include <external.h>
#include <caneka.h>

status Test(int condition, char *msg, ...){
	va_list args;
    va_start(args, msg);
    if(!condition){
        printf("\x1b[31mFAIL: ");
        vprintf(msg, args);
        printf("\x1b[0m\n");
        return ERROR;
    }else{
        printf("\x1b[32mPASS: ");
        vprintf(msg, args);
        printf("\x1b[0m\n");
        return SUCCESS;
    }
}

status Test_Runner(MemCtx *m, char *suiteName, void *tests[]){
    int i = 0;
    char *name = (char *)tests[i];
    TestFunc func = (TestFunc)tests[i+1];
    int pass = 0;
    int fail = 0;
    while(name != NULL){
        printf("[Testing %s]\n", name);
        if(func(m) != SUCCESS){
            fail++;
            break;
        }
        pass++;
        i+= 2;
        name = (char *)tests[i];
        func = (TestFunc)tests[i+1];
    }
    if(!fail){
        printf("\x1b[%dSuite %s \x1b[1;%dmpass:%d\x1b[0;%dm fail:%d\x1b[0m\n", COLOR_GREEN, suiteName, COLOR_GREEN, pass, COLOR_GREEN, fail);
        return SUCCESS;
    }else{
        printf("\x1b[%dmSuite %s pass:%d \x1b[1;%dmfail:%d\x1b[0m\n", COLOR_RED, suiteName, pass, COLOR_RED,  fail);
        return MISS;
    }
}
