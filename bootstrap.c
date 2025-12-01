#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include "src/programs/cnkbuild/include/detect.h"

static char *NORMAL_COLOR = "\x1b[0m";
static char *GREEN = "\x1b[32m";
static char *YELLOW = "\x1b[33m";
static char *RED = "\x1b[31m";

static void setNoColor(){
    NORMAL_COLOR = GREEN = YELLOW = RED = "";
}

static int compareCstr(const char *choice, char *content){
    return strncmp(choice, content, strlen(choice)) == 0;
}

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
    char *runcmd[4];

    int choice = 0;
    for(int i = 1; i < argc; i++){
        if(compareCstr("--quiet", argv[i])){
            setNoColor();
            choice = -1;
        }
    }

    if(choice >= 0){
        printf("%s%s%s", YELLOW,"\n  Hello, and welcome to the Caneka Build Helper\n", NORMAL_COLOR);
        printf("  What would you like to do?\n"
        "    [1] Build and RUN Caneka and it's tests\n"
        "     2  Build Caneka only\n"
        "     3  Clean the ./build directory\n"
        "     4  Build the CnkCli tools\n"
        "     5  Build the example WebServer\n"
        "     6  Build and RUN the example WebServer\n"
        "     7  Read some Documentation for Caneka\n"
        "     8  Oops, Wrong Command\n"
        "\n"
        "  Type a number or press Enter (default is 1)\n"
        "\n");

        char buff[2];
        read(0, buff, 2);
        
        if(compareCstr("1", buff)){
            runcmd[0] = "./build/bin/tests";
            runcmd[1] = NULL;
        }else if(compareCstr("2", buff)){
            runcmd[0] = NULL;
        }else if(compareCstr("3", buff)){
            printf("  Not yet implemented\n");
            exit(1);
        }else if(compareCstr("4", buff)){
            printf("  Not yet implemented\n");
            exit(1);
        }else if(compareCstr("5", buff)){
            printf("  Not yet implemented\n");
            exit(1);
        }else if(compareCstr("6", buff)){
            printf("  Not yet implemented\n");
            exit(1);
        }else if(compareCstr("7", buff)){
            printf("  Documentation can be found at https://caneka.org\n");
            exit(1);
        }else if(compareCstr("8", buff)){
            printf("%s%s%s", GREEN, "  Ok, See you next time!\n", NORMAL_COLOR);
            exit(1);
        }
    }

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
