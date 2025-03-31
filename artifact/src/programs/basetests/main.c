/*
Main
Example and test runner

This file is mostly an example Caneka application, and it runs the tests
*/

#include <external.h>
#include <caneka.h>
#include <tests.h>

#include "tests/inc.c"

static status test(MemCh *m){
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

    printf("MemBook Creating\n");
    MemBook *cp = MemBook_Make(NULL);
    if(cp == NULL){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "MemBook created successfully");
    }

    printf("MemChapter Creating\n");
    MemCh *m = MemCh_Make();
    if(m == NULL){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "MemCh created successfully");
    }

    printf("Caneka_Init running\n");
    Caneka_Init(m);
    printf("Testing\n");
    test(m);

    return 0;
}
