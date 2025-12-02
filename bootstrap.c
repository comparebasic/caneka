#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <dirent.h>
#include "src/programs/cnkbuild/include/detect.h"

#define TRUE 1
#define FALSE 0

char *menuKeys[] = {
    "run-tests",
    "build-only",
    "clean",
    "build-cli",
    "build-webserver",
    "build-run-webserver",
    "read-documentation",
    "oops",
    NULL
};

char *menuOptions[] = {
    "Build and RUN Caneka and it's tests",
    "Build Caneka only",
    "Clean the ./build directory",
    "Build the CnkCli tools",
    "Build the example WebServer",
    "Build and RUN the example WebServer",
    "Read some Documentation for Caneka",
    "Oops, Wrong Command",
    NULL
};

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

int cleanDir(char *path){
    struct dirent *ent;
    DIR *d = opendir(path);
    char buff[512];
    ssize_t pos = strlen(path);
    memset(buff, 0, 512);
    memcpy(buff, path, pos);
    if(d != NULL){
        char *dir = NULL;
        while((ent = readdir(d)) != NULL){
            if(ent->d_name[0] == '.' && (ent->d_name[0] == '.' || ent->d_name[0] == '0')){
                continue;
            }
            ssize_t len = strlen(ent->d_name);
            ssize_t length = strlen(ent->d_name);
            if(pos+length+1 > 512){
                return 1;
            }
            memcpy(buff+pos, "/", 1);
            memcpy(buff+pos+1, ent->d_name, length);
            if(ent->d_type == DT_DIR){
                if(!cleanDir(buff)){
                    return FALSE;
                }
                if(buff[0] != '\0'){
                    printf("rmdir %s\n", buff);
                    if(rmdir(buff) != 0){
                        return FALSE;
                    }
                }
            }else{
                printf("unlink %s\n", buff);
                if(unlink(buff) != 0){
                    return FALSE;
                }
            }
            memset(buff+pos, 0, length+1);
        }
        closedir(d);
        return TRUE;
    }else{
        return FALSE;
    }
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

char *menu(){
    printf("%s%s%s", YELLOW,"\n  Hello, and welcome to the Caneka Build Helper\n", NORMAL_COLOR);
    printf("  What would you like to do?\n\n");

    int i = 1;
    char **opt = menuOptions;
    while(*opt != NULL){
        printf("    %d %s\n", i, *opt);
        opt++; 
        i++;
    }

    printf("  Type a number or press Enter (default is 1)\n\n");

    char buff[2];
    read(0, buff, 2);
    int choice = atoi(buff);
    if(choice > 0 && choice < i-1){
        return menuKeys[choice-1];
    }
    return NULL;
}

int main(int argc, char *argv[]){
    char *cmd[12];
    char *runcmd[4];

    char *choice = menuKeys[0];
    if(argc < 2){
        choice = menu();
        if(choice == NULL){
            choice = menuKeys[0];
        }
    }else{
        for(int i = 1; i < argc; i++){
            if(compareCstr("--clean", argv[i])){
                setNoColor();
                choice = "clean";
            }
        }
    }

    if(compareCstr("run-tests", choice)){
        runcmd[0] = "./build/bin/tests";
        runcmd[1] = NULL;
    }else if(compareCstr("build-only", choice)){
        runcmd[0] = NULL;
    }else if(compareCstr("clean", choice)){
        char buff[512];
        memset(buff, 0, 512);
        char *cstr = getcwd((char *)buff, 512);
        char *dirname = "build";
        ssize_t length = strlen(cstr);
        ssize_t dirlength = strlen(dirname);
        if(length + dirlength + 1 <  512 ){
            memcpy(buff+length, "/", 1);
            memcpy(buff+length+1, dirname, dirlength);
        }
        printf("%sRemoving build dir %s%s\n",YELLOW, buff, NORMAL_COLOR);
        if(!cleanDir(buff)){
            printf("%sError Removing build dir %s%s\n", RED, buff, NORMAL_COLOR);
            exit(1);
        }
        exit(0);
    }else if(compareCstr("build-cli", choice)){
        printf("  Not yet implemented\n");
        exit(1);
    }else if(compareCstr("build-webserver", choice)){
        printf("  Not yet implemented\n");
        exit(1);
    }else if(compareCstr("build-run-webserver", choice)){
        printf("  Not yet implemented\n");
        exit(1);
    }else if(compareCstr("read-documentation", choice)){
        printf("  Documentation can be found at https://caneka.org\n");
        exit(1);
    }else if(compareCstr("oops", choice)){
        printf("%s%s%s", GREEN, "  Ok, See you next time!\n", NORMAL_COLOR);
        exit(1);
    }else{
        printf("%s%s%s", RED, "  Unexpected choice.\n", NORMAL_COLOR);
        exit(1);
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
    cmd[1] = "--src";
    cmd[2] = "src/programs/test";
    cmd[3] = NULL;

    if(run("Running cnkbuild", cmd) == -1){
        exit(1);
    }

    return 0;
}
