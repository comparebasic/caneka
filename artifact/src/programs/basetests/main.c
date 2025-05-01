/*
Main
Example and test runner

This file is mostly an example Caneka application, and it runs the tests
*/

#include <external.h>
#include <caneka.h>
#include <tests.h>

#include "tests/inc.c"

static status test(MemCtx *m){
    status r = READY;
    Tests_Init(m);
    r |= Test_Runner(m, "Caneka", Tests);
    return r;
}

int main(int argc, char **argv){
    if(argc > 1){
        for(int i = 1; i < argc; i++){
            char *arg = argv[i];
            if(strncmp(arg, "no-color", strlen("no-color")) == 0){
                GLOBAL_flags |= NO_COLOR;
            }
            if(strncmp(arg, "html", strlen("html")) == 0){
                GLOBAL_flags |= HTML_OUTPUT;
            }
        }
    }

    MemBook *cp = MemBook_Make(NULL);
    if(cp == NULL){
        Fatal("MemBook created successfully\n", 0);
    }

    MemCtx *m = MemCtx_Make();
    if(m == NULL){
        Fatal("MemCtx created successfully\n", 0);
    }

    Caneka_Init(m);
    test(m);

    return 0;
}
