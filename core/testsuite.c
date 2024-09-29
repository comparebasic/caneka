#include <external.h>
#include <caneka.h>

#define NO_COLOR RAW

status Test(int condition, char *msg, ...){
	va_list args;
    va_start(args, msg);
    if(!condition){
        if(!HasFlag(GLOBAL_flags, NO_COLOR)){
            printf("\x1b[31m");
        }
        printf("FAIL: ");
        vprintf(msg, args);
        if(!HasFlag(GLOBAL_flags, NO_COLOR)){
            printf("\x1b[0m");
        }
        printf("\n");
        return ERROR;
    }else{
        if(!HasFlag(GLOBAL_flags, NO_COLOR)){
            printf("\x1b[32m");
        }
        printf("PASS: ");
        vprintf(msg, args);
        if(!HasFlag(GLOBAL_flags, NO_COLOR)){
            printf("\x1b[0m");
        }
        printf("\n");
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
    if(HasFlag(GLOBAL_flags, NO_COLOR)){
        printf("Suite %s pass:%d fail:%d\n", suiteName, pass, fail);
    }else{
        if(!fail){
            printf("\x1b[%dmSuite %s \x1b[1;%dmpass:%d\x1b[0;%dm fail:%d\x1b[0m\n", COLOR_GREEN, suiteName, COLOR_GREEN, pass, COLOR_GREEN, fail);
        }else{
            printf("\x1b[%dmSuite %s pass:%d \x1b[1;%dmfail:%d\x1b[0m\n", COLOR_RED, suiteName, pass, COLOR_RED,  fail);
        }
    }
    return !fail ? SUCCESS : MISS;
}
