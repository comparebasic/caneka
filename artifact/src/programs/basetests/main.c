/*
Main
Example and test runner

This file is mostly an example Caneka application, and it runs the tests
*/

#include <external.h>
#include <caneka.h>
#include <tests.h>

#define servecmd "serve"
#define testcmd "test"
#define transcmd "transp"
#define doccmd "doc"

static status test(MemCtx *m, char *arg){
    status r = READY;
    word flags = 0;
    if(strncmp(arg, "test=no-color", strlen("test=no-color")) == 0){
        GLOBAL_flags |= NO_COLOR;
    }
    if(strncmp(arg, "test=html", strlen("test=html")) == 0){
        GLOBAL_flags |= HTML_OUTPUT;
    }
    Tests_Init(m);
    Test_Runner(m, "Caneka", Tests);
    return r;
}

static status handle(MemCtx *m, char *arg){
    if(strncmp(arg, testcmd, strlen(testcmd)) == 0){
        return test(m, arg);
    }

    return NOOP;
}

void Cleanup(Abstract *m){
    return;
}

int main(int argc, char **argv){
    if(argc == 1){
        printf("%s [test,serve=port,trans]\n", argv[0]);
        exit(1);
    }

    if(MemBook_Make(NULL) == NULL){
        Fatal("Unable to allocate Mem_Book", TYPE_BOOK);
        exit(1);
    };

    MemCtx *m = MemCtx_Make();
    Caneka_Init(m);
    
    if(argc > 1){
        for(int i = 1; i < argc; i++){
            if(handle(m, argv[i]) == ERROR){
                exit(1);
            }
        }
    }

    return 0;
}
