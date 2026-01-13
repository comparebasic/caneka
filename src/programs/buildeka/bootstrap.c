/* = Bootstrap Program
 * 
 * This is a small bootstrap program, for launching the rest of the build process.
 *
 * A small menu displays if no other arguments are given.
 *
 * The rest of the build process can be found at src/programs/buildeka/
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
#include <termios.h>
#include <signal.h>
#include "detect.h"

static int change = 0;
static struct termios orig_tios;
static struct termios current_tios;

void RawMode(int enable);

#define TRUE 1
#define FALSE 0

int selected = 0;
int height = 0;

char *buildekaExamples = ""
    "\x1b[32mBuildeka has been built at \x1b[1m./dist/bin/buildeka\x1b[22m!\n\x1b[0m"
    "\n"
    "  Example build commands are:\n"
    "\n"
    "    Build the \x1b[1mTest Program\x1b[22m\n"
    "\n"
    "    \x1b[1m$\x1b[22m \x1b[33m./dist/bin/buildeka --src src/programs/test --option base ext inter\x1b[0m\n"
    "\n"
    "    With NaCl for crypto:\n"
    "\n"
    "    \x1b[1m$\x1b[22m \x1b[33m./dist/bin/buildeka --src src/programs/test --option base ext inter crypto@third/nacl\x1b[0m\n"
    "\n"
    "    With OpenSsl for crypto (experimental):\n"
    "\n"
    "    \x1b[1m$\x1b[22m \x1b[33m./dist/bin/buildeka --src src/programs/test --option base ext inter crypto@third/openssl\x1b[0m\n"
    "\n"
    "    Build the \x1b[1mClineka\x1b[22m command line tool\n"
    "\n"
    "    \x1b[1m$\x1b[22m \x1b[33m./dist/bin/buildeka --src src/programs/clineka --option [crypto@third/openssl, crypto@third/nacl]\x1b[0m\n"
    "\n"
    "    Build the \x1b[1mWebServer\x1b[22m\n"
    "\n"
    "    \x1b[1m$\x1b[22m \x1b[33m./dist/bin/buildeka --src src/programs/webserver\x1b[0m\n"
    "\n"
    "  Each program, once built, will be placed in the \x1b[1m./build/bin/\x1b[22m directory, help is availabile\n"
    "  by passing a \x1b[1m--help\x1b[22m argument, for example:\n"
    "\n"
    "    \x1b[1m$\x1b[22m ./build/bin/webserver \x1b[1m--help\x1b[0m\n"
    "    \x1b[1m$\x1b[22m ./build/bin/test \x1b[1m--help\x1b[0m\n"
    "    \x1b[1m$\x1b[22m ./build/bin/clineka \x1b[1m--help\x1b[0m\n"
    "    \x1b[1m$\x1b[22m ./dist/bin/buildeka \x1b[1m--help\x1b[0m\n"
    "\n"
    ;


char *buildCaneka[] = {
    "./dist/bin/buildeka",
    "--src",
    "src/inter",
    NULL
};

char *buildTests[] = {
    "./dist/bin/buildeka",
    "--src",
    "src/programs/test",
    "--option",
    "base",
    "ext",
    "inter",
    NULL
};

char *buildWebServer[] = {
    "./dist/bin/buildeka",
    "--src",
    "src/programs/webserver",
    "--option",
    "base",
    "ext",
    "inter",
    NULL
};

char *buildCli[] = {
    "./dist/bin/buildeka",
    "--src",
    "src/programs/clineka",
    "--option",
    "base",
    "ext",
    "inter",
    "crypto@third/nacl",
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
    "buildeka-only",
    "run-tests",
    "build-only",
    "clean",
    "build-cli",
    "build-webserver",
    "build-run-webserver",
    "read-documentation",
    "all",
    "invalid",
    NULL
};

char *menuOptions[] = {
    "Buildeka - build the builder program only",
    "Tests - build and RUN",
    "Caneka (core) - build the core modules for caneka",
    "Clean (the ./build directory)",
    "Clineka - build the command line tool",
    "Webserver - build",
    "WebServer - build and RUN",
    "Documentation - show website url",
    "All - build Tests/Clineka/Webserver",
    NULL
};

static char *NORMAL_COLOR = "\x1b[0m";
static char *GREEN = "\x1b[32m";
static char *YELLOW = "\x1b[33m";
static char *RED = "\x1b[31m";

static struct sigaction _a;
static struct sigaction _b;

static void cleanup(int sig, siginfo_t *info, void *ptr){
    RawMode(0);
    exit(1);
}

static void setSigs(){
    memset(&_a, 0, sizeof(struct sigaction));
    _a.sa_flags = SA_NODEFER;
    _a.sa_sigaction = cleanup;
    sigaction(SIGSEGV, &_a, NULL);

    memset(&_b, 0, sizeof(struct sigaction));
    _b.sa_flags = SA_NODEFER;
    _b.sa_sigaction = cleanup;
    sigaction(SIGINT, &_b, NULL);
}

static void setNoColor(){
    NORMAL_COLOR = GREEN = YELLOW = RED = "";
}

static int compareCstr(const char *choice, char *content){
    return strncmp(choice, content, strlen(choice)) == 0;
}

void RawMode(int enable){
    if(enable){
       int r = tcgetattr(STDIN_FILENO, &current_tios);
       if(r != -1){
           if(!change){
               memcpy(&orig_tios, &current_tios, sizeof(struct termios));
           }
           current_tios.c_lflag &= ~(ICANON|ISIG|ECHO);
           current_tios.c_cc[VMIN] = 1;
           current_tios.c_cc[VTIME] = 0;
       }
   }else{
       printf("RAW MODE FALSE\n");
       if(!change){
            return;
       }
       memcpy(&current_tios, &orig_tios, sizeof(struct termios));
       current_tios.c_lflag |= ECHO;
   }

   change = TRUE;
   tcsetattr(STDIN_FILENO, TCSAFLUSH, &current_tios);
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

start:
    if(height > 0){
        printf("\x1b[%dA", height);
    }

    printf("%s%s%s", YELLOW,"\n  Hello, and welcome to the Caneka Build Helper\n", NORMAL_COLOR);
    printf("  What would you like to do?\n\n");
    height = 4;

    int i = 0;
    int max = 0;
    char **opt = menuOptions;
    while(*opt != NULL){
        if(i == selected){
            printf("   \x1b[1;7m %d %s \x1b[0m\n", i+1, *opt);
        }else{
            printf("    %d %s  \n", i+1, *opt);
        }

        height++;
        opt++; 
        i++;
    }
    max = i;

    printf("\n  Type a number or use arrow/vi keys to make a selection, press Enter to build/run\n\n");
    height += 3;

    char buff[3] = {0, 0, 0};
    char *b = buff;

read:
    read(0, b, 1);
    char c = *b;
    if(c == 3 || c == 4){ /* ^c ^d */
        cleanup(0, NULL, NULL);
        return NULL;
    }else if(c == '\n'){
        if(selected >= 0 && selected <= i){
            return menuKeys[selected];
        }
        return menuKeys[i-1];
    }else if(c == 'k' || c == 65){ /* arrow-up */
        selected--;
        if(selected < 0){
            selected = 0;
        }
        goto start;
    }else if(c == 'j' || c == 66){ /* arrow-down */
        selected++;
        if(selected > i){
            selected = i;
        }
        goto start;
    }else if(c >= '1' && c <= '9'){
        selected = c-'0'-1;
        goto start;
    }

    goto read;
    return NULL;
}

int main(int argc, char *argv[]){
    char *cmd[12];
    char **runcmd[4];

    char *choice = menuKeys[0];
    if(argc > 1){
        for(int i = 1; i < argc; i++){
            if(compareCstr("--clean", argv[i])){
                setNoColor();
                choice = "clean";
            }else if(compareCstr("--menu", argv[i])){
                choice = NULL;
            }
        }
    }

    if(choice == NULL){
        RawMode(1);
        choice = menu();
        RawMode(0);
    }

    if(compareCstr("buildeka-only", choice)){
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
    cmd[3] = "-Isrc/api/include";
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
    cmd[2] = "-Isrc/programs/buildeka/include";
    cmd[3] = "-Isrc/base/include";
    cmd[4] = "-Isrc/api/include";
    cmd[5] = "-Iprograms/buildeka/include";
    cmd[6] = "-o";
    cmd[7] = "./dist/bin/buildeka";
    cmd[8] = "./build/libcnkbase/libcnkbase.a";
    cmd[9] = "./src/programs/buildeka/inc.c";
    cmd[10] = "-lm";
    cmd[11] =  NULL;

    if(run("Building buildeka", cmd) == -1){
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

    if(compareCstr("buildeka-only", choice)){
        printf("%s", buildekaExamples);
        fflush(stdout);
    }else if(compareCstr("build-only", choice)){
        printf("Caneka has been built!\n");
        fflush(stdout);
    }else if(compareCstr("clean", choice)){
        printf("Cleaned!\n");
        fflush(stdout);
    }else if(compareCstr("build-cli", choice)){
        printf("The Cli has been built at ./build/bin/clineka\n");
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
