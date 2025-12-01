#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "src/programs/cnkbuild/include/detect.h"

pid_t run(char *msg, char *args[]){
    printf("Subprocess - %s: ", msg);
    char **ptr = args;
    while(*ptr != NULL){
        printf("%s", *ptr); 
        if(*(ptr+1) != NULL){
            printf(" ");
        }
        ptr++;
    }
    printf("\n");

    pid_t child = vfork();
    if(child == (pid_t)-1){
        printf("Error forking\n");
        exit(1);
    }else if(!child){
        execvp(args[0], args);
        exit(0);
        return -1;
    }

    int r;
    pid_t pid = 0;
    while(1){
        pid = waitpid(child, &r, 0);
        if(pid == child){
            if(!WIFEXITED(r)){
                printf("Subprocess %d failed for or did not exit properly", pid);
                return -1;
            }else{
                break;
            }
        }
        struct timespec remain;
        struct timespec ts = {0, 1000000};
        nanosleep(&ts, &remain);
    }

    int code = WEXITSTATUS(r);
    if(code == 0){
        return 0;
    }else{
        printf("Subprocess %d exited non-zero %d", pid, code);
        return -1;
    }

    return -1;
}

int main(int argc, char *argv[]){
    char *cmd[12];

    cmd[0] = "mkdir";
    cmd[1] = "-p";
    cmd[2] = "build/libcnkbase";
    cmd[3] =  NULL;

    if(run("Making build dir", cmd) == -1){
        exit(1);
    }

    cmd[0] = "mkdir";
    cmd[1] = "-p";
    cmd[2] = "build/include";
    cmd[3] =  NULL;

    if(run("Making build include directory", cmd) == -1){
        exit(1);
    }

    cmd[0] = "mkdir";
    cmd[1] = "-p";
    cmd[2] = "build/bin";
    cmd[3] =  NULL;

    if(run("Making build bin directory", cmd) == -1){
        exit(1);
    }

    cmd[0] = _gen_CC;
    cmd[1] = "-g";
    cmd[2] = "-Isrc/base/include";
    cmd[3] = "-Isrc/third/include";
    cmd[4] = "-c";
    cmd[5] = "-o";
    cmd[6] = "./build/libcnkbase/libcnkbase.a";
    cmd[7] = "./src/base/inc.c";
    cmd[8] =  NULL;

    if(run("Building base", cmd) == -1){
        exit(1);
    }

    if(run("Building base", cmd) == -1){
        exit(1);
    }

    cmd[0] = _gen_CC;
    cmd[1] = "-g";
    cmd[2] = "-Isrc/programs/cnkbuild/include";
    cmd[3] = "-Isrc/base/include";
    cmd[4] = "-Isrc/third/include";
    cmd[5] = "-o";
    cmd[6] = "./build/bin/cnkbuild";
    cmd[7] = "./build/libcnkbase/libcnkbase.a";
    cmd[8] = "./src/programs/cnkbuild/inc.c";
    cmd[9] = "-lm";
    cmd[10] =  NULL;

    if(run("Building cnkbuild", cmd) == -1){
        exit(1);
    }

    cmd[0] = "./build/bin/cnkbuild";
    cmd[1] = "--quiet";
    cmd[2] = "--src";
    cmd[3] = "src/programs/test";
    cmd[4] = NULL;

    if(run("Running cnkbuild", cmd) == -1){
        exit(1);
    }

    return 0;
}
