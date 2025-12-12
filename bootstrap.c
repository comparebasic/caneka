/* = Bootstrap Program
 * 
 * This is a small bootstrap program, for launching the rest of the build process.
 *
 * A small menu displays if no other arguments are given.
 *
 * The rest of the build process can be found at src/programs/cnkbuild/
 *
 * see ./LICENCE for licene details
 *
 */
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

char *buildCaneka[] = {
    "./build/bin/cnkbuild",
    "--src",
    "src/inter",
    NULL
};

char *buildTests[] = {
    "./build/bin/cnkbuild",
    "--src",
    "src/programs/test",
    NULL
};

char *buildWebServer[] = {
    "./build/bin/cnkbuild",
    "--src",
    "src/programs/webserver",
    NULL
};

char *buildCli[] = {
    "./build/bin/cnkbuild",
    "--src",
    "src/programs/cnkcli",
    NULL
};

char *runTests[] = {
    "./build/bin/test",
    NULL
};

char *runWebServer[] = {
    "./build/bin/webserver",
    "--config",
    "examples/web-server/default.config",
    NULL
};

char *menuKeys[] = {
    "run-tests",
    "cnkbuild-only",
    "build-only",
    "clean",
    "build-cli",
    "build-webserver",
    "build-run-webserver",
    "read-documentation",
    "all",
    "oops",
    NULL
};

char *menuOptions[] = {
    "Tests - build and RUN",
    "CnkBuild - build the CnkBuild program only",
    "Caneka (core) - build",
    "Clean (the ./build directory)",
    "CnkCli - build",
    "Webserver - build",
    "WebServer - build and RUN",
    "Documentation - show website url",
    "All - build core/Tests/Cli/Webserver",
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

    printf("\n  Type a number or press Enter (default is 1)\n\n");

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
    char **runcmd[4];

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

    if(compareCstr("cnkbuild-only", choice)){
        runcmd[0] = NULL;
    }else if(compareCstr("run-tests", choice)){
        runcmd[0] = buildTests;
        runcmd[1] = runTests;
        runcmd[2] = NULL;
    }else if(compareCstr("build-only", choice)){
        runcmd[0] = buildCaneka;
        runcmd[1] = NULL;
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
        runcmd[0] = buildCli;
        runcmd[1] = NULL;
    }else if(compareCstr("build-webserver", choice)){
        runcmd[0] = buildWebServer;
        runcmd[1] = NULL;
    }else if(compareCstr("build-run-webserver", choice)){
        runcmd[0] = buildWebServer;
        runcmd[1] = runWebServer;
        runcmd[2] = NULL;
    }else if(compareCstr("read-documentation", choice)){
        printf("  Documentation can be found at https://caneka.org\n");
        exit(1);
    }else if(compareCstr("all", choice)){
        runcmd[0] = buildTests;
        runcmd[1] = buildCli;
        runcmd[2] = buildWebServer;
        runcmd[3] = NULL;
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

    cmd[0] = _gen_CC;
    cmd[1] = "-g";
    cmd[2] = "-Isrc/programs/cnkbuild/include";
    cmd[3] = "-Isrc/base/include";
    cmd[4] = "-Isrc/third/include";
    cmd[5] = "-Iprograms/cnkbuild/include";
    cmd[6] = "-o";
    cmd[7] = "./build/bin/cnkbuild";
    cmd[8] = "./build/libcnkbase/libcnkbase.a";
    cmd[9] = "./src/programs/cnkbuild/inc.c";
    cmd[10] = "-lm";
    cmd[11] =  NULL;

    if(run("Building cnkbuild", cmd) == -1){
        exit(1);
    }

    char ***cmdptr = runcmd;
    while(*cmdptr != NULL){
        char **torun = *cmdptr;
        if(run("Running cmd", torun) == -1){
            exit(1);
        }
        cmdptr++;
    }

    if(compareCstr("build-only", choice)){
        printf("Caneka has been built!\n");
        fflush(stdout);
    }else if(compareCstr("clean", choice)){
        printf("Cleaned!\n");
        fflush(stdout);
    }else if(compareCstr("build-cli", choice)){
        printf("The Cli has been built at ./build/bin/cnkcli\n");
        fflush(stdout);
    }else if(compareCstr("build-webserver", choice)){
        printf("The WebServer has been built at ./build/bin/webserver\n");
        fflush(stdout);
    }else if(compareCstr("all", choice)){
        printf("The Test,WebServer and Cli are in the ./build/bin folder\n");
        fflush(stdout);
    }

    return 0;
}
